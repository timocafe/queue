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

enum container {spltree, priority_queue,binomial_heap,fibonacci_heap,pairing_heap,skew_heap};

template<container q>
struct helper_type;

template<>
struct helper_type<priority_queue>{
    typedef std::priority_queue<double,std::vector<double>,std::greater<double>> queue_type;
};

template<>
struct helper_type<spltree>{
    typedef TQueue<double, std::greater<double>> queue_type;
};

template<>
struct helper_type<binomial_heap>{
    typedef boost::heap::binomial_heap<double, boost::heap::compare<std::greater<double>>> queue_type;
};

template<>
struct helper_type<fibonacci_heap>{
    typedef boost::heap::fibonacci_heap<double, boost::heap::compare<std::greater<double>>> queue_type;
};

template<>
struct helper_type<pairing_heap>{
    typedef boost::heap::pairing_heap<double, boost::heap::compare<std::greater<double>>> queue_type;
};

template<>
struct helper_type<skew_heap>{
    typedef boost::heap::skew_heap<double, boost::heap::compare<std::greater<double>>> queue_type;
};

#endif /* trait_h */
