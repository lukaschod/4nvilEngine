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
		Console::Print("ERROR: Assertion failed \'" #isFalse "\'\n"); \
		Console::Print("    In: " STRINGIFY_BUILTIN(__FILE__) " @ " STRINGIFY_BUILTIN(__LINE__) "\n"); \
		Console::Print("    Info:"); \
		Console::Printf(__VA_ARGS__); \
		Console::Print("\n"); \
		__debugbreak(); \
	}
#	define ASSERT(isFalse) \
	if (!(bool)(isFalse)) \
	{ \
		Console::Print("ERROR: Assertion failed \'" #isFalse "\'\n"); \
		Console::Print("    In: " STRINGIFY_BUILTIN(__FILE__) " @ " STRINGIFY_BUILTIN(__LINE__) "\n"); \
		__debugbreak(); \
	}

#	define ERROR(...) \
	{ \
		Console::Print("ERROR:"); \
		Console::Printf(__VA_ARGS__); \
		Console::Print("\n"); \
		Console::Print("    In: " STRINGIFY_BUILTIN(__FILE__) " @ " STRINGIFY_BUILTIN(__LINE__) "\n"); \
		__debugbreak(); \
	}
#else
#	define ASSERT(isFalse) (void)(isFalse)
#	define ASSERT_MSG(isFalse, ...) (void)(isFalse)
#	define ERROR(...)
#endif

#	define TRACE(...) \
	{ \
		Console::Print("TRACE:"); \
		Console::Printf(__VA_ARGS__); \
		Console::Print("\n"); \
	}

//#define EXT_DEBUG

#ifdef EXT_DEBUG
#	define EXT_TRACE(...) TRACE(__VA_ARGS__)
#else
#	define EXT_TRACE(...)
#endif