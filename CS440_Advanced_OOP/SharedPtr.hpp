#include <string.h>
#include <iostream>
#include <utility>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <stdexcept>
#include <pthread.h>


namespace cs540{
	template <typename T> class SharedPtr;
	struct pdtor{
		virtual void dest() =0;
		virtual ~pdtor() = default;
	};
	template <typename U> struct dtor:public pdtor{
				U* des;
				dtor(){
					des=nullptr;
				}
				dtor(U*ptr){
					des=ptr;
				}
				void dest() override{
					delete des;
				}
			};
	struct PtrData{
		public:
			int count;
			pdtor *dp;
			void* ptr;
			pthread_mutex_t mutex;
			PtrData(){
				count=0;
				dp=nullptr;
				ptr=nullptr;
				mutex=PTHREAD_MUTEX_INITIALIZER;
			}

			~PtrData(){
				if(dp){
					dp->dest();
					delete dp;
				}
				pthread_mutex_unlock(&mutex);
			}
			
	};
	template <typename T> class SharedPtr{
		public:
			PtrData *data;
			SharedPtr(){
				data=nullptr;
			}
			template <typename U> explicit SharedPtr(U * p1){

				data=new PtrData();
				data->ptr=p1;
				data->count=1;
				data->dp=new dtor<U>(p1);

			}
			template <typename U> SharedPtr(const SharedPtr<U> &p,T* obj){
		
				data=p.data;
				data->ptr=obj;
				inc();

			}
			SharedPtr(const SharedPtr &p){

				data=p.data;
				inc();

			}
			template <typename U> SharedPtr(const SharedPtr<U> &p){

				data=p.data;
				inc();

			}
			SharedPtr(SharedPtr &&p){

				data = std::move(p.data);
				p.data = nullptr;

			}
			template <typename U> SharedPtr(SharedPtr<U> &&p){

				data = std::move(p.data);
				p.data = nullptr;

			}
			SharedPtr &operator=(const SharedPtr &p){

				dec();
				data=p.data;
				inc();
				return *this;
			}
			template <typename U> SharedPtr<T> &operator=(const SharedPtr<U> &p){

				dec();
				data=p.data;
				inc();
				return *this;
		}
			SharedPtr &operator=(SharedPtr &&p){
				dec();
				data=std::move(p.data);
				p.data=nullptr;
				return *this;
			}
			template <typename U> SharedPtr &operator=(SharedPtr<U> &&p){
				dec();
				data=std::move(p.data);
				p.data=nullptr;
				return *this;
			}
			T * get() const{
				if(data){
					return static_cast<T*>(data->ptr);
				}return nullptr;
			}
			~SharedPtr(){
				dec();
			}
			void reset(){
				dec();
				data=nullptr;
				
			}
			template <typename U> void reset(U *p){
				dec();
				data=new PtrData();
				data->ptr=p;
				data->count=1;
				data->dp=new dtor<U>(p);
			}
			T &operator*() const{return *static_cast<T*>(data->ptr);}
			T *operator->() const{return static_cast<T*>(data->ptr);}
			explicit operator bool() const{
				if(data){
					return data->ptr!=nullptr;
				}
				return false;
			}
		private:
			void inc(){
				if(data!=nullptr){
					pthread_mutex_lock(&data->mutex);
					data->count++;
					pthread_mutex_unlock(&data->mutex);
				}
			}
			void dec(){
				if(data!=nullptr){
					pthread_mutex_lock(&data->mutex);
					data->count--;
					if(data->count==0){
						delete data;
						data=nullptr;
						return;
					}
					pthread_mutex_unlock(&data->mutex);
				}
			}
	};		
	template <typename T1, typename T2> bool operator==(const SharedPtr<T1> &p1, const SharedPtr<T2> &p2){
			return p1.get()==p2.get();
		}
	template <typename T> bool operator==(const SharedPtr<T> &p1, std::nullptr_t np){
			return p1.get()==np;
		}
	template <typename T> bool operator==(std::nullptr_t np, const SharedPtr<T> &p1){
			return np==p1.get();
		}
	template <typename T1, typename T2> bool operator!=(const SharedPtr<T1>&p1, const SharedPtr<T2> &p2){
			return !(p1.get()==p2.get());
		}
	template <typename T> bool operator!=(const SharedPtr<T> &p1, std::nullptr_t np){
			return p1.get() != np;
		}
	template <typename T> bool operator!=(std::nullptr_t np, const SharedPtr<T> &p1){
			return np!=p1.get();
		}
	template <typename T, typename U> SharedPtr<T> static_pointer_cast(const SharedPtr<U> &sp) noexcept{

			return SharedPtr<T>(sp,static_cast<T*>(sp.get()));
		}
	template <typename T, typename U> SharedPtr<T> dynamic_pointer_cast(const SharedPtr<U> &sp){

			auto cp = dynamic_cast<T*>(sp.get());
			if(cp) return SharedPtr<T>(sp,cp);
			return SharedPtr<T>();
			
		}
	
}
