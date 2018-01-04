#include <Foundation\LogModule.h>
#include <Tools\Math\Math.h>
#include <Tools\IO\Directory.h>
#include <Tools\Collections\StringBuilder.h>
#include <stdarg.h>

void LogModule::Execute(const ExecutionContext& context)
{
	if (!output.IsOpened())
		OpenStream();
	PipeModule::Execute(context);
}

SERIALIZE_METHOD_ARG1(LogModule, Write, const char*);

DECLARE_COMMAND_CODE(WriteFmt);
void LogModule::RecWriteFmt(const ExecutionContext& context, const char* format, ...)
{
	// Construct message
	va_list ap;
	va_start(ap, format);
	StringBuilder<512> messageBuilder;
	messageBuilder.AppendFmt(format, ap);
	va_end(ap);

	// Fetch the constructed message
	auto message = messageBuilder.ToString();
	auto size = messageBuilder.GetSize();

	auto buffer = GetRecordingBuffer(context);
	auto& stream = buffer->stream;
	stream.Write(CommandCodeWriteFmt);
	stream.Write(size);
	stream.Write((void*)message, size);
	buffer->commandCount++;
}

void LogModule::OpenStream()
{
	StringBuilder<MAX_PATH> path;
	path.AppendFmt("%s\\%s", Directory::GetExecutablePath(), "Log.txt");
	output.Open(path.ToString(), FileModeCreate, FileAccessWrite);
}

void LogModule::CloseStream()
{
	if (output.IsOpened())
		output.Close();
}

bool LogModule::ExecuteCommand(const ExecutionContext& context, MemoryStream& stream, uint32_t commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(Write, const char*, message);
		output.WriteFmt(message);
		output.Flush();
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG1_START(WriteFmt, size_t, size);
		const char* message = (const char*)stream.Get_data();
		stream.Set_data(stream.Get_data() + size);
		output.WriteFmt(message);
		output.Flush();
		DESERIALIZE_METHOD_END;
	}
	return false;
}
