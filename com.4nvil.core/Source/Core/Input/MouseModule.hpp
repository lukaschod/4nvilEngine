#pragma once

#include <Core\Tools\Common.hpp>
#include <Core\Tools\Math\Vector.hpp>
#include <Core\Tools\Enum.hpp>
#include <Core\Foundation\ComputeModule.hpp>
#include <Core\Input\MouseInputs.hpp>

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