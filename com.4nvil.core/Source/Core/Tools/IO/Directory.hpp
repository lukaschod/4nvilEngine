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

#include <Core\Tools\String.hpp>
#include <Core\Tools\Collections\List.hpp>

namespace Core::Directory
{
	const size_t maxPathSize = 260;

	// Returns path to current executable location
	const char* GetExecutablePath();

	// Returns the names of subdirectories (including their paths) in the specified directory
	List<String> GetDirectories(String& path);

	// Returns the names of files (including their paths) in the specified directory
	List<String> GetFiles(String& path);
}