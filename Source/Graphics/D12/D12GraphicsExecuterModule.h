#pragma once

#include <Graphics\D12\D12Common.h>
#include <Graphics\D12\D12CmdQueue.h>
#include <Modules\CmdModule.h>

struct D12CmdBuffer;

class D12GraphicsExecuterModule : public Module
{
public:
	D12GraphicsExecuterModule(uint32_t workersCount);
	virtual void Execute(const ExecutionContext& context) override;
	virtual const char* GetName() override { return "D12GraphicsExecuterModule"; }

	void RecCmdBuffer(const ExecutionContext& context, const D12CmdBuffer* buffer);

private:
	struct Context
	{
		List<const D12CmdBuffer*> recordedCmds;
	};
	List<Context> executerContexts;

	List<const D12CmdBuffer*> cmdBuffersToExecute;
	List<ID3D12GraphicsCommandList*> batchedCommandLists;
	AUTOMATED_PROPERTY_GET(uint64_t, completedBufferIndex);
};