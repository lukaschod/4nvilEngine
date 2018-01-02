#define _CRT_SECURE_NO_WARNINGS
#include <Tools\IO\FileStream.h>

FileStream::FileStream()
	: isOpened(false)
{
}

bool FileStream::Open(const char * path, FileMode mode, FileAccess access)
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
	fread(data, sizeof(uint8_t), size, file);
}

void FileStream::Write(void* data, size_t size)
{
	ASSERT(isOpened);
	fwrite(data, sizeof(uint8_t), size, file);
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

	case FileModeAppend:
	{
		switch (access)
		{
		case FileAccessRead:
			return "rab+";
		case FileAccessWrite:
			return "wab+";
		default:
			return nullptr;
		}
	}

	case FileModeOpen:
	{
		switch (access)
		{
		case FileAccessRead:
			return "rb";
		case FileAccessWrite:
			return "wb";
		case FileAccessReadWrite:
			return "rb+";
		default:
			return nullptr;
		}
	}

	case FileModeCreate:
	{
		switch (access)
		{
		case FileAccessRead:
			return "rb+";
		case FileAccessWrite:
			return "wb";
		case FileAccessReadWrite:
			return "wb+";
		default:
			return nullptr;
		}
	}

	}
	return nullptr;
}
