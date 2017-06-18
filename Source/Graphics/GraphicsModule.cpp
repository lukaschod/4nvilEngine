#include <Graphics\GraphicsModule.h>
#include <Math\Math.h>

GraphicsModule::GraphicsModule(uint32_t bufferCount) : CmdModule(bufferCount)
{
}

void GraphicsModule::RecordPushDebug(const char* name)
{
	auto buffer = GetRecordingBuffer();
	auto& stream = buffer->stream;
	auto commandCode = kGraphicsCommandCodePushDebug;
	stream.Write(commandCode);
	stream.Write(name);
	buffer->commandCount++;
}

void GraphicsModule::RecordPopDebug()
{
	auto buffer = GetRecordingBuffer();
	auto& stream = buffer->stream;
	auto commandCode = kGraphicsCommandCodePopDebug;
	stream.Write(commandCode);
	buffer->commandCount++;
}

void GraphicsModule::ExecuteCommand(IOStream& stream, uint32_t commandCode)
{
	auto gfxCmdCode = (GraphicsCommandCode)commandCode;
	switch (gfxCmdCode)
	{
	case kGraphicsCommandCodePushDebug:
	{
		const char* name;
		stream.Read(name);
		printf("Push Debug: %s\n", name);
		break;
	}

	case kGraphicsCommandCodePopDebug:
	{
		printf("Pop Debug\n");
		break;
	}

	default:
		LogError("Command is not known");
		break;
	}
}
