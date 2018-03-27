#include <algorithm>
#include <Windows\Graphics\Directx12\GraphicsExecutorModule.hpp>
#include <Windows\Graphics\Directx12\CmdQueue.hpp>

using namespace Windows;
using namespace Windows::Directx12;

GraphicsExecutorModule::GraphicsExecutorModule() : completedBufferIndex(0) {}

void GraphicsExecutorModule::RecCmdBuffer(const ExecutionContext& context, const Directx12::CmdBuffer* buffer)
{
	auto& executerContext = executerContexts[context.workerIndex];
	executerContext.recordedCmds.push_back(buffer);
}

void GraphicsExecutorModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	base::SetupExecuteOrder(moduleManager);
	executerContexts.resize(moduleManager->GetWorkerCount());
}

void GraphicsExecutorModule::Execute(const ExecutionContext& context)
{
	MARK_FUNCTION;
	// TODO: Make sure no allocation happens
	cmdBuffersToExecute.clear();
	for (auto& executerContext : executerContexts)
	{
		cmdBuffersToExecute.push_back_list(executerContext.recordedCmds);
		executerContext.recordedCmds.clear();
	}

	std::sort(cmdBuffersToExecute.begin(), cmdBuffersToExecute.end(), 
		[](const Directx12::CmdBuffer* first, const Directx12::CmdBuffer* second) { return first->index < second->index; });

	auto back = cmdBuffersToExecute.back();
	for (auto cmdBuffer : cmdBuffersToExecute)
	{
		cmdBuffer->queue->Execute((Directx12::CmdBuffer*)cmdBuffer, cmdBuffer == back); // TODO: Fix this const shit
	}

	// TODO: remove once we have double/triple buffering
	if (!cmdBuffersToExecute.empty())
		cmdBuffersToExecute.back()->queue->WaitForBufferIndexToComplete(back->index);
	completedBufferIndex = back->index;
}