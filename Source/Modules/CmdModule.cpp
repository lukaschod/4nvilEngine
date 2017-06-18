#include <Modules\CmdModule.h>
#include <Math\Math.h>

CmdModule::CmdModule(uint32_t bufferCount)
{
	recordingBufferIndex = 0;
	executingBufferIndex = 0;
	for (auto i = 0; i < bufferCount; i++)
	{
		auto buffer = new CmdBuffer();
		buffers.push_back(buffer);
	}
}

CmdBuffer* CmdModule::GetRecordingBuffer() { return buffers.at(recordingBufferIndex); }
CmdBuffer* CmdModule::GetExecutingBuffer() { return buffers.at(executingBufferIndex); }
size_t CmdModule::GetExecutionkSize() { return GetExecutingBuffer()->commandCount; }
size_t CmdModule::GetSplitExecutionTreshold() { return UINT32_MAX; }

void CmdModule::Execute(uint32_t offset, size_t size)
{
	recordingBufferIndex = (recordingBufferIndex + 1) % buffers.size();
	auto recordingBuffer = GetRecordingBuffer();
	recordingBuffer->stream.Set_offset(0);
	recordingBuffer->commandCount = 0;

	auto buffer = GetExecutingBuffer();
	auto& stream = buffer->stream;
	buffer->stream.Set_offset(0);
	for (int i = 0; i < buffer->commandCount; i++)
	{
		uint32_t commandCode;
		stream.Read(&commandCode, sizeof(commandCode));
		ExecuteCommand(stream, commandCode);
	}

	executingBufferIndex = (executingBufferIndex + 1) % buffers.size();
}