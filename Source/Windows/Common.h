#pragma once

#include <Tools\Common.h>

#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <comdef.h>

#ifdef ENABLED_DEBUG
#define ASSERT_SUCCEEDED(hr) \
	if (FAILED(hr)) \
	{ \
		_com_error err(hr); \
		LPCTSTR errMsg = err.ErrorMessage(); \
		Core::Console::Write("ERROR: Assertion failed \'" #hr "\'\n"); \
		Core::Console::Write("    In: " STRINGIFY_BUILTIN(__FILE__) " @ " STRINGIFY_BUILTIN(__LINE__) "\n"); \
		Core::Console::Write("    Info: "); \
		Core::Console::Write(errMsg); \
		Core::Console::WriteFmt(" (hr = 0x%08X)", hr); \
		Core::Console::Write("\n"); \
		__debugbreak(); \
	}
#else
	#define ASSERT_SUCCEEDED(hr) (void)(hr)
#endif