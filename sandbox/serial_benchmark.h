//
//  push_pop.h
//  queue
//
//  Created by Tim Ewart on 02/08/2016.
//
//

#ifndef serial_benchmark_h
#define serial_benchmark_h

namespace queue{

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
    
} //end namespace

#endif /* push_pop_h */
