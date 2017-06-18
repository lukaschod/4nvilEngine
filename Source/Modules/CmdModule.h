#pragma once

#include <Common\EngineCommon.h>
#include <Common\IOStream.h>
#include <Modules\Module.h>
#include <queue>

struct CmdBuffer
{
	CmdBuffer() : stream(), commandCount(0) {}
	IOStream stream;
	size_t commandCount;
};

class CmdModule : public Module
{
public:
	CmdModule(uint32_t bufferCount);

	virtual void Execute(uint32_t offset, size_t size) override;
	virtual size_t GetExecutionkSize() override;
	virtual size_t GetSplitExecutionTreshold() override;

protected:
	virtual void ExecuteCommand(IOStream& stream, uint32_t commandCode) = 0;

	CmdBuffer* GetRecordingBuffer();
	CmdBuffer* GetExecutingBuffer();

private:
	std::vector<CmdBuffer*> buffers;
	uint32_t recordingBufferIndex;
	uint32_t executingBufferIndex;
};