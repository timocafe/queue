
#ifndef hybrid_queue_h
#define hybrid_queue_h

#include <boost/lockfree/stack.hpp>
#include "tbb/tbb.h"
#include "locker.h"
#include "trait.h"


namespace queue{

//pure mutex version, the queue can be whatever you want
template<class Q, class M = std::lock_guard<std::mutex>>
struct concurent_priority_queue{
    typedef typename Q::value_type value_type;
    typedef M mutex_type;

    concurent_priority_queue(std::size_t tid):my_id(tid){}

    void enqueue(size_t tid, value_type value){ // TO DO, more than double & and && version
        mutex_type lock(tool::mtx);
        if(iam(tid)) // I am myself push directly
            queue.push(value);
        else{ // I am not I am somebody else
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
        mutex_type lock(tool::mtx);
        bool b = (!queue.empty() && queue.top() <= t);
        if(b)
            queue.pop();
        return b;
    }

    Q queue;
    std::size_t my_id;
    std::stack<value_type> inter_buffer;
};

// hybrid version where I use a lock free boost stack
template<class Q, class M = std::lock_guard<std::mutex>>
struct concurent_partial_lock_free_priority_queue{
    typedef typename Q::value_type value_type;
    typedef M mutex_type;

    concurent_partial_lock_free_priority_queue(std::size_t tid):my_id(tid){}

    void enqueue(size_t tid, value_type value){ // TO DO, more than double & and && version
        if(iam(tid)) // I am myself push directly
            queue.push(value);
        else // I am not I am somebody else
            inter_buffer.push(value);
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

    Q queue;
    std::size_t my_id;
    boost::lockfree::stack<value_type> inter_buffer;
};

// put lock free version
template<class Q>
struct concurent_lock_free_priority_queue{
    typedef typename Q::value_type value_type;

    concurent_lock_free_priority_queue(){}

    void enqueue(value_type value){ // TO DO, more than double & and && version
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

    Q queue;
};

template<class T> // works with all priority_queue
struct benchmark_partial_lockfree{
    benchmark_partial_lockfree(std::size_t group, std::size_t event=1000):size_group(group),size_event(event),v(group,0){
        std::iota(v.begin(),v.end(),0); // give an id to every queue
        dt = 0.025;
        max_time = 50;
        distribution = std::uniform_real_distribution<double>(0.5,2);
        distribution_int = std::uniform_int_distribution<int>(0,group-1);
    }

    void benchmark(size_t tid){
        for(auto t=0.0; t < max_time; t += dt){
            double value(0);

            for(size_t i = 0; i < size_event; ++i)
                v.at(distribution_int(generator)).enqueue(tid,(t + distribution(generator))); //mixup my event + interevent

            v.at(tid).merge();

            while(v.at(tid).dequeue(value,t));

            t += dt;
        }
    }

    size_t size(){
        return v.size();
    }

    std::size_t size_group, size_event;
    double dt;
    double max_time;
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution;
    std::uniform_int_distribution<int> distribution_int;
    std::vector<concurent_priority_queue<typename T::value_type>> v;

    constexpr static auto name = "mhth_partial_lockfree";
};

//evil duplication to change with a specialization
template<container c> // works with all priority_queue
struct benchmark_lockfree{
    benchmark_lockfree(std::size_t group, std::size_t event=1000):size_group(group),size_event(event),v(group,0){
        std::iota(v.begin(),v.end(),0); // give an id to every queue
        dt = 0.025;
        max_time = 50;
        distribution = std::uniform_real_distribution<double>(0.5,2);
        distribution_int = std::uniform_int_distribution<int>(0,group-1);
    }

    void benchmark(size_t tid){
        for(auto t=0.0; t < max_time; t += dt){
            double value(0);

            for(size_t i = 0; i < size_event; ++i)
                v.at(distribution_int(generator)).enqueue(tid,(t + distribution(generator))); //mixup my event + interevent

            while(v.at(tid).dequeue(value,t));

            t += dt;
        }
    }

    size_t size(){
        return v.size();
    }

    std::size_t size_group, size_event;
    double dt;
    double max_time;
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution;
    std::uniform_int_distribution<int> distribution_int;
    std::vector<concurent_priority_queue<typename helper_type<c>::value_type>> v;

    constexpr static auto name = "mhth_lockfree";
};

}//end namespace

#endif

