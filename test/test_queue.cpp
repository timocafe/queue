#define BOOST_TEST_MODULE QueueTEST

#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>
#include <array>
#include <numeric>
#include <random>
#include <iostream>

#include "sptq_queue.hpp"
#include "bin_queue.hpp"

//Test only MH, not std or boost, captain obvious


typedef boost::mpl::list<tool::sptq_queue<int,std::greater<int>>,
                         tool::sptq_queue<float,std::greater<float>>,
                         tool::sptq_queue<double,std::greater<double>>,
                         tool::bin_queue<int>,
                         tool::bin_queue<float>,
                         tool::bin_queue<double> > full_test_types;


BOOST_AUTO_TEST_CASE_TEMPLATE(constructor,T,full_test_types) {
    typedef T value_type;
    value_type queue;
    BOOST_CHECK_EQUAL(queue.size(),0.);
    BOOST_CHECK_EQUAL(queue.top(),0.); 
}

BOOST_AUTO_TEST_CASE_TEMPLATE(push_pop,T,full_test_types) {
    typedef T value_type;
    value_type queue;
    queue.push(1);
    BOOST_CHECK_EQUAL(queue.size(),1); // integer
    BOOST_CHECK_EQUAL(queue.top(),1); // correct because 1 has a exact IEEE representation
    queue.pop();
    BOOST_CHECK_EQUAL(queue.size(),0); // integer
}

BOOST_AUTO_TEST_CASE(push_pop_random_less) {
    tool::sptq_queue<int> q; // std::less by default
    std::array<int,10> a;
    std::iota(a.begin(),a.end(),0);
    std::random_shuffle(std::begin(a), std::end(a));
    auto it = a.begin();
    // fill the queue and sort
    while(it != std::end(a)){
        q.push(*it);
        it++;
    }

    int total(10);
    while(!q.empty()){
        int top = q.top();
        q.pop();
        BOOST_CHECK_EQUAL(top,--total); // carefull --total != total-- here
    }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(push_pop_random_greater,T,full_test_types) {
    typedef T value_type;
    typedef typename T::value_type nested_value_type;

    value_type queue;
    std::array<nested_value_type,10> a;
    std::iota(a.begin(),a.end(),0);
    std::random_shuffle(std::begin(a), std::end(a));
    auto it = a.begin();
    // fill the queue and sort
    while(it != std::end(a)){
        queue.push(*it);
        it++;
    }

    int total(0);
    while(!queue.empty()){
        int top = queue.top();
        queue.pop();
        BOOST_CHECK_EQUAL( top, total++);
    }
}