#include <Core\Input\InputModule.hpp>

using namespace Core;

void InputModule::Execute(const ExecutionContext& context)
{
	MARK_FUNCTION;
	Reset();
	base::Execute(context);
}

const InputDevice* InputModule::TryFindInputDevice(const char* typeName) const
{
	for (auto device : devices)
		if (strcmp(device->desc.typeName, typeName) == 0)
			return device;
	return nullptr;
}

DECLARE_COMMAND_CODE(Input);
void InputModule::RecInput(const ExecutionContext& context, const InputDevice* device, InputType inputType, uint8* data, size_t size)
{
	auto buffer = GetRecordingBuffer(context);
	auto& stream = buffer->stream;
	stream.Write(TO_COMMAND_CODE(Input));
	stream.Write(device);
	stream.Write(inputType);
	stream.Write(size);
	if (size != 0)
		stream.Write(data, size);
	stream.Align();
	buffer->commandCount++;
}

DECLARE_COMMAND_CODE(CreateInputDevice);
const InputDevice* InputModule::RecCreateInputDevice(const ExecutionContext& context, const InputDeviceDesc& desc)
{
	auto buffer = GetRecordingBuffer(context);
	auto& stream = buffer->stream;
	stream.Write(TO_COMMAND_CODE(CreateInputDevice));
	auto device = new InputDevice();
	device->desc = desc;
	stream.Write(device);
	stream.Align();
	buffer->commandCount++;
	return device;
}

bool InputModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_START(Input);
		auto device = stream.FastRead<InputDevice*>();
		auto inputType = stream.FastRead<InputType>();
		auto size = stream.FastRead<size_t>();
		auto pointer = stream.Get_data();
		stream.Set_data(pointer + size);
		device->inputStream.Write(inputType);
		if (size != 0)
			device->inputStream.Write(pointer, size);
		device->inputCount++;
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG1_START(CreateInputDevice, InputDevice*, device);
		devices.push_back(device);
		DESERIALIZE_METHOD_END;
	}
	return false;
}

void InputModule::Reset()
{
	for (auto device : devices)
	{
		device->inputStream.Reset();
		device->inputCount = 0;
	}
}