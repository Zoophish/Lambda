//#define LAMBDA_API_USE_NAMESPACE

#ifdef LAMBDA_API_USE_NAMESPACE
	#define LAMBDA_API_NAMESPACE_BEGIN namespace Lambda {
	#define LAMBDA_API_NAMESPACE_END }
#else
	#define LAMBDA_API_NAMESPACE_BEGIN
	#define LAMBDA_API_NAMESPACE_END
#endif

/* define LAMBDA_STATIC in preprocessor definitions for static linkage. */
#ifndef LAMBDA_STATIC
	#ifdef _WIN32
		#ifdef _EXPORTING
			#define LAMBDA_API extern "C" __declspec(dllexport)
		#else
			#define LAMBDA_API extern "C" __declspec(dllimport)
		#endif
	#endif
#else
	#define LAMBDA_API
#endif

/* compile with C++ compiler for int-type enums (easier compatibility with other languages) */
#ifdef __cplusplus
	#define INT_ENUM : int 
#else
	#define INT_ENUM
#endif