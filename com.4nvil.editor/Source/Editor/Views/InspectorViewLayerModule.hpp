#pragma once

#include <Editor\Views\ViewLayerModule.hpp>

namespace Editor
{
	class InspectorViewLayerModule : public ViewLayerModule
	{
	public:
		BASE_IS(ViewLayerModule);

	protected:
		virtual IRenderLoopModule* GetRenderLoop(ModuleManager* moduleManager) override;
	};
}