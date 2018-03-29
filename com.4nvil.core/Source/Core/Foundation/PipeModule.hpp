/*
* Copyright (c) Lukas Chodosevicius
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#pragma once

#include <map>
#include <Core\Tools\Common.hpp>
#include <Core\Tools\IO\MemoryStream.hpp>
#include <Core\Modules\Module.hpp>
#include <Core\Foundation\ProfilerModule.hpp>
#include <Core\Foundation\PipeModuleHelper.hpp>

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