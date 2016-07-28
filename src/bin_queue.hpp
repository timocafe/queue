
#ifndef binqueue_hpp_
#define binqueue_hpp_

namespace tool {

/** the bin queue is a a kind of priority_queue using a ring concept, elements are sorted through
    bin from smallest to largest, into a bin there is NO specific order*. The determination
    of the bin is choosen by a kind of hash function (the hash give the bin "bucket").
    Into a bin with have a single link list using the "left" link of the node class.
     
    Objectively this queue is designed only for our problem because we are hashing "time"
    using the inverse of dt. Consequently, genericity with template is useless */

    class bin_queue {
    public:
        typedef double value_type;
        typedef std::size_t size_type;

        inline explicit bin_queue(value_type dt = 0.025, value_type t0 = 0.):nbin_(1000),qpt_(0),dt_(dt),tt_(t0){
            size_ = 0;
            bins_ = new tool::node<value_type>*[nbin_];
            for (int i=0; i < nbin_; ++i)
                bins_[i] = 0;
        }

        ~bin_queue();

        /** std::priority_queue API like */
        inline void push(value_type t){
            tool::node<value_type>* n = new tool::node<value_type>(t);
            size_++;
            enqueue(t,n); // t encapsulate in the node but also needed for the "hash function"
        }

        inline void pop(){
            tool::node<double>* q = dequeue();
            delete q;
        }

        inline value_type top(){
            return bins_[qpt_]->key();
        }

        inline size_type size(){
            return size_;
        }

        inline bool empty(){
            return !bool(size_); // is it true on Power?
        }

        /** original API */
        inline void enqueue(value_type tt, tool::node<value_type>*);

        void shift(value_type tt) {
            assert(!bins_[qpt_]);
            tt_ = tt;
            if (++qpt_ >= nbin_) {
                qpt_ = 0;
            }
        }

        tool::node<value_type>* dequeue();

        value_type tbin() { return tt_; }

        // for intenal only
        tool::node<value_type>* first();
        tool::node<value_type>* next(tool::node<value_type>*);
        void remove(tool::node<value_type>*);
        void resize(int);
    private:
        size_type size_;
        int nbin_; // number of bin
        int qpt_;
        value_type dt_; // step times
        value_type tt_; // time at beginning of qpt_ interval
        tool::node<value_type>** bins_; // container
    };
}


#include "bin_queue.ipp"
    
#endif
