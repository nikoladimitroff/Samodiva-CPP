#pragma once
#include "Logging.h"

inline void SamodivaCrash()
{
	*((char*)-1) = 'x';
}
#define _STRINGIFY_MACRO(x) #x
#define STRINGIFY_MACRO(x) _STRINGIFY_MACRO(x)

#define ASSERT_RETURN(expression) \
	do \
	{ \
		if (!(expression)) \
		{ \
			SAMODIVA_LOG(Error, "Assert failed: " #expression " at " __FILE__ ":" STRINGIFY_MACRO(__LINE__)); \
			return; \
		} \
	} while(0, 0)
#define ASSERT_RETURN_VALUE(expression, defaultValue) \
	do \
	{ \
		if (!(expression)) \
		{ \
			SAMODIVA_LOG(Error, "Assert failed: " #expression " at " __FILE__ ":" STRINGIFY_MACRO(__LINE__)); \
			return defaultValue; \
		} \
	} while(0, 0)
#define ASSERT_FATAL(expression) \
	do \
	{ \
		if (!(expression)) \
		{ \
			SAMODIVA_LOG(Fatal, "Fatal assert failed: " #expression " at " __FILE__ ":" STRINGIFY_MACRO(__LINE__)); \
			SamodivaCrash(); \
		} \
	} while(0, 0)
