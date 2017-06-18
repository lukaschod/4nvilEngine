#pragma once

#include <Common\EngineCommon.h>
#include <Common\IOStream.h>
#include <Modules\CmdModule.h>
#include <queue>

enum GraphicsCommandCode
{
	kGraphicsCommandCodePushDebug,
	kGraphicsCommandCodePopDebug,
};

class GraphicsModule : public CmdModule
{
public:
	GraphicsModule(uint32_t bufferCount);
	void RecordPushDebug(const char* name);
	void RecordPopDebug();

protected:
	virtual void ExecuteCommand(IOStream& stream, uint32_t commandCode) override;
};