
namespace queue{

//template<class T> // works with all priority_queue
//struct benchmark_partial_lockfree{
//    benchmark_partial_lockfree(std::size_t group, std::size_t event=1000):size_group(group),size_event(event),v(group){
//        for(int i = 0; i < v.size() ; ++i)
//            v[i].my_id = i;
//
//        dt = 0.025;
//        max_time = 50;
//        distribution = std::uniform_real_distribution<double>(0.5,2);
//        distribution_int = std::uniform_int_distribution<int>(0,group-1);
//    }
//
//    void benchmark(size_t tid){
//        for(auto t=0.0; t < max_time; t += dt){
//            double value(0);
//
//            for(size_t i = 0; i < size_event; ++i)
//                v.at(distribution_int(generator)).enqueue(tid,(t + distribution(generator))); //mixup my event + interevent
//
//            v.at(tid).merge();
//
//            while(v.at(tid).dequeue(value,t));
//
//            t += dt;
//        }
//    }
//
//    size_t size(){
//        return v.size();
//    }
//
//    std::size_t size_group, size_event;
//    double dt;
//    double max_time;
//    std::default_random_engine generator;
//    std::uniform_real_distribution<double> distribution;
//    std::uniform_int_distribution<int> distribution_int;
//    std::vector<T> v;
//
//    constexpr static auto name = "mhth_lockfree";
//};

//evil duplication to change with a specialization
struct benchmark_lockfree{

    template<class T> // works with all priority
    static double benchmark(size_t size, int repetition = 1){
        using value_type = typename T::value_type;
        std::default_random_engine generator;
        std::uniform_real_distribution<double> distribution(0.0,1.0);
        std::uniform_int_distribution<double> distribution_integer(0.0,1.0);

        unsigned long long int t1(0),t2(0),time(0);
        const double dt = 0.025;
        const double max_time = 50.0;

        std::vector<value_type> v_queue(100); // vector or queue
        t1 = rdtsc();

        tbb::parallel_for( size_t(0), size, [&](size_t tid){
            for(auto t=0.0; t < max_time; t += dt){
                double value(0);

                for(size_t i = 0; i < size; ++i)
                    v_queue.at(distribution_integer(generator)).enqueue((t + distribution(generator))); //mixup my event + interevent

                while(v_queue.at(tid).dequeue(value,t));

                t += dt;
            }
        }); //end lambda

        t2 = rdtsc();
        time = t2 -t1;
        return time;

    }

    constexpr static auto name = "mhth_lockfree";
};


}
