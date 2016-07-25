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

namespace queue{

    static __inline__ unsigned long long rdtsc(void){
        unsigned hi, lo;
        __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
        return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32   );
    }

    template<container q>
    double benchmark_queue_push(int size){
        std::default_random_engine generator;
        std::uniform_real_distribution<double> distribution(0.0,1.0);
        unsigned long long int t1(0),t2(0),time(0);

        for(int j=0; j<10;++j){
            typename helper_type<q>::queue_type queue;
            t1 = rdtsc();
                for(int i = 0; i < size ; ++i)
                    queue.push(distribution(generator));
            t2 = rdtsc();
            time += (t2 - t1);
        }

        return time*0.1;
    }

    template<container q>
    double benchmark_queue_push_one(int size){
        unsigned long long int t1(0),t2(0),time(0);
        std::default_random_engine generator;
        std::uniform_real_distribution<double> distribution(0.0,1.0);

        for(int j=0; j<10;++j){
            typename helper_type<q>::queue_type queue;
            for(int i = 0; i < size ; ++i)
                queue.push(distribution(generator));

            t1 = rdtsc();
            queue.push(distribution(generator));
            t2 = rdtsc();
            time += (t2 - t1);
        }

        return time*0.1;
    }

    template<container q>
    double benchmark_queue_pop(int size){
        std::default_random_engine generator;
        std::uniform_real_distribution<double> distribution(0.0,1.0);
        unsigned long long int t1(0),t2(0),time(0);

        for(int j=0; j<10;++j){
            typename helper_type<q>::queue_type queue;
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


    void benchmark_push(int iteration){
        int size(1);
        std::list<std::string> res;
        res.push_back("#elements,std::priority_queue,mh_splay,boost::binomial_heap,boost::fibonacci_heap,boost::pairing_heap,boost::skew_heap \n");

        for(int i=1; i< iteration; ++i){
            std::string bench = std::to_string(size) + ",";
            bench += std::to_string(queue::benchmark_queue_push<container::priority_queue>(size)) + ",";
            bench += std::to_string(queue::benchmark_queue_push<container::spltree>(size)) + ",";
            bench += std::to_string(queue::benchmark_queue_push<container::binomial_heap>(size)) + ",";
            bench += std::to_string(queue::benchmark_queue_push<container::fibonacci_heap>(size)) + ",";
            bench += std::to_string(queue::benchmark_queue_push<container::pairing_heap>(size)) + ",";
            bench += std::to_string(queue::benchmark_queue_push<container::skew_heap>(size)) + "\n";
            res.push_back(bench);
            size<<=1; // 1,2,4,8 ....
        }
        std::fstream out;
        out.open("push.csv",std::fstream::out);
        std::copy(res.begin(),res.end(), std::ostream_iterator<std::string>(out));
    }


    void benchmark_push_one(int iteration){
        int size(100000);
        std::list<std::string> res;
        res.push_back("#elements,std::priority_queue,mh_splay,boost::binomial_heap,boost::fibonacci_heap,boost::pairing_heap,boost::skew_heap \n");

        std::string bench = std::to_string(size) + ",";
        bench += std::to_string(queue::benchmark_queue_push_one<container::priority_queue>(size)) + ",";
        bench += std::to_string(queue::benchmark_queue_push_one<container::spltree>(size)) + ",";
        bench += std::to_string(queue::benchmark_queue_push_one<container::binomial_heap>(size)) + ",";
        bench += std::to_string(queue::benchmark_queue_push_one<container::fibonacci_heap>(size)) + ",";
        bench += std::to_string(queue::benchmark_queue_push_one<container::pairing_heap>(size)) + ",";
        bench += std::to_string(queue::benchmark_queue_push_one<container::skew_heap>(size)) + "\n";
        res.push_back(bench);

        std::fstream out;
        out.open("push_one.csv",std::fstream::out);
        std::copy(res.begin(),res.end(), std::ostream_iterator<std::string>(out));
    }


    void benchmark_pop(int iteration){
        int size(1);
        std::list<std::string> res;
        res.push_back("#elements,std::priority_queue,mh_splay,boost::binomial_heap,boost::fibonacci_heap,boost::pairing_heap,boost::skew_heap \n");

        for(int i=1; i< iteration; ++i){
            std::string bench = std::to_string(size) + ",";
            bench += std::to_string(queue::benchmark_queue_pop<container::priority_queue>(size)) + ",";
            bench += std::to_string(queue::benchmark_queue_pop<container::spltree>(size)) + ",";
            bench += std::to_string(queue::benchmark_queue_pop<container::binomial_heap>(size)) + ",";
            bench += std::to_string(queue::benchmark_queue_pop<container::fibonacci_heap>(size)) + ",";
            bench += std::to_string(queue::benchmark_queue_pop<container::pairing_heap>(size)) + ",";
            bench += std::to_string(queue::benchmark_queue_pop<container::skew_heap>(size)) + "\n";
            res.push_back(bench);
            size<<=1; // 1,2,4,8 ....
        }
        std::fstream out;
        out.open("pop.csv", std::fstream::out);
        std::copy(res.begin(),res.end(), std::ostream_iterator<std::string>(out));
    }
} // end name space

int main(int argc, char* argv[]){
    int iteration = std::atoi(argv[1]);

    queue::benchmark_push(iteration);
    queue::benchmark_push_one(iteration);
    queue::benchmark_pop(iteration);

    return 0;
}
