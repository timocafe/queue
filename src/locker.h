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
struct omp_mutex{
    omp_mutex(){
        omp_init_lock(&mtx);
    }

    ~omp_mutex(){
        omp_destroy_lock(&mtx);
    }

    omp_lock_t mtx;
};
omp_mutex mtx;
#else
std::mutex mtx;
#endif

#ifdef _OPENMP
struct omp_lock_guard {
    inline omp_lock_guard(omp_mutex &lock):lock_(lock){
        omp_set_lock(&lock_.mtx);
    }

    inline ~omp_lock_guard(){
        omp_unset_lock(&lock_.mtx);
    }

    omp_mutex& lock_;
};

#endif

} //end namespace
#endif /* locker_h */
