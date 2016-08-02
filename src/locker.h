//
//  locker.h
//  queue
//
//  Created by Tim Ewart on 02/08/2016.
//
//

#ifndef locker_h
#define locker_h

#ifdef _OPENMP
#include "omp.h"
#endif

namespace tool{

#ifdef _OPENMP
omp_lock_t mtx;
#else
std::mutex mtx;
#endif

#ifdef _OPENMP
struct omp_lock_guard {
    inline omp_lock_guard(omp_lock_t &lock):lock_(lock){
        omp_init_lock(&lock_);
    }

    inline ~omp_lock_guard(){
        omp_destroy_lock(&lock_);
    }

    omp_lock_t& lock_;
};

#endif

} //end namespace
#endif /* locker_h */
