#include <Editor\Views\InspectorViewLayerModule.h>
#include <Core\Rendering\RenderLoop\UnlitRenderLoopModule.h>

using namespace Editor;

IRenderLoopModule* InspectorViewLayerModule::GetRenderLoop(ModuleManager* moduleManager)
{
	return ExecuteBefore<UnlitRenderLoopModule>(moduleManager);
}
