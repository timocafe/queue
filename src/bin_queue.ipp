
#ifndef binqueue_ipp_
#define binqueue_ipp_

#include "queue_helper.h" //for the sptq::node object

namespace tool{

    bin_queue::~bin_queue() {
        tool::node<double>* q, *q2;
        for (q = first(); q; q = q2) {
                q2 = next(q);
                remove(q); /// Potentially dereferences freed pointer this->sptree_
        }
        // check
        for (int i=0; i < nbin_; ++i) {
            assert(!bins_[i]);
        }
        delete [] bins_;
    }

    void bin_queue::resize(int size) {
        //printf("BinQ::resize from %d to %d\n", nbin_, size);
        int i, j;
        tool::node<double>* q;
        assert(size >= nbin_);
        tool::node<double>** bins = new tool::node<double>*[size];
        for (i=nbin_; i < size; ++i) { bins[i] = 0; }
        for (i=0, j=qpt_; i < nbin_; ++i, ++j) {
            if (j >= nbin_) { j = 0; }
            bins[i] = bins_[j];
            for (q = bins[i]; q; q = q->left_) {
                q->cnt_ = i;
            }
        }
        delete [] bins_;
        bins_ = bins;
        nbin_ = size;
        qpt_ = 0;
    }

    void bin_queue::enqueue(double td, tool::node<double>* q) {

        int rev_dt = 1/dt_;
        int idt = (int)((td - tt_)*rev_dt + 1.e-10);
        assert(idt >= 0);
        if (idt >= nbin_) {
            resize(idt + 1000);
        }
        //assert (idt < nbin_);
        idt += qpt_;
        if (idt >= nbin_) { idt -= nbin_; }
        //printf("enqueue: idt=%d qpt=%d nbin_=%d\n", idt, qpt_, nbin_);
        assert (idt < nbin_);
        q->cnt_ = idt; // only for iteration
        q->left_ = bins_[idt];
        bins_[idt] = q;
    }

    tool::node<double>* bin_queue::dequeue() {
        tool::node<double>* q = NULL;
        q = bins_[qpt_];
        if (q) {
            bins_[qpt_] = q->left_;
        }
        return q;
    }

    tool::node<double>* bin_queue::first() {
        for (int i = 0; i < nbin_; ++i) {
            if (bins_[i]) {
                return bins_[i];
            }
        }
        return 0;
    }

    tool::node<double>* bin_queue::next(tool::node<double>* q) {
        if (q->left_) { return q->left_; }
        for (int i = q->cnt_ + 1; i < nbin_; ++i) {
            if (bins_[i]) {
                return bins_[i];
            }
        }
        return 0;
    }

    void bin_queue::remove(tool::node<double>* q) {
        tool::node<double>* q1, *q2;
        q1 = bins_[q->cnt_];
        if (q1 == q) {
            bins_[q->cnt_] = q->left_;
            return;
        }
        for (q2 = q1->left_; q2; q1 = q2, q2 = q2->left_) {
            if (q2 == q) {
                q1->left_ = q->left_;
                return;
            }   
        }   
    }
}

#endif