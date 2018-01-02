#pragma once

#include <Tools\Common.h>
#include <Tools\IO\FileStream.h>
#include <Modules\CmdModule.h>

class LogModule : public CmdModule
{
public:
	LogModule(uint32_t bufferCount, uint32_t workersCount);
	virtual void Execute(const ExecutionContext& context) override;
	void RecMessage(const ExecutionContext& context, const char* name);
	void RecMessageF(const ExecutionContext& context, const char* format, ...);

private:
	void OpenStream();
	void CloseStream();

protected:
	virtual bool ExecuteCommand(const ExecutionContext& context, MemoryStream& stream, uint32_t commandCode) override;

private:
	FileStream output;
};