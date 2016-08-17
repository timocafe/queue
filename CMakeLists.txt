cmake_minimum_required (VERSION 2.8.12)
project (queue)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake)
FIND_PACKAGE(Boost 1.49 REQUIRED unit_test_framework)

option(QUEUE_BUILD_OMP "Build queue with openmp" OFF)
if(QUEUE_BUILD_OMP)
FIND_PACKAGE(OpenMP)
endif()


FIND_PACKAGE(TBB)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${TBB_INCLUDE_DIRS})
include_directories(src)


option (QUEUE_TESTS "Build the regression tests" OFF)

if(QUEUE_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif(QUEUE_TESTS)

add_subdirectory(sandbox)
