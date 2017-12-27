#pragma once

#include <Tools\Common.h>
#include <Tools\IOStream.h>
#include <Modules\Module.h>
#include <Modules\CmdModuleHelper.h>
#include <Modules\CmdBufferPoolModule.h>

// TODO: Remove it
#include <Modules\PipeModule.h>
typedef PipeModule CmdModule;
/*class CmdModule : public Module
{
public:
	CmdModule(uint32_t bufferCount, uint32_t workersCount);
	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
	virtual void Execute(const ExecutionContext& context) override;
	virtual size_t GetExecutionSize() override;

protected:
	virtual bool ExecuteCommand(const ExecutionContext& context, IOStream& stream, uint32_t commandCode) = 0;

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