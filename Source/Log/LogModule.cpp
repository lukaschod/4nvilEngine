#include <Log\LogModule.h>
#include <Math\Math.h>
#include <stdarg.h>

LogModule::LogModule(uint32_t bufferCount, uint32_t workersCount) : CmdModule(bufferCount, workersCount){}

SERIALIZE_METHOD_ARG1(LogModule, Message, const char*);

DECLARE_COMMAND_CODE(MessageF);
void LogModule::RecordMessageF(const ExecutionContext& context, const char* format, ...)
{
	va_list arguments;
	va_start(arguments, format);

	auto buffer = GetRecordingBuffer(context);
	auto& stream = buffer->stream;
	stream.Write(kCommandCodeMessageF);
	stream.Write(format);
	stream.Write(arguments);
	buffer->commandCount++;
}

bool LogModule::ExecuteCommand(const ExecutionContext& context, IOStream& stream, uint32_t commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(Message, const char*, message);
		printf(message);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG2_START(MessageF, const char*, message, va_list, arguments);
		vprintf(message, arguments);
		va_end(arguments);
		DESERIALIZE_METHOD_END;
	}
	return false;
}
