#include <Editor\Views\GameViewLayerModule.h>
#include <Core\Rendering\RenderLoop\UnlitRenderLoopModule.h>

using namespace Editor;

IRenderLoopModule* GameViewLayerModule::GetRenderLoop(ModuleManager * moduleManager)
{
	return ExecuteBefore<UnlitRenderLoopModule>(moduleManager);
}
