#include <Modules\PipeModule.h>
#include <Tools\Math\Math.h>

PipeModule::PipeModule() 
	: isPipesSorted(false)
{
}

void PipeModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	profiler = moduleManager->GetProfiler();
	cachedCmdBuffers.resize(moduleManager->GetWorkerCount());
}

void PipeModule::OnDependancyAdd(ModuleManager* moduleManager, Module* module, bool executeBefore)
{
	auto pipe = new Pipe(module, moduleManager->GetWorkerCount());
	pipes.push_back(pipe);
	pipeMap[module] = pipe;
}

void PipeModule::SortPipes()
{
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
	// Lets try firstly to look the cache
	auto& cachedCmdBuffer = cachedCmdBuffers[context.workerIndex];
	if (cachedCmdBuffer.source == context.executingModule)
		return cachedCmdBuffer.currentBuffer;

	auto pipe = pipeMap.find(context.executingModule);
	ASSERT(pipe != pipeMap.end());
	auto buffer = &pipe->second->buffers[context.workerIndex];

	// Update cache
	cachedCmdBuffer.source = context.executingModule;
	cachedCmdBuffer.currentBuffer = buffer;

	return buffer;
}

void PipeModule::Execute(const ExecutionContext& context)
{
	// TODO: Move it to some initialization
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

			for (int i = 0; i < buffer.commandCount; i++)
			{
				auto& commandCode = stream.FastRead<uint32_t>();
				ASSERT(ExecuteCommand(context, stream, commandCode));
			}

			buffer.commandCount = 0;
			buffer.stream.Reset();
		}
	}

	// Clear cache
	for (auto& cachedCmdBuffer : cachedCmdBuffers)
		cachedCmdBuffer.source = nullptr;
}