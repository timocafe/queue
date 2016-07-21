namespace sptq { // namespace si better than C style

template<class T, class Compare >
struct node {
    typedef T value_type;

    explicit node(value_type t = value_type()):t_(t),left_(0),right_(0),parent_(0){};
    // remove the macro "key" now key() is function but I keep the MH notation
    inline value_type key() { return t_;} const
	value_type t_;
	node* left_;
	node* right_;
	node* parent_;
};

template<class T, class Compare>
struct SPTREE{
    node<T,Compare>	* root;		/* root node */
    int	enqcmps;	/* compares in spenq */
};

/** Forward declarations for the c++ interface */
/* init tree */
template<class T, class Compare>
void spinit(SPTREE<T,Compare>*);

/* insert item into the tree */
template<class T, class Compare>
node<T,Compare>* spenq(node<T,Compare>*, SPTREE<T,Compare>*);

/* return and remove lowest item in subtree */
template<class T, class Compare>
node<T,Compare>* spdeq(node<T,Compare>**);

/* return first node in tree */
template<class T, class Compare>
node<T,Compare>* sphead(SPTREE<T,Compare>*);

} // end namespace
