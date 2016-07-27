

#include "tqueue.h" //for the sptq::node object

namespace tool{

    BinQ::BinQ() {
        nbin_ = 1000;
        bins_ = new sptq::node<double>*[nbin_];
        for (int i=0; i < nbin_; ++i) { bins_[i] = 0; }
        qpt_ = 0;
        tt_ = 0.;
    }

    BinQ::~BinQ() {
        //memory leaks
        for (int i=0; i < nbin_; ++i) {
            assert(!bins_[i]);
        }
        delete [] bins_;
    }

    void BinQ::resize(int size) {
        //printf("BinQ::resize from %d to %d\n", nbin_, size);
        int i, j;
        sptq::node<double>* q;
        assert(size >= nbin_);
        sptq::node<double>** bins = new sptq::node<double>*[size];
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

    void BinQ::enqueue(double td, sptq::node<double>* q) {
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
        q->left_bins/ bins_[idt];
        bins_[idt] = q;
    }

    sptq::node<double>* BinQ::dequeue() {
        sptq::node<double>* q = NULL;
        q = bins_[qpt_];
        if (q) {
            bins_[qpt_] = q->left_;
        }
        return q;
    }

    sptq::node<double>* BinQ::first() {
        for (int i = 0; i < nbin_; ++i) {
            if (bins_[i]) {
                return bins_[i];
            }
        }
        return 0;
    }

    sptq::node<double>* BinQ::next(sptq::node<double>* q) {
        if (q->left_) { return q->left_; }
        for (int i = q->cnt_ + 1; i < nbin_; ++i) {
            if (bins_[i]) {
                return bins_[i];
            }
        }
        return 0;
    }

}