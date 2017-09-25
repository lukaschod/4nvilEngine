#include <Modules\CmdModule.h>
#include <Math\Math.h>

CmdModule::CmdModule(uint32_t bufferCount, uint32_t workersCount) :
	backbufferIndex(0),
	contexts(workersCount)
{
	bufferCounter = 0;
}

void CmdModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	cmdBufferPoolModule = ExecuteAfter<CmdBufferPoolModule>(moduleManager);
}

CmdBuffer* CmdModule::GetRecordingBuffer(const ExecutionContext& context) 
{ 
	auto& bufferContext = contexts[context.workerIndex];
	auto& buffers = bufferContext.buffers;

	// If no buffers is present add new one
	if (buffers.empty())
	{
		auto buffer = cmdBufferPoolModule->Pull(context, bufferCounter++);
		buffers.push_back(buffer);
		return buffer;
	}
	
	// Check if same module is executed 
	auto buffer = buffers.back();
	if (buffer->executingModule == context.executingModule)
		return buffer;

	buffer = cmdBufferPoolModule->Pull(context, bufferCounter++);
	buffers.push_back(buffer);
	return buffer;
}

size_t CmdModule::GetExecutionkSize() { return 1; }

void CmdModule::Execute(const ExecutionContext& context)
{
	cmdBuffersToExecute.clear();
	for (auto& cmdContext : contexts)
	{
		auto& buffers = cmdContext.buffers;
		if (buffers.empty())
			continue;
		cmdBuffersToExecute.push_back_list(buffers);
		buffers.clear();
	}

	std::sort(cmdBuffersToExecute.begin(), cmdBuffersToExecute.end(), 
		[](CmdBuffer* first, CmdBuffer* second) { return first->index < second->index; });

	for (auto buffer : cmdBuffersToExecute)
	{
		// Reset buffer for executing
		auto& stream = buffer->stream;
		buffer->stream.Set_offset(0);
		buffer->state = kCmdBufferStateExecuted;

		for (int i = 0; i < buffer->commandCount; i++)
		{
			uint32_t commandCode;
			stream.Read(commandCode);
			ASSERT(ExecuteCommand(context, stream, commandCode));
		}
	}
}