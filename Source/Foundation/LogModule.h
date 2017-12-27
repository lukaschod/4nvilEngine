#pragma once

#include <Tools\Common.h>
#include <Modules\CmdModule.h>

class LogModule : public CmdModule
{
public:
	LogModule(uint32_t bufferCount, uint32_t workersCount);
	void RecMessage(const ExecutionContext& context, const char* name);
	void RecMessageF(const ExecutionContext& context, const char* format, ...);

protected:
	virtual bool ExecuteCommand(const ExecutionContext& context, MemoryStream& stream, uint32_t commandCode) override;
};