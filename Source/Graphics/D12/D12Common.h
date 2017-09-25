#pragma once

#include <Common\EngineCommon.h>

#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <comdef.h>

#ifdef D12_DEBUG
#define D12_DEBUG_TRACE(...) TRACE(__VA_ARGS__)
#else
#define D12_DEBUG_TRACE(...)
#endif

#ifdef RELEASE
#define ASSERT_SUCCEEDED(hr) (void)(hr)
#else
#define ASSERT_SUCCEEDED(hr) \
	if (FAILED(hr)) \
	{ \
		_com_error err(hr); \
		LPCTSTR errMsg = err.ErrorMessage(); \
		Console::Print("ERROR: Assertion failed \'" #hr "\'\n"); \
		Console::Print("    In: " STRINGIFY_BUILTIN(__FILE__) " @ " STRINGIFY_BUILTIN(__LINE__) "\n"); \
		Console::Print("    Info: "); \
		Console::Print(errMsg); \
		Console::Printf(" (hr = 0x%08X)", hr); \
		Console::Print("\n"); \
		__debugbreak(); \
	}
#endif