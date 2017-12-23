#include "CmdBufferPoolModule.h"

CmdBufferPoolModule::CmdBufferPoolModule(uint32_t workerCount) :
	workerCount(workerCount)
{
	contexts = new CmdBufferPoolModule::Context[workerCount];
}

CmdBufferPoolModule::~CmdBufferPoolModule()
{
	delete[] contexts;
}

void CmdBufferPoolModule::Execute(const ExecutionContext & context)
{
	EXT_TRACE("CmdBufferPoolModule::Execute");
	for (uint32_t i = 0; i < workerCount; i++)
	{
		auto& poolContext = contexts[i];
		for (auto buffer : poolContext.cmdBuffers)
			if (buffer->state == CmdBufferStateExecuted)
			{
				poolContext.readyCmdBuffers.push(buffer);
				buffer->state = CmdBufferStateNotUsed;
			}
	}

	/*int totalBufferSize = 0;
	for (uint32_t i = 0; i < workerCount; i++)
	{
		auto& poolContext = contexts[i];
		for (auto buffer : poolContext.cmdBuffers)
		{
			totalBufferSize += buffer->stream.GetCapacity();
		}
	}
	TRACE("Capacity of all streams in cmdbuffers %d MB", totalBufferSize / (1000 * 1000));*/
}

CmdBuffer* CmdBufferPoolModule::Pull(const ExecutionContext& context, uint64_t index)
{
	auto& poolContext = contexts[context.workerIndex];

	// Try to get any available buffer
	if (!poolContext.readyCmdBuffers.empty())
	{
		auto buffer = poolContext.readyCmdBuffers.front();
		Reset(buffer, index, context);
		poolContext.readyCmdBuffers.pop();
		return buffer;
	}

	static int bufferCounter = 0;
	bufferCounter++;
	ASSERT(bufferCounter < 800);

	// Create new buffer
	auto buffer = new CmdBuffer();
	Reset(buffer, index, context);
	poolContext.cmdBuffers.push_back(buffer);
	return buffer;
}

void CmdBufferPoolModule::Reset(CmdBuffer* buffer, uint64_t index, const ExecutionContext& context)
{
	ASSERT(buffer->state == CmdBufferStateNotUsed);
	buffer->stream.Reset();
	buffer->commandCount = 0;
	buffer->index = index;
	buffer->workerIndex = context.workerIndex;
	buffer->executingModule = context.executingModule;
	buffer->state = CmdBufferStateUsed;
}
