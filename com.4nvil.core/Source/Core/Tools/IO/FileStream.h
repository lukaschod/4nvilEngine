#pragma once

#include <stdio.h>
#include <Core\Tools\Common.h>

namespace Core::IO
{
	enum class FileMode
	{
		Append,
		Open,
		Create,
	};

	enum class FileAccess
	{
		Read,
		ReadWrite,
		Write,
	};

	class FileStream
	{
	public:
		FileStream();

		bool Open(const char* path, FileMode mode, FileAccess access);
		void Close();
		void Read(void* data, size_t size);
		void Write(void* data, size_t size);
		void ReadFmt(const char* format, ...);
		void WriteFmt(const char* format, ...);
		void WriteFmt(const char* format, va_list arguments);
		inline bool IsOpened() const { return isOpened; }

		// Forces all unwritten data to be written to the file
		void Flush();

	private:
		const char* TryGetMode(FileMode mode, FileAccess access);

	private:
		FILE* file;
		bool isOpened;
	};
}