#pragma once

#include <Common\EngineCommon.h>
#include <Modules\CmdModule.h>

class LogModule : public CmdModule
{
public:
	LogModule(uint32_t bufferCount, uint32_t workersCount);
	void RecordMessage(const ExecutionContext& context, const char* name);
	void RecordMessageF(const ExecutionContext& context, const char* format, ...);

protected:
	virtual bool ExecuteCommand(const ExecutionContext& context, IOStream& stream, uint32_t commandCode) override;
};