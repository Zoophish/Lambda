#pragma once
/* 
	Static delegate by Sam Warren.

	Usage:
		Delegate< ReturnType, Arg1, Arg2, Arg3, ... >

	E.g:
		struct Foo {
			int i;

			int Sum(int val) const {
				return i + val;
			}
		};

		Foo f = { 3 };
		MyDelegate d = MyDelegate::FromFunction<Foo, &Foo::Sum>(d);
*/

template<class ReturnType, typename... params>
class Delegate {
	typedef ReturnType(*Method)(void *callee, params...);

	public:
		Delegate(void *_callee = nullptr, Method _function = nullptr) : callee(_callee), callback(_function) {}

		template <class T, ReturnType(T:: *TMethod)(params...)>
		inline static Delegate FromFunction(T *_callee) {
			return Delegate(_callee, &MethodCaller<T, TMethod>);
		}

		template <class T, ReturnType(T:: *TMethod)(params...) const>
		inline static Delegate FromFunction(T *_callee) {
			return Delegate(_callee, &MethodCaller<T, TMethod>);
		}

		inline ReturnType operator()(params... _params) const {
			return (*callback)(callee, _params...);
		}

		inline ReturnType operator()(params... _params) {
			return (*callback)(callee, _params...);
		}

		inline operator bool() const {
			return (bool)callee;
		}

	private:
		void *callee;
		Method callback;

		template <class T, ReturnType(T:: *TMethod)(params...) const>
		inline static ReturnType MethodCaller(void *_callee, params... _params) {
			T *p = static_cast<T *>(_callee);
			return (p->*TMethod)(_params...);
		}

		template <class T, ReturnType(T:: *TMethod)(params...)>
		inline static ReturnType MethodCaller(void *_callee, params... _params) {
			T *p = static_cast<T *>(_callee);
			return (p->*TMethod)(_params...);
		}
};