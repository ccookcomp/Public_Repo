#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#ifndef DEQUE_HPP
#define DEQUE_HPP
using namespace std;

#define Deque_DEFINE(t) 													\
	bool(*t##_comp)(const t &, const t &);									\
	struct Deque_##t##_Iterator {											\
		t *p1;																\
		t *startArr;														\
		t *endArr;															\
		void (*inc)(Deque_##t##_Iterator *);								\
		void (*dec)(Deque_##t##_Iterator *);								\
		t &(*deref)(Deque_##t##_Iterator *);								\
	};                														\
	void inc(Deque_##t##_Iterator *i){										\
		if(i->p1==i->endArr) i->p1=i->startArr-1;							\
		i->p1+=1;															\
	}																		\
	void dec(Deque_##t##_Iterator *i){										\
		if(i->p1==i->startArr) i->p1=i->endArr+1;							\
		i->p1-=1;															\
	}																		\
	t & deref(Deque_##t##_Iterator *i){										\
		return *i->p1;														\
	} 																		\
	void setStart(Deque_##t##_Iterator *i, t* s){							\
		i->startArr=s;														\
	}																		\
	void setEnd(Deque_##t##_Iterator *i, t* e){								\
		i->endArr=e;														\
	}																		\
    struct Deque_##t {                                          			\
        char type_name[sizeof("Deque_"#t)]="Deque_"#t;						\
		bool (*fp) (const t&,const t&);										\
		Deque_##t *dp;														\
		int total=0;								     					\
		t *data;   							\
		long long int start=1;												\
		long long int last=-1;                                     			\
		long long int arrSize=30000;										\
		Deque_##t##_Iterator (*begin)(Deque_##t *ap);						\
		Deque_##t##_Iterator (*end)(Deque_##t *ap);							\
		int (*size)(Deque_##t *);											\
		bool (*empty)(Deque_##t *);											\
		void (*push_back)(Deque_##t *, t);									\
		void (*push_front)(Deque_##t *, t);									\
		t &(*front)(Deque_##t *);											\
		t &(*back)(Deque_##t *);											\
		t(*pop_front)(Deque_##t *);											\
		t(*pop_back)(Deque_##t *);											\
		t &(*at)(Deque_##t *,size_t);										\
		void (*clear)(Deque_##t *ap);										\
		void (*dtor)(Deque_##t *ap);										\
		void (*sort)(Deque_##t *ap,Deque_##t##_Iterator 				i1,Deque_##t##_Iterator i2);												\
    };                                                              		\
	int size(Deque_##t *ap){												\
		return ap->total;													\
	}																		\
	bool empty(Deque_##t *ap){												\
		return ap->total==0;												\
	}																		\
	void push_back(Deque_##t *ap, t obj){									\
		if(ap->total == ap->arrSize-1){										\
			t *copy= (t*)malloc(sizeof(t) * ap->arrSize);					\
			int newSize=ap->arrSize*2;										\
			int i2=0;														\
			for (int i=ap->start; i<=ap->last; i++){						\
				i=i%(ap->arrSize);											\
				copy[i2]=ap->data[i];										\
				i2++;														\
				}															\
			ap->dtor(ap);													\
			ap->data=(t*)malloc(sizeof(t) * ap->arrSize*2);					\
			for(int i=0;i<ap->arrSize;i++){									\
				ap->data[i]=copy[i];										\
			}																\
			int newArrSize=ap->arrSize;										\
			ap->start=0;													\
			ap->last=newArrSize-2;											\
			ap->arrSize=newSize;											\
		}																	\
		if(ap->last!=ap->start-1 || ap->total==0){							\
			ap->last++;														\
			ap->last=ap->last%ap->arrSize;									\
			ap->data[ap->last]=obj;											\
			ap->total++;													\
			if(ap->total==1) ap->start =0;									\
		}										  							\
	}																		\
	void push_front(Deque_##t *ap, t obj){									\
		if(ap->total == ap->arrSize-1){										\
			t *copy=(t*)malloc(sizeof(t) * ap->arrSize);					\
			int newSize=ap->arrSize*2;										\
			int i2=0;														\
			for (int i=ap->start; i<=ap->last; i++){						\
				i=i%ap->arrSize;											\
				copy[i2]=ap->data[i];										\
				i2++;														\
			}																\
			ap->dtor(ap);													\
			ap->data=(t*)malloc(sizeof(t) * ap->arrSize*2);					\
			for(int i=0;i<ap->arrSize-1;i++){								\
				ap->data[i]=copy[i];										\
			}																\
			ap->start=0;													\
			ap->last=ap->arrSize-2;											\
			ap->arrSize=newSize;											\
		}																	\
		if(ap->start!=ap->last+1 || ap->total==0){							\
			ap->start--;													\
			if(ap->start==-1)ap->start=ap->arrSize-1;						\
			ap->data[ap->start]=obj;										\
			ap->total++;													\
			if(ap->total==1)ap->last=0;										\
		}																	\
	}																		\
	t & front(Deque_##t *ap){												\
		return ap->data[ap->start];											\
	}																		\
	t & back(Deque_##t *ap){												\
		return ap->data[ap->last];											\
	}																		\
	t pop_front(Deque_##t *ap){												\
		if(ap->start!=ap->last+1%ap->arrSize){								\
			t obj=ap->data[ap->start];										\
			ap->start=(ap->start+1)%ap->arrSize;							\
			ap->total--;													\
			return obj;														\
		}else{return ap->data[(ap->last+1)%ap->arrSize];}					\
	}																		\
	t pop_back(Deque_##t *ap){												\
		if(ap->start!=ap->last+1){											\
			t obj=ap->data[ap->last];										\
			ap->last--;														\
			if(ap->last==-1)ap->last=ap->arrSize-1;							\
			ap->total--;													\
			return obj;														\
		}else{return ap->data[(ap->last+1)%ap->arrSize];}					\
	}																		\
	t & at(Deque_##t *ap,size_t i1){				   		  				\
			return ap->data[(ap->start+i1)%(ap->arrSize)];					\
	}																		\
	void clear(Deque_##t *ap){												\
		for (int i=0;i<ap->arrSize;i++){									\
			pop_front(ap);													\
			}																\
		ap->start=1;														\
		ap->last=-1;														\
	}																		\
	void dtor(Deque_##t *ap){												\
		free(ap->data);														\
																			\
	}																		\
	Deque_##t##_Iterator begin(Deque_##t *ap){								\
		Deque_##t##_Iterator i{ &ap->data[ap->start]};						\
		setStart(&i,&(ap->data[0]));										\
		setEnd(&i,&(ap->data[ap->arrSize-1]));								\
		i.inc = &inc;														\
		i.dec = &dec;														\
		i.deref = &deref;													\
		return i;															\
	}																		\
	Deque_##t##_Iterator end(Deque_##t *ap){								\
		Deque_##t##_Iterator i {(&(ap->data[ap->last])+1)};					\
		setStart(&i,&(ap->data[0]));										\
		setEnd(&i,&(ap->data[ap->arrSize-1]));								\
		i.inc = &inc;														\
		i.dec = &dec;														\
		i.deref = &deref;													\
		return i;															\
	}																		\
	bool Deque_##t##_Iterator_equal(Deque_##t##_Iterator i1,Deque_##t##_Iterator i2){												\
		return i1.p1==i2.p1;												\
	}																		\
	bool Deque_##t##_equal(Deque_##t d1,Deque_##t d2){						\
		if(d1.total!=d2.total||d1.start!=d2.start||d1.last!=d2.last) return false;	\
		for (int i=d1.start;i<=d1.last;i++){								\
				if ((d1.dp->fp(d1.data[i],d2.data[i])||d1.dp->fp(d2.data[i],d1.data[i])))return false;												\
				}															\
		return true; 														\
		}																	\
	int Deque_##t##_compare(const void* obj1, const void* obj2){																			\
		const t *ob1=(t*)obj1; const t *ob2=(t*)obj2;\
		if(t##_comp(*ob1,*ob2)) return -1;									\
		else if(t##_comp(*ob2,*ob1)) return 1;								\
		else return 0;														\
	}																		\
	void sort(Deque_##t *ap,Deque_##t##_Iterator i1,Deque_##t##_Iterator i2){\
		t* temp=(t*)malloc(sizeof(t) * ap->arrSize);																	\
		size_t s=0;															\
		for(auto it=ap->begin(ap);!Deque_##t##_Iterator_equal(it,i2);it.inc(&it)){	\
			temp[s]=*it.p1;													\
			s++;															\
		}																	\
		qsort(temp, s, sizeof(t),Deque_##t##_compare);						\
		s=0;																\
		for(auto it=ap->begin(ap);!Deque_##t##_Iterator_equal(it,i2);it.inc(&it)){																		\
			*it.p1=temp[s];													\
			s++;															\
		}																	\
		free(temp);															\
	}																		\
void Deque_##t##_ctor(Deque_##t *ap,bool (*fp1) (const t&,const t&)){		\
			ap->data=(t*)malloc(sizeof(t) * ap->arrSize);																	\
			ap->dp=ap;														\
			ap->fp=fp1;														\
			ap->begin=&begin;												\
			ap->end=&end;													\
			ap->size=&size;													\
			ap->empty=&empty;												\
			ap->push_back=&push_back;										\
			ap->push_front=&push_front;										\
			ap->front=&front;												\
			ap->back=&back;													\
			ap->pop_front=&pop_front;										\
			ap->pop_back=&pop_back;											\
			ap->at=&at;														\
			ap->clear=&clear;												\
			ap->dtor=&dtor;													\
			ap->sort=&sort;													\
			t##_comp=fp1;																\
		 }                            										

#endif
