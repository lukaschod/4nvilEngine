#pragma once

#include <Core\Tools\Common.hpp>
#include <Core\Tools\IO\FileStream.hpp>
#include <Core\Foundation\PipeModule.hpp>

namespace Core
{
	class LogModule : public PipeModule
	{
	public:
		BASE_IS(PipeModule);

		virtual void Execute(const ExecutionContext& context) override;
		void RecWrite(const ExecutionContext& context, const char* name);
		void RecWriteFmt(const ExecutionContext& context, const char* format, ...);

	private:
		void OpenStream();
		void CloseStream();

	protected:
		virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

	private:
		IO::FileStream output;
	};
}