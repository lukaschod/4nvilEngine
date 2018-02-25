#pragma once

#include <Tools\Console.h>

#ifdef ERROR
#	undef ERROR
#endif
#ifdef ASSERT
#	undef ASSERT
#endif

#if !defined(ENABLED_ASSERT) && defined(ENABLED_DEBUG)
#define ENABLED_ASSERT
#endif

#ifdef ENABLED_ASSERT
#	define STRINGIFY(x) #x
#	define STRINGIFY_BUILTIN(x) STRINGIFY(x)
#	define ASSERT_MSG(isFalse, ...) \
	if (!(bool)(isFalse)) \
	{ \
		Core::Console::Write("ERROR: Assertion failed \'" #isFalse "\'\n"); \
		Core::Console::Write("    In: " STRINGIFY_BUILTIN(__FILE__) " @ " STRINGIFY_BUILTIN(__LINE__) "\n"); \
		Core::Console::Write("    Info:"); \
		Core::Console::WriteFmt(__VA_ARGS__); \
		Core::Console::Write("\n"); \
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
		Core::Console::Write("ERROR:"); \
		Core::Console::WriteFmt(__VA_ARGS__); \
		Core::Console::Write("\n"); \
		Core::Console::Write("    In: " STRINGIFY_BUILTIN(__FILE__) " @ " STRINGIFY_BUILTIN(__LINE__) "\n"); \
		__debugbreak(); \
	}
#else
#	define ASSERT(isFalse) (void)(isFalse)
#	define ASSERT_MSG(isFalse, ...) (void)(isFalse)
#	define ERROR(...) (void)0
#endif

#	define TRACE(...) \
	{ \
		Core::Console::Write("TRACE:"); \
		Core::Console::WriteFmt(__VA_ARGS__); \
		Core::Console::Write("\n"); \
	}

#ifdef EXT_DEBUG
#	define EXT_TRACE(...) TRACE(__VA_ARGS__)
#else
#	define EXT_TRACE(...)
#endif