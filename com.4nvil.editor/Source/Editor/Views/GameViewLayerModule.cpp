#include <Editor\Views\GameViewLayerModule.hpp>
#include <Core\Rendering\RenderLoop\UnlitRenderLoopModule.hpp>

using namespace Editor;

IRenderLoopModule* GameViewLayerModule::GetRenderLoop(ModuleManager * moduleManager)
{
	return ExecuteBefore<UnlitRenderLoopModule>(moduleManager);
}
