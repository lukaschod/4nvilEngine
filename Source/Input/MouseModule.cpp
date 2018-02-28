#include <Input\MouseModule.h>
#include <Input\InputModule.h>

using namespace Core;

MouseModule::MouseModule() 
	: device(nullptr)
{
}

void MouseModule::Execute(const ExecutionContext& context)
{
	MARK_FUNCTION; 

	device = inputModule->TryFindInputDevice("Mouse");

	// If device is still not available simply skip everything
	if (device == nullptr)
		return;

	// Examine all inputs and update the state of mouse
	auto& stream = device->inputStream;
	auto inputCount = device->inputCount;
	size_t offset = 0;
	for (uint32 i = 0; i < inputCount; i++)
	{
		auto inputType = stream.FastRead<MouseInputType>(offset);
		switch (inputType)
		{

		case MouseInputType::Move:
		{
			auto& desc = stream.FastRead<MousePositionDesc>(offset);
			position = desc.position;
			break;
		}

		case MouseInputType::Button:
		{
			auto& desc = stream.FastRead<MouseButtonDesc>(offset);
			auto& state = buttonState[Enum::ToUnderlying(desc.type)];
			auto isDown = desc.isDown;
			if (state == MouseButtonState::None && isDown)
			{
				state = MouseButtonState::Down;
				break;
			}

			if (state == MouseButtonState::Down && isDown)
			{
				state = MouseButtonState::Click;
				break;
			}

			if (state == MouseButtonState::Click && !isDown)
			{
				state = MouseButtonState::Up;
				break;
			}

			if (state == MouseButtonState::Up && !isDown)
			{
				state = MouseButtonState::None;
				break;
			}

			ERROR("Invalid button state");
			break;
		}

		default:
			ERROR("Unknown input type");
		}
	}
}

void MouseModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	base::SetupExecuteOrder(moduleManager);
	inputModule = ExecuteAfter<InputModule>(moduleManager);
}
