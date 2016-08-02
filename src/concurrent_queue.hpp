//
//  concurent_queue.hpp
//  queue
//
//  Created by Tim Ewart on 02/08/2016.
//
//

#ifndef concurent_queue_h
#define concurent_queue_h


#include <thread>
#include <mutex>

namespace tool{
std::mutex mtx;

/** concurrent_queue is just a wrapper for the lock, for c++11 thing to the the move constructor
it may help for boost and std */
template<class T, /** type */
         class Q = std::priority_queue<T,std::vector<T>>, /** container type */
         class L = std::lock_guard<std::mutex>> /** lock type */
class concurrent_queue{
    typedef typename Q::value_type value_type;
    typedef typename Q::size_type size_type;
    typedef Q container_type;
    typedef L mutex_type;//not necessarely a mutex but using RAII pattern
    public:
    concurrent_queue():queue(){}

    inline void push(const value_type& value){
        mutex_type lock(mtx);
        queue.push(value);
    }

    inline value_type pop(){
        mutex_type lock(mtx);
        return queue.pop();
    }

    inline value_type& top(){
        return queue.top();
    }

    inline size_type size(){
        return queue.size();
    }

    inline bool empty(){
        return queue.empty();
    }
    
    void print(std::ostream &os) {
        queue.print(os);
    }
    
    private:
    container_type queue;
};


// carefull the << delete the queue only for debugging 
template<class T, class Q, class L>
std::ostream& operator<< (std::ostream& os, concurrent_queue<T,Q,L>& q ){
    q.print(os);
    return os;
}

} //end namespace
#endif /* concurent_queue_h */
