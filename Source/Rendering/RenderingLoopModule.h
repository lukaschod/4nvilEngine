#pragma once

#include <Common\EngineCommon.h>
#include <Modules\CmdModule.h>
#include <Graphics\IGraphicsModule.h>

class IRenderingLoopModule : public CmdModule
{
public:
	IRenderingLoopModule(uint32_t bufferCount, uint32_t workersCount);
};