#pragma once

#include <Tools\Common.h>
#include <Tools\IO\MemoryStream.h>
#include <Modules\Module.h>
#include <Modules\CmdModuleHelper.h>
#include <Modules\CmdBufferPoolModule.h>
#include <map>

class PipeModule : public Module
{
public:
	PipeModule(uint32_t bufferCount, uint32_t workersCount);
	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
	virtual void Execute(const ExecutionContext& context) override;
	virtual size_t GetExecutionSize() override;

protected:
	virtual bool ExecuteCommand(const ExecutionContext& context, MemoryStream& stream, uint32_t commandCode) = 0;
	virtual void OnDependancyAdd(ModuleManager* moduleManager, Module* module, bool executeBefore) override;
	void SortPipes();
	CmdBuffer* GetRecordingBuffer(const ExecutionContext& context);

private:
	struct Pipe
	{
		Pipe(Module* source, size_t size)
			: buffers(size)
			, source(source)
		{
		}
		Module* source;
		List<CmdBuffer> buffers;
	};

	std::map<Module*, Pipe*> pipeMap;
	List<Pipe*> pipes;
	bool isPipesSorted;

	struct CachedCmdBuffer
	{
		CachedCmdBuffer()
			: source(nullptr)
			, currentBuffer(nullptr)
		{
		}
		Module* source;
		CmdBuffer* currentBuffer;
	};
	List<CachedCmdBuffer> cachedCmdBuffers;
};