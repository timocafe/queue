

namespace tool {

    class BinQ {
    public:
        BinQ();
        ~BinQ();
        void enqueue(double tt, sptq::node<double>*);

        void shift(double tt) {
            assert(!bins_[qpt_]);
            tt_ = tt;
            if (++qpt_ >= nbin_) {
                qpt_ = 0;
            }
        }

        sptq::node<double>* top() { return bins_[qpt_]; }
        sptq::node<double>* dequeue();

        double tbin() { return tt_; }
        // for iteration
        sptq::node<double>* first();
        sptq::node<double>* next(sptq::node<double>*);
        void resize(int);
    private:
        double tt_; // time at beginning of qpt_ interval
        int nbin_, qpt_;
        sptq::node<double>** bins_;
    };
}
   #include "binqueue.ipp"
    
