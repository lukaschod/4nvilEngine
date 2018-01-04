#pragma once

#include <Windows\Common.h>
#include <d3d12.h>
#include <d3dx12.h>

#pragma comment(lib, "d3d12.lib")

#ifdef D12_DEBUG
#	define D12_DEBUG_TRACE(...) TRACE(__VA_ARGS__)
#else
#	define D12_DEBUG_TRACE(...)
#endif