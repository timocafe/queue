/*
Copyright (c) 2016, Blue Brain Project
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef tqueue_hpp_
#define tqueue_hpp_

#include <stdio.h>
#include <assert.h>
#include <cstring>
#include <ostream>
#include <functional>

#include "tqueue.h"

namespace sptq {
/** The queue: TQeue from Michael starts here, not compliant with std for the container,
but ok for the type support and the comparator, by default std::less and not std::greated
as it was in the original version */

template<class T = double, class Compare = std::less<T>>
class TQueue {
public:
    typedef sptq::SPTREE<T> container;
    typedef std::size_t size_type;
    typedef T value_type;

    inline TQueue():s(0) {
        sptq::spinit(&q);
    }

    inline ~TQueue(){
        sptq::node<T> *n;
        while((n = sptq::spdeq(&(&q)->root)) != NULL)
          delete n;
    }

    inline void push(value_type value){
        sptq::node<T> *n = new sptq::node<T>(value);
        sptq::spenq<T,Compare>(n, &q); // the Comparator is use only here
        s++;
    }

    inline void pop(){
        if(!empty()){
            sptq::node<T> *n = sptq::spdeq(&(&q)->root);
            delete n; // pop remove definitively the element else memory leak
            s--;
        }
    }

    inline value_type top(){
        value_type tmp = value_type();
        if(!empty())
            tmp = sptq::sphead<T>(&q)->key();
        return tmp;
    }

    inline size_type size(){
        return s;
    }

    inline bool empty(){
        return !bool(s); // is it true on Power?
    }

    void print(std::ostream &os) {
        while(!empty()){
            os << top() << std::endl;
            pop();
        }
    }

private:
    size_type s;
    container q;
};

// carefull the << delete the queue only for debugging 
template<class T, class Compare>
std::ostream& operator<< (std::ostream& os, TQueue<T,Compare>& q ){
    q.print(os);
    return os;
}

} // end namespace
#include "tqueue.ipp"

#endif
