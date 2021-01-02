#define LAMBDA_API_NAMESPACE_BEGIN namespace Lambda {
#define LAMBDA_API_NAMESPACE_END }

//#define LAMBDA_STATIC

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