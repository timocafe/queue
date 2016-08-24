
#ifndef hybrid_queue_h
#define hybrid_queue_h

#include <thread>
#include <boost/lockfree/stack.hpp>
#include "tbb/tbb.h"
#include "locker.h"

namespace queue{

//pure mutex version, the queue can be whatever you want
    template<class Q, class M = std::lock_guard<tool::QUEUE_MUTEX_TYPE>>
struct concurent_priority_queue{
    typedef Q container_type;
    typedef typename Q::value_type value_type;
    typedef M mutex_type;

    concurent_priority_queue():my_id(0){} // my id must be independenant i.e. the entry of the vectotr

    void enqueue(size_t tid, value_type value){ // TO DO, more than double & and && version
        if(iam(tid)) // I am myself push directly
            queue.push(value);
        else { /* I am not I am somebody else */
            mutex_type lock(tool::mtx);
            inter_buffer.push(value);
        }
    }

    void merge(){
        mutex_type lock(tool::mtx); //lock guard pattern
        while(!inter_buffer.empty()){
            value_type value = inter_buffer.top();
            queue.push(value);
            inter_buffer.pop();
        }
    }

    bool iam(std::size_t tid){
        return (tid == my_id);
    }

    bool dequeue(value_type& value,double t){
        bool b = (!queue.empty() && queue.top() <= t);
        if(b)
            queue.pop(); 
        return b;
    }

    container_type queue;
    std::size_t my_id;
    std::stack<value_type> inter_buffer;
};

// hybrid version where I use a lock free boost stack
template<class Q, class M = std::lock_guard<tool::QUEUE_MUTEX_TYPE>>
struct concurent_partial_lock_free_priority_queue{
    typedef Q container_type;
    typedef typename Q::value_type value_type;
    typedef M mutex_type;

    concurent_partial_lock_free_priority_queue():my_id(0){}

    void enqueue(size_t tid, value_type value){ // TO DO, more than double & and && version
        if(iam(tid)) // I am myself push directly
            queue.push(value);
        else // I am not I am somebody else
            while(!inter_buffer.push(value))
                ;
    }

    void merge(){
        mutex_type lock(tool::mtx); //lock guard pattern
        value_type value;
        while(inter_buffer.pop(value))
            queue.push(value);
    }

    bool iam(std::size_t tid){
        return (tid == my_id);
    }

    bool dequeue(value_type& value,double t){
        merge();
        bool b = (!queue.empty() && queue.top() <= t);
        if(b)
            queue.pop();
        return b;
    }

    container_type queue;
    std::size_t my_id;
    boost::lockfree::stack<double> inter_buffer;
};

// put lock free version
template<class Q>
struct concurent_lock_free_priority_queue{
    typedef Q container_type;
    typedef typename Q::value_type value_type;

    void enqueue(size_t tid = 0, value_type value = 0){ // tid u
        queue.push(value);
    }

    bool dequeue(value_type& value,double t){
        bool b = queue.try_pop(value);
        if(value > t){
            queue.push(t);
            b = false;
        }
        return b;
    }

    std::size_t my_id;
    container_type queue;
};

}//end namespace

#endif

