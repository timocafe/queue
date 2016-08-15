//
//  trait.h
//  queue
//
//  Created by Ewart Timothée on 15/07/16.
//
//

#ifndef trait_h
#define trait_h

#include <boost/heap/binomial_heap.hpp>
#include <boost/heap/d_ary_heap.hpp>
#include <boost/heap/priority_queue.hpp>
#include <boost/heap/fibonacci_heap.hpp>
#include <boost/heap/pairing_heap.hpp>
#include <boost/heap/skew_heap.hpp>
#include "tbb/concurrent_priority_queue.h"

enum container {sptq_queue, bin_queue, priority_queue,binomial_heap,
                fibonacci_heap,pairing_heap,skew_heap,d_ary_heap,concurrent_priority_queue};

//name helper printer
template<class D, class... T>
struct name_helper;

template<class T>
struct name_helper<T>{
    static std::string name(){
        return T::name;
    }
};

template<class D, class ...T>
struct name_helper{
    static std::string name(){
        return name_helper<D>::name()+","+name_helper<T...>::name();
    }
};

//benchmark helper execution
template<class F, class D, class... T>
struct benchmark_helper;

template<class F, class D>
struct benchmark_helper<F,D>{
    static std::string benchmark(int size){
        return (std::to_string(F::template benchmark<D>(size))); //execute the bench here
    }
};

template<class F, class D, class ...T>
struct benchmark_helper{
    static std::string benchmark(int size){
        return benchmark_helper<F,D>::benchmark(size)+","
                +benchmark_helper<F,T...>::benchmark(size);
    }
};

//serial queue
template<container q>
struct helper_type;

template<>
struct helper_type<priority_queue>{
    typedef std::priority_queue<double,std::vector<double>,std::greater<double>> value_type;
    constexpr static auto name = "std::priority_queue";
};

template<>
struct helper_type<sptq_queue>{
    typedef tool::sptq_queue<double, std::greater<double>> value_type;
    constexpr static auto name = "original_sptq_queue";
};

template<>
struct helper_type<bin_queue>{ // no comparator great by default
    typedef tool::bin_queue<double> value_type;
    constexpr static auto name = "original_bin_queue";
};

template<>
struct helper_type<binomial_heap>{
    typedef boost::heap::binomial_heap<double, boost::heap::compare<std::greater<double>>> value_type;
    constexpr static auto name = "boost::binomial_heap";
};

template<>
struct helper_type<fibonacci_heap>{
    typedef boost::heap::fibonacci_heap<double, boost::heap::compare<std::greater<double>>> value_type;
    constexpr static auto name = "boost::fibonacci_heap";
};

template<>
struct helper_type<pairing_heap>{
    typedef boost::heap::pairing_heap<double, boost::heap::compare<std::greater<double>>> value_type;
    constexpr static auto name = "boost::pairing_heap";
};

template<>
struct helper_type<skew_heap>{
    typedef boost::heap::skew_heap<double, boost::heap::compare<std::greater<double>>> value_type;
    constexpr static auto name = "boost::skew_heap";
};

template<>
struct helper_type<d_ary_heap>{
    typedef boost::heap::d_ary_heap<double,boost::heap::arity<16>, boost::heap::compare<std::greater<double>>> value_type;
    constexpr static auto name = "boost::d_ary_heap";
};

template<>
struct helper_type<concurrent_priority_queue>{
    typedef tbb::concurrent_priority_queue<double, std::greater<double>>  value_type;
    constexpr static auto name = "tbb::concurrent_priority_queue";
};

//serial queue
template<container q>
struct helper_parallel_type;

template<>
struct helper_parallel_type<concurrent_priority_queue>{
    typedef queue::concurent_lock_free_priority_queue<helper_type<concurrent_priority_queue>::value_type > value_type;
    constexpr static auto name = "tbb::concurrent_priority_queue";
};





#endif /* trait_h */
