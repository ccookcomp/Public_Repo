#include <assert.h>
#include <vector>
#include <iostream>
#include <typeinfo>
#include <stdexcept>
#include <iomanip>


namespace cs540{

	struct OutOfRange : public std::runtime_error {
		public:
		    OutOfRange(const std::string &mes) : runtime_error("Error: " + mes) {}
		};    
    
    template <typename T, size_t D, size_t... Dims>
    class Array {
    public:
        typedef T ValueType;
        friend class Array<T, Dims...>;
        Array<T, Dims...>* arrP;
        int size;
        int dSize;
		class FirstDimensionMajorIterator {
			public:
				Array<T, D, Dims...>* p;
				int size;
				int index;
				typename Array<T, Dims...>::FirstDimensionMajorIterator element;
				FirstDimensionMajorIterator(){
					size=D;
					index=0; 
				}
				
				FirstDimensionMajorIterator(const FirstDimensionMajorIterator &itF){
					p=itF.p;
					size=itF.size;
					index=itF.index;
					element=itF.element; 
			}
				
				FirstDimensionMajorIterator& operator=(const FirstDimensionMajorIterator &itF) {
				    p = itF.p;
				    size = itF.size;
				    index = itF.index;
				    element = itF.element;
				    return *this;
				}
				
				FirstDimensionMajorIterator &operator++() {
				    ++element;
				    if (element.index == element.size) {
				        index++;
				        if (index < size)
				            element = (p->arrP[index]).fmbegin();
				    }
				    return *this;
				}
				
				FirstDimensionMajorIterator operator++(int) {
				    FirstDimensionMajorIterator itF = *this;
				    ++(*this);
				    return itF;
				}
				
				T &operator*() const {
				    return *element;
				}
			};
		class LastDimensionMajorIterator {
			public:
				Array<T, D, Dims...>* p;
				int size;
				int index;
				typename Array<T, Dims...>::LastDimensionMajorIterator element;
				LastDimensionMajorIterator(){
					size=D;
					index=0;
				}
				
				LastDimensionMajorIterator(const LastDimensionMajorIterator &itL){
					p=itL.p;
					size=itL.size;
					index=itL.index;
					element=itL.element;
				}
				
				LastDimensionMajorIterator& operator=(const LastDimensionMajorIterator &itL) {
				    p = itL.p;
				    size = itL.size;
				    index = itL.index;
				    element = itL.element;
				    return *this;
				}
				
				LastDimensionMajorIterator &operator++() {
				    index++;
				    if (index == size) {
				        ++element;
				        index = 0;
				    }
				    setPoint();
				    return *this;
				}
				
				LastDimensionMajorIterator operator++(int) {
				    LastDimensionMajorIterator itL = *this;
				    ++(*this);
				    return itL;
				}
				
				void setPoint() {
				    element.p = &(p->arrP[index]);
				    element.setPoint();
				}
				
				T &operator*() const {
				    return *element;
				}
			};
		class ConstFirstDimensionMajorIterator {
			public:
				const Array<T, D, Dims...>* p;
				int size;
				int index;
				typename Array<T, Dims...>::FirstDimensionMajorIterator element;
				ConstFirstDimensionMajorIterator(){
					size=D;
					index=0;
				}
				
				ConstFirstDimensionMajorIterator(const ConstFirstDimensionMajorIterator &itF){
					p=itF.p;
					size=itF.size;
					index=itF.index;
					element=itF.element;
				}
				
				ConstFirstDimensionMajorIterator& operator=(const ConstFirstDimensionMajorIterator &itF) {
				    p = itF.p;
				    size = itF.size;
				    index = itF.index;
				    element = itF.element;
				    return *this;
				}
				
				ConstFirstDimensionMajorIterator &operator++() {
				    ++element;
				    if (element.index == element.size) {
				        index++;
				        if (index < size)
				            element = (p->arrP[index]).fmbegin();
				    }
				    return *this;
				}
				
				ConstFirstDimensionMajorIterator operator++(int) {
				    ConstFirstDimensionMajorIterator itF = *this;
				    ++(*this);
				    return itF;
				}
				
