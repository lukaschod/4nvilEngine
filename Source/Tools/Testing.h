#pragma once

#include <Tools\Console.h>

#ifdef ERROR
#	undef ERROR
#endif
#ifdef ASSERT
#	undef ASSERT
#endif

#ifdef DEBUG
#	define STRINGIFY(x) #x
#	define STRINGIFY_BUILTIN(x) STRINGIFY(x)
#	define ASSERT_MSG(isFalse, ...) \
	if (!(bool)(isFalse)) \
	{ \
		Console::Write("ERROR: Assertion failed \'" #isFalse "\'\n"); \
		Console::Write("    In: " STRINGIFY_BUILTIN(__FILE__) " @ " STRINGIFY_BUILTIN(__LINE__) "\n"); \
		Console::Write("    Info:"); \
		Console::WriteFmt(__VA_ARGS__); \
		Console::Write("\n"); \
		__debugbreak(); \
	}
#	define ASSERT(isFalse) \
	if (!(bool)(isFalse)) \
	{ \
		Console::Write("ERROR: Assertion failed \'" #isFalse "\'\n"); \
		Console::Write("    In: " STRINGIFY_BUILTIN(__FILE__) " @ " STRINGIFY_BUILTIN(__LINE__) "\n"); \
		__debugbreak(); \
	}

#	define ERROR(...) \
	{ \
		Console::Write("ERROR:"); \
		Console::WriteFmt(__VA_ARGS__); \
		Console::Write("\n"); \
		Console::Write("    In: " STRINGIFY_BUILTIN(__FILE__) " @ " STRINGIFY_BUILTIN(__LINE__) "\n"); \
		__debugbreak(); \
	}
#else
#	define ASSERT(isFalse) (void)(isFalse)
#	define ASSERT_MSG(isFalse, ...) (void)(isFalse)
#	define ERROR(...) (void)0
#endif

#	define TRACE(...) \
	{ \
		Console::Write("TRACE:"); \
		Console::WriteFmt(__VA_ARGS__); \
		Console::Write("\n"); \
	}

//#define EXT_DEBUG

#ifdef EXT_DEBUG
#	define EXT_TRACE(...) TRACE(__VA_ARGS__)
#else
#	define EXT_TRACE(...)
#endif