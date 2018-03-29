/*
* Copyright (c) Lukas Chodosevicius
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#pragma once

#include <Core\Tools\Common.hpp>
#include <Core\Foundation\PipeModule.hpp>
#include <Core\Tools\IO\MemoryStream.hpp>
#include <Core\Tools\Collections\List.hpp>

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
		BASE_IS(PipeModule);

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