				const T &operator*() const {
				    return *element;
				}
			};
		class ConstLastDimensionMajorIterator {
			public:
				const Array<T, D, Dims...>* p;
				int size;
				int index;
				typename Array<T, Dims...>::ConstLastDimensionMajorIterator element;
				ConstLastDimensionMajorIterator(){
					size=D;
					index=0;
				}
				
				ConstLastDimensionMajorIterator(const ConstLastDimensionMajorIterator &itL){
					p=itL.p;
					size=itL.size;
					index=itL.index;
					element=itL.element;
				}
				
				ConstLastDimensionMajorIterator& operator=(const ConstLastDimensionMajorIterator &itL) {
				    p = itL.p;
				    size = itL.size;
				    index = itL.index;
				    element = itL.element;
				    return *this;
				}
				
				ConstLastDimensionMajorIterator &operator++() {
				    index++;
				    if (index == size) {
				        ++element;
				        index = 0;
				    }
				    setPoint();
				    return *this;
				}
				
				ConstLastDimensionMajorIterator operator++(int) {
				    ConstLastDimensionMajorIterator itL = *this;
				    ++(*this);
				    return itL;
				}
				
				void setPoint() {
				    element.p = &(p->arrP[index]);
				    element.setPoint();
				}
				
				const T &operator*() const {
				    return *element;
				}
			};
        Array(){
			size=D;
			dSize=(sizeof...(Dims));
            if (D <= 0)
                throw OutOfRange("non-positive dimension");
            arrP = new Array<T, Dims...>[D];
        }
        
        Array(const Array &arr_in){
			size=D;
			dSize=(sizeof...(Dims));
            arrP = new Array<T, Dims...>[D];
            for (size_t i=0; i<D; i++) {
                arrP[i] = arr_in.arrP[i];
            }
        }
        
        template <typename U>
        Array(const Array<U, D, Dims...> &arr_in){
			size=D;
			dSize=(sizeof...(Dims));
            arrP = new Array<T, Dims...>[D];
            for (size_t i=0; i<D; i++) {
                arrP[i] = arr_in.arrP[i];
            }
        }
        
        Array &operator=(const Array &arr_in) {
            if (arrP == arr_in.arrP) {
                return *this;
            }
            else {
                if (size==arr_in.size && dSize==arr_in.dSize) {
                    for (int i=0; i<size; i++) {
                        arrP[i] = arr_in.arrP[i];
                    }
                }
                return *this;
            }
        }
        
        template <typename U>
        Array &operator=(const Array<U, D, Dims...> &arr_in) {
            if (size==arr_in.size && dSize==arr_in.dSize) {
                for (int i=0; i<size; i++) {
                    arrP[i] = arr_in.arrP[i];
                }
            }
            return *this;
        }
        
        Array<T, Dims...>& operator[] (const int d) {
            if (size <= d) {
                throw OutOfRange("out of range");
            }
            else {
                return arrP[d];
            }
        }
        
        const Array<T, Dims...>& operator[] (const int d) const {
            if (size <= d) {
                throw OutOfRange("out of range");
            }
            else {
                return arrP[d];
            }
        }
        
        ~Array() {
            delete[] arrP;
        }
        
        FirstDimensionMajorIterator fmbegin() {
            FirstDimensionMajorIterator first;
            first.p = this;
            first.index = 0;
            first.element = arrP[0].fmbegin();
            return first;
        }
        
        FirstDimensionMajorIterator fmend() {
            FirstDimensionMajorIterator first;
            first.p = this;
            first.index = this->size;
            first.element = arrP[size-1].fmend();
            return first;
        }
        
        friend bool operator==(const FirstDimensionMajorIterator &itF1, const FirstDimensionMajorIterator &itF2) {
            if (itF1.p==itF2.p && itF1.size==itF2.size && itF1.index==itF2.index && itF1.element==itF2.element)
                return true;
            else
                return false;
        }
        
        friend bool operator!=(const FirstDimensionMajorIterator &itF1, const FirstDimensionMajorIterator &itF2) {
            if (itF1 == itF2)
                return false;
            else
                return true;
        }
        
        LastDimensionMajorIterator lmbegin() {
            LastDimensionMajorIterator l;
            l.p = this;
            l.index = 0;
            l.element = arrP[0].lmbegin();
            return l;
        }
        
