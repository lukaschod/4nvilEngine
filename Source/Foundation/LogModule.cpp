#include <stdarg.h>
#include <Tools\Math\Math.h>
#include <Tools\IO\Directory.h>
#include <Tools\Collections\StringBuilder.h>
#include <Foundation\LogModule.h>

using namespace Core;
using namespace Core::IO;

void LogModule::Execute(const ExecutionContext& context)
{
	MARK_FUNCTION;
	if (!output.IsOpened())
		OpenStream();
	base::Execute(context);
	output.Flush();
}

SERIALIZE_METHOD_ARG1(LogModule, Write, const char*);

DECLARE_COMMAND_CODE(WriteFmt);
void LogModule::RecWriteFmt(const ExecutionContext& context, const char* format, ...)
{
	// Construct message
	va_list ap;
	va_start(ap, format);
	StringBuilder<1024> messageBuilder;
	messageBuilder.AppendFmt(format, ap);
	va_end(ap);

	// Fetch the constructed message
	auto message = messageBuilder.ToString();
	auto size = messageBuilder.GetSize();

	auto buffer = GetRecordingBuffer(context);
	auto& stream = buffer->stream;
	stream.Write(TO_COMMAND_CODE(WriteFmt));
	stream.Write(size);
	stream.Write((void*)message, size);
	stream.Align();
	buffer->commandCount++;
}

void LogModule::OpenStream()
{
	StringBuilder<Directory::maxPathSize> path;
	path.AppendFmt("%s\\%s", Directory::GetExecutablePath(), "Log.txt");
	output.Open(path.ToString(), FileMode::Create, FileAccess::Write);
}

void LogModule::CloseStream()
{
	if (output.IsOpened())
		output.Close();
}

bool LogModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(Write, const char*, message);
		output.WriteFmt(message);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG1_START(WriteFmt, size_t, size);
		const char* message = (const char*)stream.Get_data();
		stream.Set_data(stream.Get_data() + size);
		output.WriteFmt(message);
		DESERIALIZE_METHOD_END;
	}
	return false;
}
