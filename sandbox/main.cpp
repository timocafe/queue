#include <iostream>
#include <fstream>

#include <random>
#include <chrono>
#include <vector>
#include <list>
#include <stack>

#include <set>
#include <sstream>
#include <thread>
#include <iomanip>

#if defined(_OPENMP)
#include <omp.h>
#endif

#include "tbb/tbb.h"
#include "locker.h"


#include "timer_asm.h"

#include "sptq_queue.hpp"
#include "bin_queue.hpp"

#include "trait.h"
#include "push_pop.h" //benchmark push/pop

namespace queue{



    struct mhines_bench_helper {

        template<class T>
        static double benchmark(int size, int repetition = 2){
            repetition=1;
            using value_type = typename T::value_type;
            std::default_random_engine generator;
            std::uniform_real_distribution<double> distribution(0.5,2.0);
            unsigned long long int t1(0),t2(0),time(0);

            typename value_type::value_type value; // this is a double
            bool state; // for try pop no really relevant with serial queue

            const double dt = 0.025;
            const double max_time = 50.0;

            for(int j=0; j<repetition; ++j){
                value_type queue;
                t1 = rdtsc();

                for(auto t=0.0; t < max_time; t += dt){
                    for(int i = 0; i < size ; ++i)
                        queue.push((t + distribution(generator)));

                    do {
                        state = try_pop(queue,value);
                        if(value > t){
                            queue.push(t);
                            state = false;
                        }
                    }while (state);

                    t += dt;
                }


                t2 = rdtsc();
                time += (t2 - t1);
            }

            return time*1/static_cast<double>(repetition);
        }

        constexpr static auto name = "mh";
    };


    template<class F, class ...T>
    void benchmark(int iteration, int size = 16){

        std::list<std::string> res(1,"#elements," + name_helper<T...>::name() + "\n");

        for(int i=1; i< iteration; ++i){
            std::string results = std::to_string(size) + ",";
            results += benchmark_helper<F,T...>::benchmark(size) + "\n";
            res.push_back(results);
            size<<=1; // 1,2,4,8 ....
        }
        //start IO
        std::fstream out;
        out.open(std::string(F::name) + ".csv",std::fstream::out);
        std::copy(res.begin(),res.end(), std::ostream_iterator<std::string>(out));
    }
} // end name space

//sequential benchmark to test the queue under serial mode
void sequential_benchmark(int iteration = 10){
    //queue types
    using t0 = helper_type<priority_queue>;
    using t1 = helper_type<sptq_queue>;
    using t2 = helper_type<bin_queue>;
    using t3 = helper_type<binomial_heap>;
    using t4 = helper_type<fibonacci_heap>;
    using t5 = helper_type<pairing_heap>;
    using t6 = helper_type<skew_heap>;
    using t7 = helper_type<d_ary_heap>;
    using t8 = helper_type<concurrent_priority_queue>;
    //bench types
    using push = queue::push_helper;
    using pop = queue::pop_helper;
    using push_one = queue::push_one_helper;
    using mh = queue::mhines_bench_helper;
    //benchmarks
    queue::benchmark<push,t0,t1,t2,t3,t4,t5,t6,t7>(iteration);
    queue::benchmark<pop,t0,t1,t2,t3,t4,t5,t6,t7>(iteration);
    queue::benchmark<push_one,t0,t1,t2,t3,t4,t5,t6,t7>(iteration);
    queue::benchmark<mh,t0,t1,t2,t3,t4,t5,t6,t7,t8>(iteration);
}

template<class Q, class M = std::lock_guard<std::mutex>>
struct concurent_priority_queue{
    typedef typename Q::value_type value_type;
    typedef M mutex_type;

    concurent_priority_queue(std::size_t tid):my_id(tid){}

    void enqueue(size_t tid, value_type value){ // TO DO, more than double & and && version
        if(iam(tid)) // I am myself push directly
            queue.push(value);
        else{ // I am not I am somebody else
            mutex_type lock(tool::mtx);
            inter_buffer.push(value);
        }
    }

    void merge(){
        mutex_type lock(tool::mtx); //lock guard pattern
        while(!inter_buffer.empty()){
            value_type value = inter_buffer.top();
            queue.push(value);
            inter_buffer.pop();
        }
    }

    bool iam(std::size_t tid){
        return (tid == my_id);
    }

    bool dequeue(value_type& value){
        mutex_type lock(tool::mtx);
        bool b=!queue.empty();
        if(b){
            value=queue.top();
            queue.pop();
        }
        return b;
    }

    Q queue;
    std::size_t my_id;
    std::stack<value_type> inter_buffer;
};

template<container c> // works with all priority_queue
struct benchmark{
    benchmark(std::size_t group, std::size_t event=1024):size_group(group),size_event(event),v(group,0){
        std::iota(v.begin(),v.end(),0); // give an id to every queue
        dt = 0.025;
        max_time = 50;
        distribution = std::uniform_real_distribution<double>(0.5,2);
        distribution_int = std::uniform_int_distribution<int>(0,group-1);
    }

    void operator()(size_t tid){
        bool state(false); // angry programmer
        for(auto t=0.0; t < max_time; t += dt){
            double value(0);


            //enqueue in CNeuron
            for(size_t i = 0; i < size_event/10; ++i)
                v.at(distribution_int(generator)).enqueue(tid,(t + distribution(generator))); //mixup my event + interevent

            v.at(tid).merge();

            std::this_thread::sleep_for(std::chrono::milliseconds(10)); // we do something

            //deliver event in CNeuron
            do {
                state = v.at(tid).dequeue(value);
                if(value > t){
                    v.at(tid).enqueue(tid,t);
                    state = false;
                    std::this_thread::sleep_for(std::chrono::milliseconds(10)); // we do something
                }
            } while(state);

            t += dt;
        }
    }

    size_t size(){
        return v.size();
    }

    std::size_t size_group, size_event;
    double dt;
    double max_time;
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution;
    std::uniform_int_distribution<int> distribution_int;
    std::vector<concurent_priority_queue<typename helper_type<c>::value_type>> v;
};

int main(int argc, char* argv[]){

    //time
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_seconds;
    //command line
    size_t size = std::atoi(argv[1]);

    {
        benchmark<priority_queue> a(size);
        start = std::chrono::system_clock::now();
            tbb::parallel_for( size_t(0), a.size(), [&](size_t i){a(i);} );
        end = std::chrono::system_clock::now();
        elapsed_seconds = end-start;
        std::cout << "elapsed time parallel tbb: " << elapsed_seconds.count() << "[s]\n";
    }

#if defined(_OPENMP)
    {
        benchmark<concurrent_priority_queue> a(size);
        start = std::chrono::system_clock::now();
        #pragma omp parallel for
        for(int i=0; i < size; ++i)
            a(i);
        end = std::chrono::system_clock::now();
        elapsed_seconds = end-start;
        std::cout << "elapsed time parallel omp: " << elapsed_seconds.count() << "[s]\n";
    }
#endif

//    {
//        benchmark<priority_queue> a(size);
//        start = std::chrono::system_clock::now();
//        for(int i=0; i < size; ++i)
//            a(i);
//        end = std::chrono::system_clock::now();
//        elapsed_seconds = end-start;
//        std::cout << "elapsed time serial: " << elapsed_seconds.count() << "[s]\n";
//    }

    return 0;
}
