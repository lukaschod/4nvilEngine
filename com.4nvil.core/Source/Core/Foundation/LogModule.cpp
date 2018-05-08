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

#include <stdarg.h>
#include <Core/Tools/Math/Math.hpp>
#include <Core/Tools/IO/Directory.hpp>
#include <Core/Tools/Collections/StringBuilder.hpp>
#include <Core/Foundation/LogModule.hpp>

using namespace Core;
using namespace Core::IO;

Void LogModule::Execute(const ExecutionContext& context)
{
    MARK_FUNCTION;
    if (!output.IsOpened())
        OpenStream();
    base::Execute(context);
    output.Flush();
}

SERIALIZE_METHOD_ARG1(LogModule, Write, const char*);

DECLARE_COMMAND_CODE(WriteFmt);
Void LogModule::RecWriteFmt(const ExecutionContext& context, const char* format, ...)
{
    // Construct message
    va_list ap;
    va_start(ap, format);
    StringBuilder<1024 * 4> messageBuilder;
    messageBuilder.AppendFmt(format, ap);
    va_end(ap);

    // Fetch the constructed message
    auto message = messageBuilder.ToString();
    auto size = messageBuilder.GetSize();

    auto buffer = GetRecordingBuffer(context);
    auto& stream = buffer->stream;
    stream.Write(TO_COMMAND_CODE(WriteFmt));
    stream.Write(size);
    stream.Write((Void*) message, size);
    stream.Align();
    buffer->commandCount++;
}

Void LogModule::OpenStream()
{
    StringBuilder<Directory::maxPathSize> path;
    path.AppendFmt("%s\\%s", Directory::GetExecutablePath(), "Log.txt");
    output.Open(path.ToString(), FileMode::Create, FileAccess::Write);
}

Void LogModule::CloseStream()
{
    if (output.IsOpened())
        output.Close();
}

Bool LogModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
    switch (commandCode)
    {
        DESERIALIZE_METHOD_ARG1_START(Write, const char*, message);
        output.WriteFmt(message);
        DESERIALIZE_METHOD_END;

        DESERIALIZE_METHOD_ARG1_START(WriteFmt, UInt, size);
        const char* message = (const char*) stream.Get_data();
        stream.Set_data(stream.Get_data() + size);
        output.WriteFmt(message);
        DESERIALIZE_METHOD_END;
    }
    return false;
}
