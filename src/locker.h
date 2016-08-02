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
            omp_init_lock(&omp_mtx);
        }

        ~omp_mutex(){
            omp_destroy_lock(&omp_mtx);
        }

        omp_lock_t omp_mtx;
    };

    omp_mutex mtx; // global object

    struct omp_lock_guard {
        inline omp_lock_guard(omp_mutex &mtx):mtx_(mtx){
            omp_set_lock(&mtx_.omp_mtx);
        }

        inline ~omp_lock_guard(){
            omp_unset_lock(&mtx_.omp_mtx);
        }

        omp_mutex& mtx_;
    };
#else
    std::mutex mtx;
#endif

} //end namespace
#endif /* locker_h */
