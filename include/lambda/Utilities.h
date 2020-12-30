#define LAMBDA_NAMESPACE_BEGIN namespace lambda {
#define LAMBDA_NAMESPACE_END }

/*
	Define LAMBDA_DYNAMIC to export API symbols to shared library.
*/
//#define LAMBDA_DYNAMIC

#ifdef LAMBDA_DYNAMIC
	#ifdef _WIN32
		#ifdef _EXPORTING
			#define LAMBDA_EXPORT __declspec(dllexport)
		#else
			#define LAMBDA_EXPORT __declspec(dllimport)
		#endif
	#endif
#else
	#define LAMBDA_EXPORT
#endif