        LastDimensionMajorIterator lmend() {
            LastDimensionMajorIterator l;
            l.p = this;
            l.index = 0;
            l.element = arrP[0].lmend();
            return l;
        }
        
        friend bool operator==(const LastDimensionMajorIterator &itL1, const LastDimensionMajorIterator &itL2) {
            if (itL1.p==itL2.p && itL1.size==itL2.size && itL1.index==itL2.index && itL1.element==itL2.element)
                return true;
            else
                return false;
        }
        
        friend bool operator!=(const LastDimensionMajorIterator &itL1, const LastDimensionMajorIterator &itL2) {
            if (itL1 == itL2)
                return false;
            else
                return true;
        }
        
        ConstFirstDimensionMajorIterator fmbegin() const {
            ConstFirstDimensionMajorIterator first;
            first.p = this;
            first.index = 0;
            first.element = arrP[0].fmbegin();
            return first;
        }
        
        ConstFirstDimensionMajorIterator fmend() const {
            ConstFirstDimensionMajorIterator first;
            first.p = this;
            first.index = this->size;
            first.element = arrP[size-1].fmend();
            return first;
        }
        
        friend bool operator==(const ConstFirstDimensionMajorIterator &itF1, const ConstFirstDimensionMajorIterator &itF2) {
            if (itF1.p==itF2.p && itF1.size==itF2.size && itF1.index==itF2.index && itF1.element==itF2.element)
                return true;
            else
                return false;
        }
        
        friend bool operator!=(const ConstFirstDimensionMajorIterator &itF1, const ConstFirstDimensionMajorIterator &itF2) {
            if (itF1 == itF2)
                return false;
            else
                return true;
        }
        
        ConstLastDimensionMajorIterator lmbegin() const{
            ConstLastDimensionMajorIterator l;
            l.p = this;
            l.index = 0;
            l.element = arrP[0].lmbegin();
            return l;
        }
        
        ConstLastDimensionMajorIterator lmend() const{
            ConstLastDimensionMajorIterator l;
            l.p = this;
            l.index = 0;
            l.element = arrP[0].lmend();
            return l;
        }
        
        friend bool operator==(const ConstLastDimensionMajorIterator &itL1, const ConstLastDimensionMajorIterator &itL2) {
            if (itL1.p==itL2.p && itL1.size==itL2.size && itL1.index==itL2.index && itL1.element==itL2.element)
                return true;
            else
                return false;
        }
        
        friend bool operator!=(const ConstLastDimensionMajorIterator &itL1, const ConstLastDimensionMajorIterator &itL2) {
            if (itL1 == itL2)
                return false;
            else
                return true;
        }
    };
    
    
    
    template <typename T, size_t D>
    class Array <T, D> {
    public:
        typedef T ValueType;
        T* arrP;
        int size;
        int dSize;
		class FirstDimensionMajorIterator{
			public:
				Array<T, D>* p;
				int size;
				int index;
				T* element;
				FirstDimensionMajorIterator(){
					size=D;
					index=0;
				}
				
				FirstDimensionMajorIterator(const FirstDimensionMajorIterator &itF){
					p=itF.p;
					size=itF.size;
					index=itF.index;
					element=itF.element;
				}
				
				FirstDimensionMajorIterator& operator=(const FirstDimensionMajorIterator &itF) {
				    p = itF.p;
				    size = itF.size;
				    index = itF.index;
				    element = itF.element;
				    return *this;
				}
				
				FirstDimensionMajorIterator &operator++() {
				    index++;
				    if (index == size)
				        element = &(p->arrP[0]);
				    else
				        element = &(p->arrP[index]);
				    return *this;
				}
				
				FirstDimensionMajorIterator operator++(int) {
				    FirstDimensionMajorIterator itF = *this;
				    ++(*this);
				    return itF;
				}
				
				T &operator*() const {
				    return *element;
				}
			};
		class LastDimensionMajorIterator{
			public:
				Array<T, D>* p;
				int size;
				int index;
				T* element;
				LastDimensionMajorIterator(){
					size=D;
					index=0;
				}
				
				LastDimensionMajorIterator(const LastDimensionMajorIterator &itL){
					p=itL.p;
					size=itL.size;
					index=itL.index;
					element=itL.element;
				}
				
