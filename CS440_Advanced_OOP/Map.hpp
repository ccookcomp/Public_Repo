#include <string.h>
#include <iostream>
#include<utility>
#include<cmath>
#include<ctime>
#include<cstdlib>
#include<stdexcept>

#define LEVEL 100

//Skiplist Implementation from http://www.drdobbs.com/cpp/skip-lists-in-c/184403579

namespace cs540{
	template <typename Key_T, typename Mapped_T> class Map;
		template <typename Key_T, typename Mapped_T>class Node{
			public:
				friend class Map<Key_T,Mapped_T>;
				typedef std::pair<const Key_T,Mapped_T> ValueType;
				Node(std::size_t level){
					int level1=level+1;
					fwd=new Node*[level1];
					size_t size1=sizeof(Node*)*(level1);
					memset(fwd,'\0',size1);
					val=NULL;
					prev=NULL;
				}

				Node(std::size_t level, const ValueType &v){
					int level1=level+1;
					fwd=new Node*[level1];
					size_t size1=sizeof(Node*)*(level1);
					memset(fwd,'\0',size1);
					val=new std::pair<const Key_T,Mapped_T>(v);
					prev=NULL;
				}
				~Node(){
					delete[] fwd;
					delete val;
				}
			private:
				Node **fwd;
				Node *prev;
				ValueType *val;


		};
		template <typename Key_T, typename Mapped_T> class Map{
			public:
				typedef std::pair<const Key_T,Mapped_T> ValueType;
				class Iterator{
					public:
						Iterator(const Iterator& it):currentNode(it.getCurrentNode()){}
						Iterator(Node<Key_T,Mapped_T>* node) : currentNode(node){}
						Node<Key_T,Mapped_T>* getCurrentNode()const{
							return currentNode;
						}
						Iterator & operator=(const Iterator &it){
							currentNode=it.getCurrentNode();
							return *this;
						}
						Iterator & operator++(){
							if(currentNode==NULL) return *this;
							else{
								currentNode=currentNode->fwd[0];
								return *this;
							}
						}
						Iterator operator++(int){
							Map<Key_T,Mapped_T>::Iterator newt=*this;
							if(currentNode==NULL) return newt;
							else{
								currentNode=currentNode->fwd[0];
								return *this;
							}
						}
						Iterator & operator--(){
							if(currentNode==NULL) return *this;
							else{
								currentNode=currentNode->prev;
								return *this;
							}
						}
						Iterator operator--(int){
							Map<Key_T,Mapped_T>::Iterator newt=*this;
							if(currentNode==NULL) return newt;
							else{
								currentNode=currentNode->prev;
								return *this;
							}
						}
						ValueType& operator*() const{
							return *currentNode->val;
						}
						ValueType* operator->() const{
							return currentNode->val;
						}
						~Iterator(){
							currentNode=NULL;
						}
					private:
						Node<Key_T,Mapped_T>* currentNode;
				};
				class ConstIterator{
					public:
						ConstIterator(const ConstIterator &cit) : constCurrentNode(cit.getConstCurrentNode()){}
						ConstIterator(const Iterator& it) : constCurrentNode(it.getCurrentNode()){}
						ConstIterator(Node<Key_T,Mapped_T>* node) : constCurrentNode(node){}
						Node<Key_T,Mapped_T>* getConstCurrentNode() const{
							return constCurrentNode;
						}
						ConstIterator & operator=(const ConstIterator &cit){
							constCurrentNode=cit.getConstCurrentNode();
							return *this;
						}
						ConstIterator & operator++(){
							if(constCurrentNode==NULL) return *this;
							else{
								constCurrentNode=constCurrentNode->fwd[0];
								return *this;
							}
						}
						ConstIterator operator++(int){
							Map<Key_T,Mapped_T>::ConstIterator newt=*this;
							if(constCurrentNode==NULL) return newt;
							else{
								constCurrentNode=constCurrentNode->fwd[0];
								return *this;
							}
						}
						ConstIterator & operator--(){
							if(constCurrentNode==NULL) return *this;
							else{
								constCurrentNode=constCurrentNode->prev;
								return *this;
							}
						}
						ConstIterator operator--(int){
							Map<Key_T,Mapped_T>::ConstIterator newt=*this;
							if(constCurrentNode==NULL) return newt;
							else{
								constCurrentNode=constCurrentNode->prev;
								return *this;
							}
						}
						const ValueType& operator*() const{
							return *constCurrentNode->val;
						}
						const ValueType& operator->() const{
							return *constCurrentNode->val;
						}
						~ConstIterator(){
							constCurrentNode=NULL;
						}
					private:
						Node<Key_T,Mapped_T>* constCurrentNode;
				};

				class ReverseIterator{
					public:
						ReverseIterator(const ReverseIterator &rit) : revCurrentNode(rit.getRevCurrentNode()){}
						ReverseIterator(Node<Key_T,Mapped_T>* node) : revCurrentNode(node){}
						Node<Key_T,Mapped_T>* getRevCurrentNode() const{
							return revCurrentNode;
						}
						ReverseIterator & operator=(const ReverseIterator &rit){
							revCurrentNode=rit.getRevCurrentNode();
							return *this;
						}
						ReverseIterator & operator++(){
							if(revCurrentNode==NULL) return *this;
							else{
								revCurrentNode=revCurrentNode->prev;
								return *this;
							}
						}
						ReverseIterator operator++(int){
							Map<Key_T,Mapped_T>::ReverseIterator newt=*this;
							if(revCurrentNode==NULL) return newt;
							else{
								revCurrentNode=revCurrentNode->prev;
								return *this;
							}
						}
						ReverseIterator & operator--(){
							if(revCurrentNode==NULL) return *this;
							else{
								revCurrentNode=revCurrentNode->fwd[0];
								return *this;
							}
						}
						ReverseIterator operator--(int){
							Map<Key_T,Mapped_T>::ReverseIterator newt=*this;
							if(revCurrentNode==NULL) return newt;
							else{
								revCurrentNode=revCurrentNode->fwd[0];
								return *this;
							}
						}
						ValueType& operator*() const{
							return *revCurrentNode->val;
						}
						ValueType* operator->() const{
							return revCurrentNode->val;
						}
						~ReverseIterator(){
							revCurrentNode=NULL;
						}
					private:
						Node<Key_T,Mapped_T>* revCurrentNode;
				};
				Map(){
					Map_ctor();
				}
				void Map_ctor(){
					head= new Node<Key_T,Mapped_T>(LEVEL);
					tail= new Node<Key_T,Mapped_T>(LEVEL);
					head->fwd[0]=tail;
					tail->prev=head;
					head->prev=NULL;
					tail->fwd[0]=NULL;
					s=0;
					max=0;
				}
				Map(const Map& map){
					Map_ctor();
					Node<Key_T,Mapped_T> *temp= map.getHead()->fwd[0];
					while(temp!=map.getTail()){
						ValueType& val=*(temp->val);
						insert(val);
						temp=temp->fwd[0];
					}
				}
				Map(std::initializer_list<std::pair<const Key_T, Mapped_T>> list){
					Map_ctor();
					auto it=list.begin();
					while(it!=list.end()){
						insert(*it);
						it++;
					}
				}
				~Map(){
					Node<Key_T,Mapped_T> *tempH=head;
					Node<Key_T,Mapped_T> *temp;
					while(tempH!=NULL){
						temp=tempH->fwd[0];
						delete tempH;
						tempH=temp;
					}
				}
				Node<Key_T,Mapped_T>* getHead() const{
					return head;
				}
				Node<Key_T,Mapped_T>* getTail() const{
					return tail;
				}
				std::size_t size() const{
					return s;
				}
				bool empty(){
					return s==0;
				}
				Iterator find(const Key_T& key){
					Node<Key_T,Mapped_T> *temp=findLast(key);
					if(temp==NULL) return Map<Key_T,Mapped_T>::Iterator(tail);
					return Map<Key_T,Mapped_T>::Iterator(temp);
				}
				ConstIterator find(const Key_T& key) const{
					Node<Key_T,Mapped_T> *temp=findLast(key);
					if(temp==NULL) return Map<Key_T,Mapped_T>::ConstIterator(tail);
					return Map<Key_T,Mapped_T>::ConstIterator(temp);
				}
				Iterator begin(){
					return (Map<Key_T,Mapped_T>::Iterator(head->fwd[0]));
				}
				Iterator end(){
					return Map<Key_T,Mapped_T>::Iterator(tail);
				}
				ConstIterator begin() const{
					return Map<Key_T,Mapped_T>::ConstIterator(head->fwd[0]);
				}
				ConstIterator end() const{
					return Map<Key_T,Mapped_T>::ConstIterator(tail);
				}
				ReverseIterator rbegin(){
					return Map<Key_T,Mapped_T>::ReverseIterator(tail->prev);
				}
				ReverseIterator rend(){
					return Map<Key_T,Mapped_T>::ReverseIterator(head);
				}
				Mapped_T &at(const Key_T& key){
					Node<Key_T,Mapped_T>*temp=findLast(key);
					if(temp==NULL) throw std::out_of_range("Not in Map");
					else return temp->val->second;
				}
				const Mapped_T &at(const Key_T& key) const{
					Node<Key_T,Mapped_T>*temp=findLast(key);
					if(temp==NULL) throw std::out_of_range("Not in Map");
					else return temp->val->second;
				}
				friend bool operator==(const Iterator& it1,const Iterator& it2){
					return(it1.getCurrentNode() == it2.getCurrentNode());
				}
				friend bool operator!=(const Iterator& it1,const Iterator& it2){
					return(it1.getCurrentNode() != it2.getCurrentNode());
				}
				friend bool operator==(const ConstIterator& cit1,const ConstIterator& cit2){
					return(cit1.getConstCurrentNode() == cit2.getConstCurrentNode());
				}
				friend bool operator!=(const ConstIterator& cit1,const ConstIterator& cit2){
					return(cit1.getConstCurrentNode() != cit2.getConstCurrentNode());
				}
				friend bool operator==(const ReverseIterator& rit1,const ReverseIterator& rit2){
					return(rit1.getRevCurrentNode() == rit2.getRevCurrentNode());
				}
				friend bool operator!=(const ReverseIterator& rit1,const ReverseIterator& rit2){
					return(rit1.getRevCurrentNode() != rit2.getRevCurrentNode());
				}
				friend bool operator==(const Iterator& it1,const ConstIterator& cit2){
					return(it1.getCurrentNode() == cit2.getConstCurrentNode());
				}
				friend bool operator==(const ConstIterator& cit1,const Iterator& it2){
					return(cit1.getConstCurrentNode() == it2.getCurrentNode());
				}
				friend bool operator!=(const Iterator& it1,const ConstIterator& cit2){
					return(it1.getCurrentNode() != cit2.getConstCurrentNode());
				}
				friend bool operator!=(const ConstIterator& cit1,const Iterator& it2){
					return(cit1.getConstCurrentNode() != it2.getCurrentNode());
				}
				template<typename Key,typename Mapped> friend bool operator==(const Map<Key,Mapped>&, const Map<Key,Mapped>&);
				template<typename Key,typename Mapped> friend bool operator<(const Map<Key,Mapped>&, const Map<Key,Mapped>&);
				template<typename Key,typename Mapped> friend bool operator!=(const Map<Key,Mapped>&, const Map<Key,Mapped>&);
				void clear();
				Mapped_T& operator[](const Key_T&);
				size_t randomLevel(Node<Key_T,Mapped_T>**);
				Node<Key_T,Mapped_T>* findLast(const Key_T& key) const;
				std::pair<Iterator,bool> insert(const ValueType&);
				template <typename it_T> void insert(it_T begin, it_T end);
				void erase(Iterator);
				void erase(const Key_T&);
				Map& operator=(const Map&);
				bool operator!=(const Map&) const;
				private:
					Node<Key_T,Mapped_T>* head;
					Node<Key_T,Mapped_T>* tail;
					std::size_t max;
					std::size_t s;
		};
		template <typename Key_T, typename Mapped_T> Mapped_T& Map<Key_T,Mapped_T>::operator[](const Key_T& key){
			Node<Key_T,Mapped_T>* temp=findLast(key);
			if(temp==NULL){
				Map<Key_T,Mapped_T>::Iterator it=insert(std::make_pair(key,Mapped_T())).first;
				return it.getCurrentNode()->val->second;
			}
			return temp->val->second;
		}

