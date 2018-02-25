#pragma once

#include <Tools\Common.h>
#include <stdio.h>

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
	void Flush();
	inline bool IsOpened() const { return isOpened; }

private:
	const char* TryGetMode(FileMode mode, FileAccess access);

private:
	FILE* file;
	bool isOpened;
};