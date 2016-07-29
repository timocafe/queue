#include <iostream>
#include <fstream>

#include <random>
#include <chrono>
#include <vector>
#include <list>
#include <set>
#include <sstream>
#include <iomanip>

#include "sptq_queue.hpp"
#include "bin_queue.hpp"
#include "trait.h"
#include "timer_asm.h"

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
                for(auto t=0.0; t < max_time; t += dt){
                    t1 = rdtsc();
                    for(int i = 0; i < size ; ++i)
                        queue.push(t + distribution(generator));

                    while (queue.top() <= t)
                        queue.pop();

                    t += dt;
                    t2 = rdtsc();
                    time += (t2 - t1);
                }
            }
            return time*1/static_cast<double>(repetition);
        }

        constexpr static auto name = "mh";
    };

    struct push_helper{
        template<class T>
        static double benchmark(int size, int repetition = 5){
            using value_type = typename T::value_type;
            std::default_random_engine generator;
            std::uniform_real_distribution<double> distribution(0.0,1.0);
            unsigned long long int t1(0),t2(0),time(0);
            value_type queue;

            for(int j=0; j<repetition; ++j){
                t1 = rdtsc();
                for(int i = 0; i < size ; ++i)
                    queue.push(distribution(generator));
                t2 = rdtsc();

                while(!queue.empty())
                    queue.pop();

                time += (t2 - t1);
            }
            return time*1/static_cast<double>(repetition);
        }

        constexpr static auto name = "push";
    };

    struct pop_helper{
        template<class T>
        static double benchmark(int size, int repetition = 5){
            using value_type = typename T::value_type;
            std::default_random_engine generator;
            std::uniform_real_distribution<double> distribution(0.0,1.0);
            unsigned long long int t1(0),t2(0),time(0);
            value_type queue;

            for(int j=0; j<repetition; ++j){
                for(int i = 0; i < size ; ++i)
                    queue.push(distribution(generator));

                t1 = rdtsc();
                while(!queue.empty())
                    queue.pop();
                t2 = rdtsc();

                time += (t2 - t1);
            }
            return time*1/static_cast<double>(repetition);
        }

        constexpr static auto name = "pop";
    };

    struct push_one_helper{
        template<class T>
        static double benchmark(int size, int repetition = 10){
            using value_type = typename T::value_type;
            unsigned long long int t1(0),t2(0),time(0);
            std::default_random_engine generator;
            std::uniform_real_distribution<double> distribution(0.0,1.0);
            value_type queue;

            for(int i = 0; i < size-1 ; ++i) // size-1 because to avoid realloc
                queue.push(distribution(generator));

            for(int j=0; j<repetition; ++j){
                typename value_type::value_type value = distribution(generator);
                t1 = rdtsc();
                queue.push(value);
                t2 = rdtsc();
                time += (t2 - t1);
                queue.pop(); // keep the number of element constant
            }
            return time*1/static_cast<double>(repetition);
        }

        constexpr static auto name = "push_one";
    };

    template<class F, class ...T>
    void benchmark(int iteration, int size = 1){

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

int main(int argc, char* argv[]){
    int iteration = std::atoi(argv[1]);
    //queue types
    using t0 = helper_type<priority_queue>;
    using t1 = helper_type<sptq_queue>;
    using t2 = helper_type<bin_queue>;
    using t3 = helper_type<binomial_heap>;
    using t4 = helper_type<fibonacci_heap>;
    using t5 = helper_type<pairing_heap>;
    using t6 = helper_type<skew_heap>;
    using t7 = helper_type<d_ary_heap>;
    //bench types
    using push = queue::push_helper;
    using pop = queue::pop_helper;
    using push_one = queue::push_one_helper;
    using mh = queue::mhines_bench_helper;
    //benchmarks
    queue::benchmark<push,t0,t1,t2,t3,t4,t5,t6,t7>(iteration);
    queue::benchmark<pop,t0,t1,t2,t3,t4,t5,t6,t7>(iteration);
    queue::benchmark<push_one,t0,t1,t2,t3,t4,t5,t6,t7>(iteration);
    queue::benchmark<mh,t0,t1,t2,t3,t4,t5,t6,t7>(iteration);

    return 0;
}
