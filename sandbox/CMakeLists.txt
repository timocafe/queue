add_compile_options(-std=c++11 -mavx)

if(QUEUE_BUILD_OMP)
    add_compile_options(${OpenMP_CXX_FLAGS})
endif()
add_executable(queue main.cpp)
target_link_libraries(queue ${TBB_LIBRARY})
if(QUEUE_BUILD_OMP)
    target_link_libraries(queue ${OpenMP_CXX_FLAGS})
endif()
