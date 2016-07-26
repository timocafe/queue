cmake_minimum_required (VERSION 2.8.12)
project (queue)
find_package(Boost 1.49 REQUIRED unit_test_framework)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(src)

option (QUEUE_TESTS "Build the regression tests" OFF)

if(QUEUE_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif(QUEUE_TESTS)

add_subdirectory(sandbox)
