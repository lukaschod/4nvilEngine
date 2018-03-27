#define _CRT_SECURE_NO_WARNINGS
#include <Core\Tools\IO\FileStream.hpp>
#include <stdarg.h>

using namespace Core::IO;

FileStream::FileStream()
	: isOpened(false)
{
}

bool FileStream::Open(const char* path, FileMode mode, FileAccess access)
{
	ASSERT_MSG(file == nullptr, "File is already opened");
	auto modeTexted = TryGetMode(mode, access);
	file = fopen(path, modeTexted);
	isOpened = file != nullptr;
	return isOpened;
}

void FileStream::Close()
{
	ASSERT(isOpened);
	fclose(file);
}

void FileStream::Read(void* data, size_t size)
{
	ASSERT(isOpened);
	fread(data, sizeof(uint8), size, file);
}

void FileStream::Write(void* data, size_t size)
{
	ASSERT(isOpened);
	fwrite(data, sizeof(uint8), size, file);
}

void FileStream::ReadFmt(const char* format, ...)
{
	ASSERT(isOpened);
	va_list ap;
	va_start(ap, format);
	vfscanf(file, format, ap);
	va_end(ap);
}

void FileStream::WriteFmt(const char* format, ...)
{
	ASSERT(isOpened);
	va_list ap;
	va_start(ap, format);
	vfprintf(file, format, ap);
	va_end(ap);
}

void FileStream::WriteFmt(const char* format, va_list arguments)
{
	ASSERT(isOpened);
	vfprintf(file, format, arguments);
}

void FileStream::Flush()
{
	ASSERT(isOpened);
	fflush(file);
}

const char* FileStream::TryGetMode(FileMode mode, FileAccess access)
{
	switch (mode)
	{

	case FileMode::Append:
	{
		switch (access)
		{
		case FileAccess::Read:
			return "rab+";
		case FileAccess::Write:
			return "wab+";
		default:
			return nullptr;
		}
	}

	case FileMode::Open:
	{
		switch (access)
		{
		case FileAccess::Read:
			return "rb";
		case FileAccess::Write:
			return "wb";
		case FileAccess::ReadWrite:
			return "rb+";
		default:
			return nullptr;
		}
	}

	case FileMode::Create:
	{
		switch (access)
		{
		case FileAccess::Read:
			return "rb+";
		case FileAccess::Write:
			return "wb";
		case FileAccess::ReadWrite:
			return "wb+";
		default:
			return nullptr;
		}
	}

	}
	return nullptr;
}
