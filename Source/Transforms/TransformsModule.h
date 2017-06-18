#pragma once

#include <Common\EngineCommon.h>
#include <Common\IOStream.h>
#include <Modules\CmdModule.h>
#include <queue>

class TransformsModule : public Module
{
public:
	TransformsModule();
	virtual void Execute(uint32_t offset, size_t size) override;
	virtual size_t GetExecutionkSize() override { return 10; };
	virtual size_t GetSplitExecutionTreshold() override { return 5; };
};