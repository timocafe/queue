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

#include "timer_asm.h"

#include "sptq_queue.hpp"
#include "bin_queue.hpp"
#include "hybrid_queue.h"
#include "trait.h"

#include "push_pop.h" //benchmark push/pop

#include "tbb/tbb.h"
namespace queue{

    struct mhines_bench_helper {

        template<class T>
        static double benchmark(int size, int repetition = 2){
            repetition=1;
            using value_type = typename T::value_type;
            std::default_random_engine generator;
            std::uniform_real_distribution<double> distribution(0.5,2.0);
            unsigned long long int t1(0),t2(0),time(0);

            const double dt = 0.025;
            const double max_time = 50.0;

            for(int j=0; j<repetition; ++j){
                value_type queue;
                t1 = rdtsc();

                for(auto t=0.0; t < max_time; t += dt){
                    for(int i = 0; i < size ; ++i)
                        queue.push((t + distribution(generator)));

                    while (queue.top() <= t)
                        queue.pop();

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


    // ok to make the glue with variadic template
    struct parallel_helper{
        template<class T> // T here is the queue type for the parallel
        static double benchmark(int size=4){ // size it the number of entry for the thread bufffer or cell group for neuron
            unsigned long long int t1(0),t2(0),time(0);
            benchmark_partial_lockfree<T> a(size);
            a.benchmark(0);
            t1 = rdtsc();
            tbb::parallel_for( size_t(0), size, [&](size_t i){ a.benchmark(i) ;} );
            #ifdef _OPENMP
            #pragma omp parallel for
            for(int i=0; i < size; ++i)
                a.benchmark(i);
            #endif
            t2 = rdtsc();
            time = (t2 - t1);
            return time;
        };

        constexpr static auto name = "toto";
    };

} // end name space

//sequential benchmark to test the queue under serial mode
void benchmarks(int iteration = 10){
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
    //parallel queue type on t0 only or t8
    using qcpq = queue::concurent_priority_queue<t0>;
    using qcplf = queue::concurent_partial_lock_free_priority_queue<t0>;
    using qclfpq = queue::concurent_lock_free_priority_queue<t8>; // TBB ONLY !
    // parallel bench
    using mhth = queue::parallel_helper; // do the glue between 
    //benchmarks
//    queue::benchmark<push,t0,t1,t2,t3,t4,t5,t6,t7>(iteration);
//    queue::benchmark<pop,t0,t1,t2,t3,t4,t5,t6,t7>(iteration);
//    queue::benchmark<push_one,t0,t1,t2,t3,t4,t5,t6,t7>(iteration);
//    queue::benchmark<mh,t0,t1,t2,t3,t4,t5,t6,t7>(iteration);


    queue::benchmark<mhth,t0>(4);

}

namespace toto {
void foo(size_t i){
    std::cout << i << std::endl;
}
}


int main(int argc, char* argv[]){
   int rep = atoi(argv[1]);
   benchmarks(rep);
    return 0;
}
