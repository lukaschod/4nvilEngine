#pragma once

namespace Core::Directory
{
	const size_t maxPathSize = 260;

	// Returns path to current executable location
	const char* GetExecutablePath();
}