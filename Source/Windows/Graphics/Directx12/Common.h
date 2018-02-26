#pragma once

#include <Tools\Windows\Common.h>
#include <d3d12.h>
#include <d3dx12.h>

#pragma comment(lib, "d3d12.lib")

// Automatically enable debug layer on debug modes 
#if !defined(ENABLED_D12_DEBUG_LAYER) && defined(ENABLED_DEBUG)
#	define ENABLED_D12_DEBUG_LAYER
#endif