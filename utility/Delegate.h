/*---- Sam Warren 2019 ----
Compile-time delegate to a const function.
*/
template<class ReturnType, typename... params>
class ConstDelegate {
	typedef ReturnType(*Type)(void *callee, params...);

	public:
		ConstDelegate(void *_callee = nullptr, Type _function = nullptr) : callee(_callee), callback(_function) {}

		template <class T, ReturnType(T:: *TMethod)(params...) const>
		inline static ConstDelegate FromFunction(T *_callee) {
			return ConstDelegate(_callee, &MethodCaller<T, TMethod>);
		}

		inline void operator()(params... _params) const {
			return (*callback)(callee, _params...);
		}

		inline operator bool() const {
			return (bool)callee;
		}

	private:
		void *callee;
		Type callback;

		template <class T, ReturnType(T:: *TMethod)(params...) const>
		inline static ReturnType MethodCaller(void *_callee, params... _params) {
			T *p = static_cast<T *>(_callee);
			return (p->*TMethod)(_params...);
		}
};