#pragma once

#include <Tools\Common.h>
#include <Foundation\PipeModule.h>
#include <Tools\IO\MemoryStream.h>
#include <Tools\Collections\List.h>

namespace Core
{
	typedef uint32_t InputType;

	struct InputDeviceDesc
	{
		const char* typeName;
		const char* vendorName;
	};

	struct InputDevice
	{
		InputDeviceDesc desc;
		IO::MemoryStream inputStream;
		uint32 inputCount;
	};

	class InputModule : public PipeModule
	{
	public:
		virtual void Execute(const ExecutionContext& context) override;

		// Find input devices by the type name
		const InputDevice* TryFindInputDevice(const char* typeName) const;

		// Record abstract input
		void RecInput(const ExecutionContext& context, const InputDevice* device, InputType inputType, uint8* data, size_t size);

		// Create new input device, type name must be unique
		const InputDevice* RecCreateInputDevice(const ExecutionContext& context, const InputDeviceDesc& desc);

	private:
		// Remove all previous inputs
		void Reset();

	protected:
		virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

	private:
		List<InputDevice*> devices;
	};
}