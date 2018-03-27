#pragma once

#include <Core\Tools\Common.h>
#include <Core\Tools\Math\Vector.h>
#include <Core\Tools\Enum.h>
#include <Core\Foundation\ComputeModule.h>
#include <Core\Input\MouseInputs.h>

namespace Core
{
	class InputModule;
	struct InputDevice;

	class MouseModule : public ComputeModule
	{
	public:
		BASE_IS(ComputeModule);

		MouseModule();
		virtual void Execute(const ExecutionContext& context) override;
		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;

		// Checks if mouse is detected
		bool IsPresent() const { return device != nullptr; }

		// Returns the specific button state
		bool GetButton(MouseButtonType type) const { return buttonsState[Enum::ToUnderlying(type)].click; }

		// Returns the specific button state
		bool GetButtonUp(MouseButtonType type) const { return buttonsState[Enum::ToUnderlying(type)].up; }

		// Returns the specific button state
		bool GetButtonDown(MouseButtonType type) const { return buttonsState[Enum::ToUnderlying(type)].down; }

		// Returns mouse position in screen space
		Math::Vector2f GetPosition() const { return position; }

	private:
		InputModule* inputModule;
		const InputDevice* device;
		MouseButtonState buttonsState[Enum::ToUnderlying(MouseButtonType::Count)];

		Math::Vector2f position;
	};
}