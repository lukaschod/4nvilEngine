#pragma once

#include <Tools\Common.h>
#include <Tools\Math\Vector.h>
#include <Tools\Enum.h>
#include <Foundation\PipeModule.h>
#include <Input\MouseInputs.h>

namespace Core
{
	class InputModule;

	class MouseModule : public PipeModule
	{
	public:
		virtual void Execute(const ExecutionContext& context) override { MARK_FUNCTION; base::Execute(context); }
		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;

		// Checks if mouse is detected
		bool IsPresent();

		// Returns the specific button state
		MouseButtonState GetButtonState(MouseButtonType type);

		// Returns mouse position in screen space
		Math::Vector2f GetPosition();

	protected:
		virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

	private:
		InputModule* inputModule;
		MouseButtonState buttonState[Enum::ToUnderlying(MouseButtonType::Count)];
	};
}