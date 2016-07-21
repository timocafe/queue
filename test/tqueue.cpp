#define BOOST_TEST_MODULE QueueTEST
#include <boost/test/unit_test.hpp>

#include "tqueue.hpp"
//Test only MH, not std or boost, captain obvious
BOOST_AUTO_TEST_CASE(constructor) {
    sptq::TQueue<> queue;
    BOOST_CHECK_EQUAL(queue.size(),0); // integer
    BOOST_CHECK_EQUAL(queue.top(),0.); //by default double
}
