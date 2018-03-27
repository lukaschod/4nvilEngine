#pragma once

#include <map>
#include <Core\Tools\Common.h>
#include <Core\Tools\IO\MemoryStream.h>
#include <Core\Modules\Module.h>
#include <Core\Foundation\ProfilerModule.h>
#include <Core\Foundation\PipeModuleHelper.h>

namespace Core
{
	typedef uint32 CommandCode;
	typedef IO::MemoryStream CommandStream;

	struct CmdBuffer
	{
		CmdBuffer() : commandCount(0) {}

		// Recorded commands with all its data
		CommandStream stream;

		// Total count of commands recorded in this buffer
		size_t commandCount;

		// Index of the cmdbuffer
		uint64 index;

		// On which worker the cmdbuffer was created
		uint32 workerIndex;

		// Module that records this buffer
		Module* executingModule;
	};

	class PipeModule : public Module
	{
	public:
		BASE_IS(Module);

		PipeModule();
		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
		virtual void Execute(const ExecutionContext& context) override;

	protected:
		// This is where each PipeModule implementation will add its commands
		virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) = 0;
		virtual void OnDependancyAdd(ModuleManager* moduleManager, Module* module, bool executeBefore) override;

		// Sort the pipes according the order of Module execution, deducted from the dependancy tree
		void SortPipes();

		// Returns buffer that should be used for recording the commands
		CmdBuffer* GetRecordingBuffer(const ExecutionContext& context);

	private:
		// Pipe is simple one directiona connection between Modules
		struct Pipe
		{
			Pipe(Module* source, size_t size) : buffers(size), source(source) {}
			Module* source;
			List<CmdBuffer> buffers;
		};

		std::map<Module*, Pipe*> pipeMap; // Map between the Module and Pipe
		List<Pipe*> pipes; // All the Pipes that belong to this Module
		bool isPipesSorted;

		struct CachedCmdBuffer
		{
			CachedCmdBuffer() : source(nullptr), currentBuffer(nullptr) {}
			Module* source;
			CmdBuffer* currentBuffer;
		};
		List<CachedCmdBuffer> cachedCmdBuffers;

	protected:
		ProfilerModule* profilerModule;
	};
}