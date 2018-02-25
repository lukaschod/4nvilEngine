#pragma once

#if ENABLED_WINDOWS

// Exclude rarely-used stuff from Windows headers for speed up
#ifndef WIN32_LEAN_AND_MEAN
#	define WIN32_LEAN_AND_MEAN 
#endif
#ifndef NOMINMAX
#	define NOMINMAX
#endif

// Exclude ERROR macros for our usage
#ifndef NOGDI
#	define NOGDI
#endif

#include <windows.h>

#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <comdef.h>

#include <Tools\Common.h>

#ifdef ENABLED_DEBUG
#define ASSERT_SUCCEEDED(hr) \
	if (FAILED(hr)) \
	{ \
		_com_error err(hr); \
		LPCTSTR errMsg = err.ErrorMessage(); \
		Console::Write("ERROR: Assertion failed \'" #hr "\'\n"); \
		Console::Write("    In: " STRINGIFY_BUILTIN(__FILE__) " @ " STRINGIFY_BUILTIN(__LINE__) "\n"); \
		Console::Write("    Info: "); \
		Console::Write(errMsg); \
		Console::WriteFmt(" (hr = 0x%08X)", hr); \
		Console::Write("\n"); \
		__debugbreak(); \
	}
#else
#define ASSERT_SUCCEEDED(hr) (void)(hr)
#endif
#endif