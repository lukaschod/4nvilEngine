#include <Core\Tools\Windows\Common.h>
#include <Core\Tools\Common.h>
#include <Core\Tools\IO\Directory.h>
#include <filesystem>

namespace Core::Directory
{
	namespace filesystem = std::experimental::filesystem;

	const char* GetExecutablePath()
	{
		static char path[maxPathSize];
		static bool pathValid = false;

		// Store executable path on demand
#if ENABLED_WINDOWS
		if (!pathValid)
		{
			HMODULE hModule = GetModuleHandleW(NULL);
			GetModuleFileName(hModule, path, maxPathSize);
			char* last = strrchr(path, '\\');
			*last = 0;
			pathValid = true;
		}
#endif

		ASSERT(pathValid);

		return path;
	}

	List<String> GetDirectories(String& path)
	{
		List<String> out;
		for (auto& directory : filesystem::directory_iterator(path))
			if (directory.status().type() == filesystem::v1::file_type::directory)
				out.push_back(directory.path().string());
		return out;
	}

	List<String> GetFiles(String& path)
	{
		List<String> out;
		for (auto& directory : filesystem::directory_iterator(path))
			if (directory.status().type() == filesystem::v1::file_type::regular)
				out.push_back(directory.path().string());
		return out;
	}
}