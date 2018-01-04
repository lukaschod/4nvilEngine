#pragma once

#include <Tools\Common.h>
#include <Tools\IO\FileStream.h>
#include <Modules\PipeModule.h>

class LogModule : public PipeModule
{
public:
	virtual void Execute(const ExecutionContext& context) override;
	void RecWrite(const ExecutionContext& context, const char* name);
	void RecWriteFmt(const ExecutionContext& context, const char* format, ...);

private:
	void OpenStream();
	void CloseStream();

protected:
	virtual bool ExecuteCommand(const ExecutionContext& context, MemoryStream& stream, uint32_t commandCode) override;

private:
	FileStream output;
};