		template<typename Key_T,typename Mapped_T> size_t Map<Key_T,Mapped_T>::randomLevel(Node<Key_T,Mapped_T>** nodes){
			size_t level=0;
			while(level<LEVEL && rand()< (RAND_MAX*.5)){
				level++;
			}
			if(level>max){
				size_t temp=max+1;
				while(temp<=level){
					nodes[temp]=head;
					temp++;
				}
				max=level;
			}
			return level;
		}

		template <typename Key_T,typename Mapped_T>	Map<Key_T,Mapped_T>&Map<Key_T,Mapped_T>::operator=(const Map<Key_T,Mapped_T>& map){
			if(this==&map) return *this;

			Node<Key_T,Mapped_T> *tempH=head;
			Node<Key_T,Mapped_T> *temp;
			while(tempH!=NULL){
				temp=tempH->fwd[0];
				delete tempH;
				tempH=NULL;
				tempH=temp;
			}
			Map_ctor();
			Node<Key_T,Mapped_T>*first=map.getHead()->fwd[0];
			if(first == map.getTail()) return *this;
			while(first != map.getTail()){
				insert(*(first->val));
				first=first->fwd[0];
			}
			return *this;
		}
		template <typename Key_T,typename Mapped_T> std::pair<typename Map<Key_T,Mapped_T>::Iterator, bool> Map<Key_T,Mapped_T>::insert(const std::pair<const Key_T,Mapped_T> & val){
			const Key_T& key=val.first;
			Node<Key_T,Mapped_T> *temp= head;
			Node<Key_T,Mapped_T> **newFwd= new Node<Key_T,Mapped_T>*[LEVEL+1];
			memset(newFwd, '\0',sizeof(Node<Key_T,Mapped_T>*)*(LEVEL+1));
			size_t i=max;
			//std::cout<<i<<std:endl;
			while(i>0){
				while(temp->fwd[i]!=NULL && temp->fwd[i]->val->first < key){
					temp=temp->fwd[i];
				}
				newFwd[i]=temp;
				i--;
			}
			while(temp->fwd[0]!=tail && temp->fwd[0]->val->first < key){
				temp=temp->fwd[0];
			}
			newFwd[0]=temp;
			Node<Key_T,Mapped_T> *firstFwd=newFwd[0];
			firstFwd=firstFwd->fwd[0];
			if(firstFwd->val != NULL && firstFwd->val->first == val.first){
				delete[] newFwd;
				return std::make_pair(Map<Key_T,Mapped_T>::Iterator(firstFwd),false);
			}
			size_t level=randomLevel(newFwd);
			firstFwd=NULL;
			firstFwd= new Node<Key_T,Mapped_T>(level,val);
			i=0;
			while(i<=level){
				firstFwd->fwd[i]=newFwd[i]->fwd[i];
				newFwd[i]->fwd[i] = firstFwd;
				i++;
			}
			firstFwd->prev=newFwd[0];
			if(firstFwd->fwd[0]!=tail){
				firstFwd->fwd[0]->prev=firstFwd;
			}
			else tail->prev=firstFwd;
			s++;
			delete[] newFwd;
			return std::make_pair(Map<Key_T,Mapped_T>::Iterator(firstFwd),true);
		}
		template <typename Key_T,typename Mapped_T> template <typename it_T> void Map<Key_T,Mapped_T>::insert(it_T begin, it_T end){
			auto it = begin;
			while(it!=end){
				insert(*it);
				it++;
			}
		}
		template <typename Key_T,typename Mapped_T> void Map<Key_T,Mapped_T>::erase(const Key_T& key){
			Node<Key_T,Mapped_T> ** newFwd=new Node<Key_T,Mapped_T>* [LEVEL+1];
			Node<Key_T,Mapped_T>* temp=head;
			memset(newFwd, '\0',sizeof(Node<Key_T,Mapped_T>*)*(LEVEL+1));
			size_t i=max;
			//std::cout<<i<<std:endl;
			while(i>0){
				while(temp->fwd[i]!=NULL && temp->fwd[i]->val->first<key){
					temp=temp->fwd[i];
				}
				newFwd[i]=temp;
				i--;
			}
			//std::cout<<i<<std:endl;
			while(temp->fwd[0]!=tail && temp->fwd[0]->val->first < key){
				temp=temp->fwd[0];
			}
			newFwd[0]=temp;
			Node<Key_T,Mapped_T> *firstFwd=newFwd[0];
			firstFwd=firstFwd->fwd[0];
			if(firstFwd->val->first == key){
				i=0;
				while(i<=max && newFwd[i]->fwd[i]==firstFwd){
					newFwd[i]->fwd[i]=firstFwd->fwd[i];
					i++;
				}
				if(firstFwd->fwd[0]!=tail){
					firstFwd->fwd[0]->prev=firstFwd->prev;
				}
				else{
					tail->prev=firstFwd->prev;
					firstFwd->prev->fwd[0]=tail;
				}
				delete firstFwd;
				while(max>0 && head->fwd[max] ==NULL){
					max--;
				}
				s--;
				delete[] newFwd;
			}else{
				throw std::out_of_range("Not in Map");
			}
		}
		template <typename Key_T,typename Mapped_T> void Map<Key_T,Mapped_T>::clear(){
			Node<Key_T,Mapped_T>* tempH=head;
			Node<Key_T,Mapped_T>* temp;
			while(temp!=NULL){
				temp=tempH->fwd[0];
				delete tempH;
				tempH=temp;
			}
			Map_ctor();
		}
		template <typename Key_T,typename Mapped_T> void Map<Key_T,Mapped_T>::erase(Map<Key_T,Mapped_T>::Iterator it){
			const Key_T& key=it.getCurrentNode()->val->first;
			Node<Key_T,Mapped_T>** newFwd=new Node<Key_T,Mapped_T>* [LEVEL+1];
			Node<Key_T,Mapped_T>* temp=head;
			memset(newFwd, '\0',sizeof(Node<Key_T,Mapped_T>*)*(LEVEL+1));
			size_t i=max;
			while(i>0){
				while(temp->fwd[i]!=NULL && temp->fwd[i]->val->first < key){
					temp=temp->fwd[i];
				}
				newFwd[i]=temp;
				i--;
			}
			while(temp->fwd[0]!=tail && temp->fwd[0]->val->first < key){
				temp=temp->fwd[0];
			}
			newFwd[0]=temp;
			Node<Key_T,Mapped_T> *firstFwd=newFwd[0];
			firstFwd=firstFwd->fwd[0];
			if(firstFwd->val->first == key){
				i=0;
				while(i<=max && newFwd[i]->fwd[i]==firstFwd){
					newFwd[i]->fwd[i]=firstFwd->fwd[i];
					i++;
				}
				if(firstFwd->fwd[0]!=tail){
					firstFwd->fwd[0]->prev=firstFwd->prev;
				}
				else{
					tail->prev=firstFwd->prev;
					firstFwd->prev->fwd[0]=tail;
				}
				delete firstFwd;
				while(max>0 && head->fwd[max] ==NULL){
					max--;
				}
				s--;
				delete[] newFwd;
			}else{
				throw std::out_of_range("Not in Map");
			}
		}
		template <typename Key_T,typename Mapped_T> bool Map<Key_T,Mapped_T>::operator!=(const Map<Key_T,Mapped_T>& map) const{
			if(*this == map) return false;
			else return true;
		}
		template <typename Key_T,typename Mapped_T> bool operator==(const Map<Key_T,Mapped_T>& m1,const Map<Key_T,Mapped_T>& m2){
			if((m1.size())!=(m2.size())) return false;
			auto it1 = m1.begin();
			auto it2 = m1.begin();
			while(it1 != m1.end() && it2!=m2.end()){
				if(*it1 != *it2) return false;
				it1++;
				it2++;
			}
			return true;
		}
		template <typename Key_T,typename Mapped_T> bool operator!=(const Map<Key_T,Mapped_T>& m1,const Map<Key_T,Mapped_T>& m2){
			return !(m1==m2);
		}
		template <typename Key_T,typename Mapped_T> Node<Key_T,Mapped_T>* Map<Key_T,Mapped_T>::findLast(const Key_T& key) const{
			Node<Key_T,Mapped_T>* temp=head;
			size_t i=max;
			//std::cout<<i<<std:endl;
			while(i>0){
				while(temp->fwd[i] != NULL && temp->fwd[i]->val->first < key){
					temp=temp->fwd[i];
				}
				i--;
			}
			while(temp->fwd[0] !=tail && temp->fwd[0]->val->first < key){
				temp=temp->fwd[0];
			}
			temp=temp->fwd[0];
			if(temp==tail) return NULL;
			if(temp!=NULL){
				if(temp->val->first == key) return temp;
			}
			return NULL;
		}
		template <typename Key_T,typename Mapped_T> bool operator<(const Map<Key_T,Mapped_T>& m1,const Map<Key_T,Mapped_T>& m2){
			size_t s1=m1.size();
			size_t s2=m2.size();
			if(s1<s2)return true;
			if(s1>s2)return false;
			auto it1=m1.begin();
			auto it2=m2.begin();
			while(it1 !=m1.end() && it2 != m2.end()){
				bool b1=(*it1).first<(*it2).first;
				bool b2=(*it1).first>(*it2).first;
				if(b1) return true;
				if(b2) return false;
				it1++;
				it2++;
			}
			return false;
		}
	
}		