				LastDimensionMajorIterator& operator=(const LastDimensionMajorIterator &itL) {
				    p = itL.p;
				    size = itL.size;
				    index = itL.index;
				    element = itL.element;
				    return *this;
				}
				
				LastDimensionMajorIterator &operator++() {
				    index++;
				    setPoint();
				    return *this;
				}
				
				LastDimensionMajorIterator operator++(int) {
				    LastDimensionMajorIterator itL = *this;
				    ++(*this);
				    return itL;
				}
				
				void setPoint() {
				    if (index < size)
				        element = &(p->arrP[index]);
				    else 
				        element = &(p->arrP[0]);
				}
				
				T &operator*() const {
				    return *element;
				}
			};
		class ConstFirstDimensionMajorIterator{
		public:
		    const Array<T, D>* p;
		    int size;
		    int index;
		    T* element;
		    ConstFirstDimensionMajorIterator(){
				size=D;
				index=0;
		    }
		    
		    ConstFirstDimensionMajorIterator(const ConstFirstDimensionMajorIterator &itF){
				p=itF.p;
				size=itF.size;
				index=itF.index;
				element=itF.element;
		    }
		    
		    ConstFirstDimensionMajorIterator& operator=(const ConstFirstDimensionMajorIterator &itF) {
		        p = itF.p;
		        size = itF.size;
		        index = itF.index;
		        element = itF.element;
		        return *this;
		    }
		    
		    ConstFirstDimensionMajorIterator &operator++() {
		        index++;
		        if (index == size)
		            element = &(p->arrP[0]);
		        else
		            element = &(p->arrP[index]);
		        return *this;
		    }
		    
		    ConstFirstDimensionMajorIterator operator++(int) {
		        ConstFirstDimensionMajorIterator itF = *this;
		        ++(*this);
		        return itF;
		    }
		    
		    const T &operator*() const {
		        return *element;
		    }
		};
		class ConstLastDimensionMajorIterator{
			public:
				const Array<T, D>* p;
				int size;
				int index;
				const T* element;
				ConstLastDimensionMajorIterator(){
					size=D;
					index=0;
				}
				
				ConstLastDimensionMajorIterator(const ConstLastDimensionMajorIterator &itL){
					p=itL.p;
					size=itL.size;
					index=itL.index;
					element=itL.element;
				}
				
				ConstLastDimensionMajorIterator& operator=(const ConstLastDimensionMajorIterator &itL) {
				    p = itL.p;
				    size = itL.size;
				    index = itL.index;
				    element = itL.element;
				    return *this;
				}
				
				ConstLastDimensionMajorIterator &operator++() {
				    index++;
				    setPoint();
				    return *this;
				}
				
				ConstLastDimensionMajorIterator operator++(int) {
				    ConstLastDimensionMajorIterator itL = *this;
				    ++(*this);
				    return itL;
				}
				
				void setPoint() {
				    if (index < size)
				        element = &(p->arrP[index]);
				    else 
				        element = &(p->arrP[0]);
				}
				
				const T &operator*() const {
				    return *element;
				}
		};
        Array(){
			size=D;
			dSize=0;
            if (D <= 0)
                throw OutOfRange("non-positive dimension");
            arrP = new T[D];
        }
        
        Array(const Array &arr_in){
			size=D;
			dSize=0;
            arrP = new T[D];
            for (size_t i=0; i<D; i++) {
                arrP[i] = arr_in.arrP[i];
            }
        }
        
        template <typename U>
        Array(const Array<U, D> &arr_in){
			size=D;
			dSize=0;
            arrP = new T[D];
            for (size_t i=0; i<D; i++) {
                arrP[i] = arr_in.arrP[i];
            }
        }
        
        Array &operator=(const Array &arr_in) {
            if (arrP == arr_in.arrP) {
                return *this;
            }
            else {
                if (size==arr_in.size && dSize==arr_in.dSize) {
                    for (int i=0; i<size; i++) {
                        arrP[i] = arr_in.arrP[i];
                    }
                }
                return *this;
            }
        }
        
        template <typename U>
        Array &operator=(const Array<U, D> &arr_in) {
            if (size==arr_in.size && dSize==arr_in.dSize) {
                for (int i=0; i<size; i++) {
                    arrP[i] = arr_in.arrP[i];
                }
            }
            return *this;
        }
        
