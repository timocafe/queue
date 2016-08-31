
#ifndef hybrid_queue_h
#define hybrid_queue_h

#include <thread>
#include <boost/lockfree/stack.hpp>
#include "tbb/tbb.h"
#include "locker.h"

namespace queue{


// http://jguegant.github.io/blogs/tech/sfinae-introduction.html
// Primary template, inherit from std::false_type.
// ::value will return false.
// Note: the second unused template parameter is set to default as void
template <typename T, typename = void> //= std::void_t<> c++17
struct has_merge : std::false_type{ // std::false_type equivalent const static bool value = false; inside the class
};

// Partial template specialisation, inherit from std::true_type.
// ::value will return true.
template <typename T>
struct has_merge<T, decltype(std::declval<T>().merge())> : std::true_type{
};

// wrapper merge using the code defined up, if merge is defined in T it call it
template <class T>
typename std::enable_if<has_merge<T>::value>::type merge(T& obj){
    return obj.merge();
}

// no merge we do nothing
template <class T>
typename std::enable_if<!has_merge<T>::value>::type merge(T& obj){} // nothing

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
            mutex_type lock(mtx);
            inter_buffer.push(value);
        }
    }

    void merge(){
        mutex_type lock(mtx); //lock guard pattern
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
    tool::QUEUE_MUTEX_TYPE mtx;
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
        mutex_type lock(mtx); //lock guard pattern
        value_type value;
        while(inter_buffer.pop(value))
            queue.push(value);
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
    boost::lockfree::stack<double> inter_buffer;
    tool::QUEUE_MUTEX_TYPE mtx;
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

