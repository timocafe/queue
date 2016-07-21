#captain obvious there is not point to test boost::heap and std::priority_queue
add_compile_options(-std=c++11 -DBOOST_TEST_DYN_LINK)
add_executable(tqueue_test tqueue.cpp)
target_link_libraries(tqueue_test ${Boost_LIBRARIES})
add_test(tqueue_test tqueue_test)
