#pragma once

#include <Editor\Views\ViewLayerModule.h>

namespace Editor
{
	class GameViewLayerModule : public ViewLayerModule
	{
	protected:
		virtual IRenderLoopModule* GetRenderLoop(ModuleManager* moduleManager) override;
	};
}