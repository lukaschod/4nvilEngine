#include <Windows\Graphics\D12\D12GraphicsExecutorModule.h>
#include <Windows\Graphics\D12\D12GraphicsModule.h>
#include <Windows\Graphics\D12\D12GraphicsPlannerModule.h>
#include <algorithm>

D12GraphicsExecutorModule::D12GraphicsExecutorModule()
	: completedBufferIndex(0)
{
}

void D12GraphicsExecutorModule::RecCmdBuffer(const ExecutionContext& context, const D12CmdBuffer* buffer)
{
	auto& executerContext = executerContexts[context.workerIndex];
	executerContext.recordedCmds.push_back(buffer);
}

void D12GraphicsExecutorModule::SetupExecuteOrder(ModuleManager * moduleManager)
{
	Module::SetupExecuteOrder(moduleManager);
	executerContexts.resize(moduleManager->GetWorkerCount());
}

void D12GraphicsExecutorModule::Execute(const ExecutionContext& context)
{
	PROFILE_FUNCTION;
	// TODO: Make sure no allocation happens
	cmdBuffersToExecute.clear();
	for (auto& executerContext : executerContexts)
	{
		cmdBuffersToExecute.push_back_list(executerContext.recordedCmds);
		executerContext.recordedCmds.clear();
	}

	std::sort(cmdBuffersToExecute.begin(), cmdBuffersToExecute.end(), 
		[](const D12CmdBuffer* first, const D12CmdBuffer* second) { return first->index < second->index; });

	auto back = cmdBuffersToExecute.back();
	for (auto cmdBuffer : cmdBuffersToExecute)
	{
		cmdBuffer->queue->Execute((D12CmdBuffer*)cmdBuffer, cmdBuffer == back); // TODO: Fix this const shit
	}

	// TODO: remove onc we have buffer caching
	if (!cmdBuffersToExecute.empty())
		cmdBuffersToExecute.back()->queue->WaitForBufferIndexToComplete(back->index);
	completedBufferIndex = back->index;
}