#pragma once

#include <Tools\Common.h>
#include <Foundation\PipeModule.h>
#include <Tools\IO\MemoryStream.h>

namespace Core
{
	typedef uint64 InputCommandCode;

	class InputModule : public PipeModule
	{
	public:
		virtual void Execute(const ExecutionContext& context) override;
		IO::MemoryStream& GetInputStream() { return inputStream; }

		// Record abstract input
		void RecInput(const ExecutionContext& context, InputCommandCode commandCode, uint8* data, size_t size);

	private:
		// Remove all previous inputs
		void Reset();

	protected:
		virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

	private:
		IO::MemoryStream inputStream;
	};
}