#include <iostream>
#include <fstream>

#include <random>
#include <chrono>
#include <vector>
#include <list>
#include <set>
#include <sstream>
#include <iomanip>

#include "tqueue.hpp"
#include "trait.h"
#include "timer_asm.h"

namespace queue{

    struct push_helper{
        template<class T>
        static double benchmark(int size){
            using value_type = typename T::value_type;
            std::default_random_engine generator;
            std::uniform_real_distribution<double> distribution(0.0,1.0);
            unsigned long long int t1(0),t2(0),time(0);

            for(int j=0; j<10;++j){
                value_type queue;
                t1 = rdtsc();
                    for(int i = 0; i < size ; ++i)
                        queue.push(distribution(generator));
                t2 = rdtsc();
                time += (t2 - t1);
            }

            return time*0.1;
        }

        constexpr static auto name = "push";
    };

    struct pop_helper{
        template<class T>
        static double benchmark(int size){
            using value_type = typename T::value_type;
            std::default_random_engine generator;
            std::uniform_real_distribution<double> distribution(0.0,1.0);
            unsigned long long int t1(0),t2(0),time(0);

            for(int j=0; j<10;++j){
                value_type queue;
                for(int i = 0; i < size ; ++i)
                    queue.push(distribution(generator));

                t1 = rdtsc();
                while(!queue.empty())
                    queue.pop();
                t2 = rdtsc();

                time += (t2 - t1);
            }
            return  time*0.1;
        }

        constexpr static auto name = "pop";
    };

    struct push_one_helper{
            template<class T>
            static double benchmark(int size){
            using value_type = typename T::value_type;
            unsigned long long int t1(0),t2(0),time(0);
            std::default_random_engine generator;
            std::uniform_real_distribution<double> distribution(0.0,1.0);

            for(int j=0; j<10;++j){
                value_type queue;
                for(int i = 0; i < size ; ++i)
                    queue.push(distribution(generator));

                t1 = rdtsc();
                queue.push(distribution(generator));
                t2 = rdtsc();
                time += (t2 - t1);
            }

            return time*0.1;
        }

        constexpr static auto name = "push_one";
    };

    template<class F, class ...T>
    void benchmark(int iteration){
        int size(1);
        std::list<std::string> res;

        res.push_back("#elements," + name_helper<T...>::name() + "\n");

        for(int i=1; i< iteration; ++i){
            std::string results = std::to_string(size) + ",";
            results += benchmark_helper<F,T...>::benchmark(size) + "\n";
            res.push_back(results);
            size<<=1; // 1,2,4,8 ....
        }
        //start IO
        std::fstream out;
        std::string name = std::string(F::name) + ".csv";
        out.open(name,std::fstream::out);
        std::copy(res.begin(),res.end(), std::ostream_iterator<std::string>(out));
    }
} // end name space

int main(int argc, char* argv[]){
    int size = std::atoi(argv[1]);
    //queue types
    using t0 = helper_type<priority_queue>;
    using t1 = helper_type<spltree>;
    using t2 = helper_type<binomial_heap>;
    using t3 = helper_type<fibonacci_heap>;
    using t4 = helper_type<pairing_heap>;
    using t5 = helper_type<skew_heap>;
    //bench types
    using push = queue::push_helper;
    using pop = queue::pop_helper;
    using push_one = queue::push_one_helper;
    //benchmarks
    queue::benchmark<push,t0,t1,t2,t3,t4,t5>(size);
    queue::benchmark<pop,t0,t1,t2,t3,t4,t5>(size);
    queue::benchmark<push_one,t0,t1,t2,t3,t4,t5>(size);

    return 0;
}
