#pragma once

#include <Tools\Console.h>

// Automatically enable asserting in debug mode
#if !defined(ENABLED_ASSERT) && defined(ENABLED_DEBUG)
#define ENABLED_ASSERT
#endif

#define STRINGIFY(x) #x
#define STRINGIFY_BUILTIN(x) STRINGIFY(x)

// Assert with custom message use it for debugging purpose
#define RELEASE_ASSERT_MSG(isFalse, ...) \
	if (!(bool)(isFalse)) \
	{ \
		Core::Console::Write("ERROR: Assertion failed \'" #isFalse "\'\n"); \
		Core::Console::Write("    In: " STRINGIFY_BUILTIN(__FILE__) " @ " STRINGIFY_BUILTIN(__LINE__) "\n"); \
		Core::Console::Write("    Info:"); \
		Core::Console::WriteFmt(__VA_ARGS__); \
		Core::Console::Write("\n"); \
		__debugbreak(); \
	}

// Default assert use it for debugging purpose
#define RELEASE_ASSERT(isFalse) \
	if (!(bool)(isFalse)) \
	{ \
		Console::Write("ERROR: Assertion failed \'" #isFalse "\'\n"); \
		Console::Write("    In: " STRINGIFY_BUILTIN(__FILE__) " @ " STRINGIFY_BUILTIN(__LINE__) "\n"); \
		__debugbreak(); \
	}

// Unconditional assert
#define RELEASE_ERROR(...) \
	{ \
		Core::Console::Write("ERROR:"); \
		Core::Console::WriteFmt(__VA_ARGS__); \
		Core::Console::Write("\n"); \
		Core::Console::Write("    In: " STRINGIFY_BUILTIN(__FILE__) " @ " STRINGIFY_BUILTIN(__LINE__) "\n"); \
		__debugbreak(); \
	}

// Prints into IDEA console
#define TRACE(...) \
	{ \
		Core::Console::Write("TRACE:"); \
		Core::Console::WriteFmt(__VA_ARGS__); \
		Core::Console::Write("\n"); \
	}

#ifdef ENABLED_ASSERT
#	define ASSERT(isFalse) RELEASE_ASSERT_MSG(isFalse)
#	define ASSERT_MSG(isFalse, ...) ASSERT(isFalse)
#	define ERROR(...) ERROR(isFalse)
#else
#	define ASSERT(isFalse) (void)(isFalse)
#	define ASSERT_MSG(isFalse, ...) (void)(isFalse)
#	define ERROR(...) (void)0
#endif