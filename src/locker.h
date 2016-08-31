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

#include  <mutex>
#include <thread>

namespace tool{

#ifdef _OPENMP
    struct omp_mutex{
        omp_mutex(){
            omp_init_lock(&omp_mtx);
        }

        ~omp_mutex(){
            omp_destroy_lock(&omp_mtx);
        }

        void lock(){
            omp_set_lock(&omp_mtx);
        }

        void unlock(){
            omp_unset_lock(&omp_mtx);
        }

        omp_lock_t omp_mtx;
    };

    typedef omp_mutex QUEUE_MUTEX_TYPE;
#else
    typedef std::mutex QUEUE_MUTEX_TYPE;
#endif

} //end namespace
#endif /* locker_h */
