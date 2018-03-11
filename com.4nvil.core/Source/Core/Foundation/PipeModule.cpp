#include <algorithm>
#include <Core\Tools\Math\Math.h>
#include <Core\Foundation\PipeModule.h>
#include <Core\Foundation\ProfilerModule.h>

using namespace Core;

PipeModule::PipeModule() : isPipesSorted(false) {}

void PipeModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	profilerModule = ExecuteAfter<ProfilerModule>(moduleManager);
	cachedCmdBuffers.resize(moduleManager->GetWorkerCount());
}

void PipeModule::OnDependancyAdd(ModuleManager* moduleManager, Module* module, bool executeBefore)
{
	ASSERT(pipeMap.find(module) == pipeMap.end());
	// Once dependancy is added, we should create corresponding Pipe for it, where all communication will be stored
	auto pipe = new Pipe(module, moduleManager->GetWorkerCount());
	pipes.push_back(pipe);
	pipeMap[module] = pipe;
}

void PipeModule::SortPipes()
{
	// Sort the pipies according the dependencies
	std::function<bool(Module*, Module*)> recusrive = [&recusrive](Module* first, Module* second)
	{
		if (first == second)
			return false;

		for (auto module : second->Get_dependencies())
			if (!recusrive(first, module))
				return false;

		return true;
	};

	std::sort(pipes.begin(), pipes.end(), 
		[&recusrive](Pipe* first, Pipe* second) { return !recusrive(first->source, second->source); });
	isPipesSorted = true;
}

CmdBuffer* PipeModule::GetRecordingBuffer(const ExecutionContext& context)
{
	// As long as for the worker executing Module doesn't change, we can actaully cache Pipe for faster access
	auto& cachedCmdBuffer = cachedCmdBuffers[context.workerIndex];
	if (cachedCmdBuffer.source == context.executingModule)
		return cachedCmdBuffer.currentBuffer;

	// If cache miss happens, we should find the Pipe corresponding to the executing Module
	auto pipe = pipeMap.find(context.executingModule);
	ASSERT(pipe != pipeMap.end());
	auto buffer = &pipe->second->buffers[context.workerIndex];

	// Update cache with new executing module
	cachedCmdBuffer.source = context.executingModule;
	cachedCmdBuffer.currentBuffer = buffer;

	return buffer;
}

void PipeModule::Execute(const ExecutionContext& context)
{
	// TODO: Move it to some initialization
	// We can sort the pipes only once, as dependencies between the Modules are static
	// NOTE: Of course branch prediction migh eleminate this performance cost
	if (!isPipesSorted)
		SortPipes();

	for (auto pipe : pipes)
	{
		for (auto& buffer : pipe->buffers)
		{
			// TODO: Investigate is it worth creating buffer to execute
			if (buffer.commandCount == 0)
				continue;

			// Reset buffer for executing
			auto& stream = buffer.stream;
			buffer.stream.Reset();
			buffer.executingModule = context.executingModule;

			// Execute each command one by one
			for (int i = 0; i < buffer.commandCount; i++)
			{
				auto& commandCode = stream.FastRead<CommandCode>();
				ASSERT(ExecuteCommand(context, stream, commandCode));
				stream.Align();
			}

			// Prepare buffer for next frame recording
			buffer.commandCount = 0;
			buffer.stream.Reset();
		}
	}

	// Clear cache
	for (auto& cachedCmdBuffer : cachedCmdBuffers)
		cachedCmdBuffer.source = nullptr;
}