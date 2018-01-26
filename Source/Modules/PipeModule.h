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
	PipeModule();
	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
	virtual void Execute(const ExecutionContext& context) override;

protected:
	// This is where each PipeModule implementation will add its commands
	virtual bool ExecuteCommand(const ExecutionContext& context, MemoryStream& stream, uint32_t commandCode) = 0;
	virtual void OnDependancyAdd(ModuleManager* moduleManager, Module* module, bool executeBefore) override;

	// Sort the pipes according the order of Module execution, deducted from the dependancy tree
	void SortPipes();

	// Returns buffer that should be used for recording the commands
	CmdBuffer* GetRecordingBuffer(const ExecutionContext& context);

private:
	// Pipe is simple one directiona connection between Modules
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

	std::map<Module*, Pipe*> pipeMap; // Map between the Module and Pipe
	List<Pipe*> pipes; // All the Pipes that belong to this Module
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