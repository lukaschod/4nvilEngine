#pragma once

#include <Tools\Common.h>
#include <Tools\IO\MemoryStream.h>
#include <Modules\Module.h>
#include <queue>
#include <mutex>

enum CmdBufferState
{
	CmdBufferStateNotUsed,
	CmdBufferStateUsed,
	CmdBufferStateExecuted,
};

struct CmdBuffer
{
	CmdBuffer() : 
		commandCount(0),
		state(CmdBufferStateNotUsed)
	{}

	/// Reced commands buffer
	MemoryStream stream;

	/// Reced commandcount
	size_t commandCount;

	/// Index of the cmdbuffer
	uint64_t index;

	/// On which worker the cmdbuffer was created
	uint32_t workerIndex;

	/// Module that records this buffer
	Module* executingModule;

	CmdBufferState state;
};

/*class CmdBufferPoolModule : public Module
{
public:
	CmdBufferPoolModule(uint32_t workerCount);
	~CmdBufferPoolModule();
	virtual const char* GetName() { return "CmdBufferPoolModule"; }
	virtual void Execute(const ExecutionContext& context) override;

	CmdBuffer* Pull(const ExecutionContext& context, uint64_t index);

private:
	void Reset(CmdBuffer* buffer, uint64_t index, const ExecutionContext& context);

private:
	struct Context
	{
		std::vector<CmdBuffer*> cmdBuffers;
		std::queue<CmdBuffer*> readyCmdBuffers;
	};

	Context* contexts;
	uint32_t workerCount;
};*/