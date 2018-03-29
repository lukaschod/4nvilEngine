/*
* Copyright (c) Lukas Chodosevicius
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#pragma once

#include <Core\Tools\Windows\Common.hpp>
#include <d3d12.h>
#include <d3dx12.h>
#include <comdef.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <wrl.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")

using Microsoft::WRL::ComPtr;

// Automatically enable debug layer on debug modes 
#if !defined(ENABLED_D12_DEBUG_LAYER) && defined(ENABLED_DEBUG)
#	define ENABLED_D12_DEBUG_LAYER
#endif