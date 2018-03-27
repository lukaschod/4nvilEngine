#pragma once

#include <Core\Tools\String.h>
#include <Core\Tools\Collections\List.h>

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