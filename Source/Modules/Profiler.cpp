#include <Modules\Profiler.h>
#include <Tools\Cpu.h>

Profiler::Profiler(uint32_t workersCount)
	: workers(workersCount)
{
	Reset();
	stopWatch.Start();
}

void Profiler::StartFunction(
	uint32_t workerIndex,
	const char* const fileName, 
	const char* const functionName, 
	const uint32_t lineNumber)
{
	auto& worker = workers[workerIndex];

	IProfiler::Function function;
	function.fileName = fileName;
	function.functionName = functionName;
	function.lineNumber = lineNumber;
	function.parent = worker.currentFunction;
	function.startCycle = stopWatch.GetElapsedPicoseconds();

	worker.functions.push_back(function);
	worker.currentFunction = &worker.functions.back();
}

void Profiler::StopFunction(uint32_t workerIndex)
{
	auto& worker = workers[workerIndex];
	ASSERT(worker.currentFunction != nullptr);
	worker.currentFunction->endCycle = stopWatch.GetElapsedPicoseconds();
	worker.currentFunction = worker.currentFunction->parent;
}

void Profiler::Reset()
{
	for (auto& worker : workers)
	{
		worker.currentFunction = nullptr;
		worker.functions.clear();
	}
}
