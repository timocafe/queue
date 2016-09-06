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

#include "serial_benchmark.h" //benchmark push/pop
#include "parallel_benchmark.h" //benchmark mh //

#include "tbb/tbb.h"

namespace queue{

    template<class F, class ...T>
    void benchmark(int iteration, int size = 1, std::string prefix = std::string()){
        std::string filename = std::string(prefix+F::name) + ".csv";
        std::list<std::string> res(1,"#elements," + name_helper<T...>::name() + "\n");

        for(int i=1; i< iteration; ++i){
            std::string results = std::to_string(size) + ",";
            results += benchmark_helper<F,T...>::benchmark(size) + "\n";
            res.push_back(results);
            size<<=1; // 1,2,4,8 ....
        }
        //start IO
        std::fstream out;
        out.open(filename,std::fstream::out);
        std::copy(res.begin(),res.end(), std::ostream_iterator<std::string>(out));
    }
} // end name space

//sequential benchmark to test the queue under serial mode
void benchmarks(int iteration = 10){
    //queue types serial
    using t0 = helper_type<priority_queue>;
    using t1 = helper_type<sptq_queue>;
    using t2 = helper_type<bin_queue>;
    using t3 = helper_type<binomial_heap>;
    using t4 = helper_type<fibonacci_heap>;
    using t5 = helper_type<pairing_heap>;
    using t6 = helper_type<skew_heap>;
    using t7 = helper_type<d_ary_heap>;
    //queue types parallel
    using tp0 = helper_parallel_type<priority_queue>; // pure mutex + stack
    using tp1 = helper_parallel_type<concurrent_stack>; //  // less mutex + boost::stack
    using tp2 = helper_parallel_type<concurrent_priority_queue>; // tbb
    //bench types
    using push = queue::push_helper;
    using pop = queue::pop_helper;
    using push_one = queue::push_one_helper;
    using mh = queue::mhines_bench_helper;
    using mh_partial_lockfree  = queue::benchmark_partial_lockfree;
    //benchmarks
    queue::benchmark<push,t0,t1,t2,t3,t4,t5,t6,t7>(iteration);
    queue::benchmark<pop,t0,t1,t2,t3,t4,t5,t6,t7>(iteration);
    queue::benchmark<push_one,t0,t1,t2,t3,t4,t5,t6,t7>(iteration);
    queue::benchmark<mh,t0,t1,t2,t3,t4,t5,t6,t7>(iteration);
    queue::benchmark<mh_partial_lockfree,tp0,tp1,tp2>(iteration);

}

int main(int argc, char* argv[]){
//    int rep = atoi(argv[1]);
//    benchmarks(rep);
    tool::sptq_queue<double,std::greater<double> > q;
    q.push(1);
    q.push(9);
    q.push(2);
    q.push(501);
    q.push(123);
    q.push(235);
    q.push(2);
    q.push(-1);
    q.push(0);
    std::cout << q << std::endl;
    return 0;
}
