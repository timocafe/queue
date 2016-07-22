#define BOOST_TEST_MODULE QueueTEST

#include <boost/test/unit_test.hpp>
#include <array>
#include <numeric>
#include <random>
#include <iostream>

#include "tqueue.hpp"
//Test only MH, not std or boost, captain obvious
BOOST_AUTO_TEST_CASE(constructor) {
    sptq::TQueue<> queue;
    BOOST_CHECK_EQUAL(queue.size(),0); // integer
    BOOST_CHECK_EQUAL(queue.top(),0.); //by default double
}

BOOST_AUTO_TEST_CASE(push_pop) {
    sptq::TQueue<> queue;
    queue.push(1);
    BOOST_CHECK_EQUAL(queue.size(),1); // integer
    BOOST_CHECK_EQUAL(queue.top(),1); //by default double
    queue.pop();
    BOOST_CHECK_EQUAL(queue.size(),0); // integer
}

BOOST_AUTO_TEST_CASE(push_pop_random_less) {
    sptq::TQueue<int> q; // std::less by default
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

BOOST_AUTO_TEST_CASE(push_pop_random_greater) {
    sptq::TQueue<int,std::greater<int>> q;
    std::array<int,10> a;
    std::iota(a.begin(),a.end(),0);
    std::random_shuffle(std::begin(a), std::end(a));
    auto it = a.begin();
    // fill the queue and sort
    while(it != std::end(a)){
        q.push(*it);
        it++;
    }

    int total(0);
    while(!q.empty()){
        int top = q.top();
        q.pop();
        BOOST_CHECK_EQUAL(top,total++); // carefull ++total != total ++ here
    }
}
