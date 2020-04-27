#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <cstddef>
#include <string>
#include <stdexcept>
//#include <exception>

namespace cs540{
	
	class BadFunctionCall : std::runtime_error {
        public:
            explicit BadFunctionCall(const std::string& s) : std::runtime_error(s){ }
            explicit BadFunctionCall(const char* s) : std::runtime_error(s){ }
    };
	template <typename ResultType, typename ... ArgumentTypes> class Function1{
		public:
		virtual ResultType operator()(ArgumentTypes... args) = 0;
		virtual Function1<ResultType,ArgumentTypes...>* clone() = 0;
		virtual ~Function1() = default;
	};

	template <typename T> class Function;

	template <typename ResultType, typename ... ArgumentTypes> class Function<ResultType(ArgumentTypes...)>{
		template<typename T> struct Function2 : public Function1<ResultType, ArgumentTypes...>{
			T obj;
			Function2(const T& o) : obj(o){};
			virtual ~Function2() = default;
			virtual ResultType operator()(ArgumentTypes... args){
				return obj(args...);
			}
			virtual Function1<ResultType, ArgumentTypes...>* clone(){
				return new Function2<T>(obj);
			}
		};
	public:
		Function() : fp(nullptr), f(nullptr){}
		template <typename T> Function(T t){
			fp=nullptr;
			f=new Function2<T>(t);
		}
	
		Function(const Function &f1){
			fp=f1.fp;
			if(f1.f){
				f=f1.f->clone();
			}else{
				f=nullptr;
			}
		}
		Function &operator=(const Function &f1){
			if(f) delete f;
			fp=f1.fp;
			if(f1.f)f=f1.f->clone();
			else f=nullptr;
			return *this;
		}
		~Function(){
			if(f){
				delete f;
				f=nullptr;
			}
		}
		ResultType operator()(ArgumentTypes... args){
			if(!f&&!fp){
				throw BadFunctionCall("Bad call");
			}
			if(fp){
				return fp(args...);
			}else{
				return (*f)(args...);
			}
		}
		explicit operator bool() const{
			return ((fp!=nullptr) || (f!=nullptr));
		}
		private:
			ResultType (*fp)(ArgumentTypes...);
			Function1<ResultType,ArgumentTypes...>* f;
	};
	template <typename T, typename... ArgumentTypes> bool operator==(const Function<T(ArgumentTypes...)> &f1, std:: nullptr_t){
		return !f1;
	}
	template <typename T, typename... ArgumentTypes> bool operator==(std:: nullptr_t,const Function<T(ArgumentTypes...)> &f1){
		return !f1;
	}
	template <typename T, typename... ArgumentTypes> bool operator!=(const Function<T(ArgumentTypes...)> &f1, std:: nullptr_t){
		return bool(f1);
	}
	template <typename T, typename... ArgumentTypes> bool operator!=(std:: nullptr_t,const Function<T(ArgumentTypes...)> &f1){
		return bool(f1);
	}
}
#endif