        T& operator[] (const int d) {
            if (size <= d) {
                throw OutOfRange("out of range");
            }
            else {
                return arrP[d];
            }
        }
        
        const T& operator[] (const int d) const {
            if (size <= d) {
                throw OutOfRange("out of range");
            }
            else {
                return arrP[d];
            }
        }
        
        ~Array() {
            delete[] arrP;
        }
        
        FirstDimensionMajorIterator fmbegin() {
            FirstDimensionMajorIterator first;
            first.p = this;
            first.index = 0;
            first.element = &(arrP[0]);
            return first;
        }
        
        FirstDimensionMajorIterator fmend() {
            FirstDimensionMajorIterator first;
            first.p = this;
            first.index = this->size;
            first.element = &(arrP[0]);
            return first;
        }
        
        friend bool operator==(const FirstDimensionMajorIterator &itF1, const FirstDimensionMajorIterator &itF2) {
            if (itF1.p==itF2.p && itF1.size==itF2.size && itF1.index==itF2.index && itF1.element==itF2.element)
                return true;
            else
                return false;
        }
        
        friend bool operator!=(const FirstDimensionMajorIterator &itF1, const FirstDimensionMajorIterator &itF2) {
            if (itF1 == itF2)
                return false;
            else
                return true;
        }
        
        LastDimensionMajorIterator lmbegin() {
            LastDimensionMajorIterator last;
            last.p = this;
            last.index = 0;
            last.element = &(arrP[0]);
            return last;
        }
        
        LastDimensionMajorIterator lmend() {
            LastDimensionMajorIterator last;
            last.p = this;
            last.index = this->size;
            last.element = &(arrP[0]);
            return last;
        }
        
        friend bool operator==(const LastDimensionMajorIterator &itL1, const LastDimensionMajorIterator &itL2) {
            if (itL1.p==itL2.p && itL1.size==itL2.size && itL1.index==itL2.index && itL1.element==itL2.element)
                return true;
            else
                return false;
        }
        
        friend bool operator!=(const LastDimensionMajorIterator &itL1, const LastDimensionMajorIterator &itL2) {
            if (itL1 == itL2)
                return false;
            else
                return true;
        }
        
        ConstFirstDimensionMajorIterator fmbegin() const {
            ConstFirstDimensionMajorIterator first;
            first.p = this;
            first.index = 0;
            first.element = &(arrP[0]);
            return first;
        }
        
        ConstFirstDimensionMajorIterator fmend() const {
            ConstFirstDimensionMajorIterator first;
            first.p = this;
            first.index = this->size;
            first.element = &(arrP[0]);
            return first;
        }
        
        friend bool operator==(const ConstFirstDimensionMajorIterator &itF1, const ConstFirstDimensionMajorIterator &itF2) {
            if (itF1.p==itF2.p && itF1.size==itF2.size && itF1.index==itF2.index && itF1.element==itF2.element)
                return true;
            else
                return false;
        }
        
        friend bool operator!=(const ConstFirstDimensionMajorIterator &itF1, const ConstFirstDimensionMajorIterator &itF2) {
            if (itF1 == itF2)
                return false;
            else
                return true;
        }
        
        ConstLastDimensionMajorIterator lmbegin() const {
            LastDimensionMajorIterator last;
            last.p = this;
            last.index = 0;
            last.element = &(arrP[0]);
            return last;
        }
        
        ConstLastDimensionMajorIterator lmend() const {
            LastDimensionMajorIterator last;
            last.p = this;
            last.index = this->size;
            last.element = &(arrP[0]);
            return last;
        }
        
        friend bool operator==(const ConstLastDimensionMajorIterator &itL1, const ConstLastDimensionMajorIterator &itL2) {
            if (itL1.p==itL2.p && itL1.size==itL2.size && itL1.index==itL2.index && itL1.element==itL2.element)
                return true;
            else
                return false;
        }
        
        friend bool operator!=(const ConstLastDimensionMajorIterator &itL1, const ConstLastDimensionMajorIterator &itL2) {
            if (itL1 == itL2)
                return false;
            else
                return true;
        }
    };
}

