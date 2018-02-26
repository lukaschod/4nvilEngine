#include <Input\InputModule.h>

using namespace Core;

void InputModule::Execute(const ExecutionContext & context)
{
	MARK_FUNCTION;
	Reset();
	base::Execute(context);
}

bool InputModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_START(Input);
		auto size = stream.FastRead<size_t>();
		auto pointer = stream.Get_data();
		inputStream.Write(pointer, size);
		DESERIALIZE_METHOD_END;
	}
	return false;
}

void InputModule::Reset()
{
	inputStream.Reset();
}

DECLARE_COMMAND_CODE(Input);
void InputModule::RecInput(const ExecutionContext& context, InputCommandCode commandCode, uint8* data, size_t size)
{
	auto buffer = GetRecordingBuffer(context);
	auto& stream = buffer->stream;
	stream.Write(TO_COMMAND_CODE(Input));
	stream.Write(size);
	stream.Write(data, size);
	stream.Align();
	buffer->commandCount++;
}
