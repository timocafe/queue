#captain obvious there is not point to test boost::heap and std::priority_queue
add_compile_options(-std=c++11 -DBOOST_TEST_DYN_LINK)
add_executable(queue_test test_queue.cpp)
target_link_libraries(queue_test ${Boost_LIBRARIES})
add_test(queue_test queue_test)
