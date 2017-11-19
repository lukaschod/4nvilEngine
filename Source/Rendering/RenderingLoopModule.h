#pragma once

#include <Tools\Common.h>
#include <Modules\CmdModule.h>
#include <Graphics\IGraphicsModule.h>

class IRenderingLoopModule : public CmdModule
{
public:
	IRenderingLoopModule(uint32_t bufferCount, uint32_t workersCount);
};