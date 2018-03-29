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

#include <stdio.h>
#include <Core\Tools\Common.hpp>

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