
namespace queue{


struct benchmark_partial_lockfree{

    template<class T> // works with all priority
    static double benchmark(size_t size, int repetition = 1){

        using value_type = typename T::value_type;
        std::default_random_engine generator;
        std::uniform_real_distribution<double> distribution(0.5,2.0);
        std::uniform_int_distribution<int> distribution_int(0,99);

        unsigned long long int t1(0),t2(0),time(0);
        const double dt = 0.025;
        const double max_time = 50.0;

        std::vector<value_type> v_queue(100); // vector of queue

        for(int i(0); i < 100;++i)
            v_queue[i].my_id = i; // my_id useless for tbb

        t1 = rdtsc();

        for(auto t=0.0; t < max_time; t += dt){
            tbb::parallel_for( size_t(0),  v_queue.size(), [&](size_t tid){
                double value(0);

                for(size_t i = 0; i < size; ++i)
                    v_queue.at(distribution_int(generator)).enqueue(tid,(t + distribution(generator))); //mixup my event + interevent

                merge(v_queue.at(tid)); //SFINAE this is skiped for TBB queue version

                while(v_queue.at(tid).dequeue(value,t))

                t += dt;
            }); // end lambda
        }

        t2 = rdtsc();
        time = t2 -t1;
        return time;

    }

    constexpr static auto name = "mhth";
};

}
