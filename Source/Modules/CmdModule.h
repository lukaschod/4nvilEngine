#pragma once

// TODO: Remove
/*#include <Tools\Common.h>
#include <Tools\IO\MemoryStream.h>
#include <Modules\Module.h>
#include <Modules\CmdModuleHelper.h>
#include <Modules\CmdBufferPoolModule.h>

class CmdModule : public Module
{
public:
	CmdModule(uint32_t bufferCount, uint32_t workersCount);
	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
	virtual void Execute(const ExecutionContext& context) override;
	virtual size_t GetExecutionSize() override;

protected:
	virtual bool ExecuteCommand(const ExecutionContext& context, MemoryStream& stream, uint32_t commandCode) = 0;

	CmdBuffer* GetRecordingBuffer(const ExecutionContext& context);

private:
	struct Context
	{
		List<CmdBuffer*> buffers;
	};

	List<Context> contexts;
	uint32_t backbufferIndex;
	List<CmdBuffer*> cmdBuffersToExecute;

	CmdBufferPoolModule* cmdBufferPoolModule;

	uint64_t bufferCounter;
};*/