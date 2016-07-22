cmake_minimum_required (VERSION 2.8.12)
project (queue)
find_package(Boost 1.49 REQUIRED unit_test_framework)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(src)

include(CTest)
enable_testing()

add_subdirectory(test)
add_subdirectory(sandbox)
