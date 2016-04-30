#pragma once


#if defined(_WIN32)
	#define SAMODIVA_PLATFORM_WIN 1
#elif defined(__linux__)
	#define SAMODIVA_PLATFORM_LINUX 1
#elif defined(__APPLE_CC__) || defined(__APPLE_CPP__)
	#define SAMODIVA_PLATFORM_MAC 1
#endif

#if !defined(SAMODIVA_IMPORT_SYMBOL)
	#if defined(SAMODIVA_PLATFORM_WIN)
		#define SAMODIVA_EXPORT_SYMBOL __declspec(dllexport)
		#define SAMODIVA_IMPORT_SYMBOL __declspec(dllimport)
	#elif defined(SAMODIVA_PLATFORM_POSIX)
		#define SAMODIVA_EXPORT_SYMBOL __attribute__ ((visibility ("default")))
		#define SAMODIVA_IMPORT_SYMBOL
	#elif defined(SAMODIVA_PLATFORM_IOS)
		#define SAMODIVA_EXPORT_SYMBOL
		#define SAMODIVA_IMPORT_SYMBOL
	#endif
#endif


#ifdef SAMODIVA_EXPORTS
	#define SAMODIVA_API SAMODIVA_EXPORT_SYMBOL
#else
	#define SAMODIVA_API SAMODIVA_IMPORT_SYMBOL
#endif
