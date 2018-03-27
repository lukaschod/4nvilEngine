#include <Core\Rendering\RenderLoop\UnlitRenderLoopModule.h>
#include <Editor\Views\InspectorViewLayerModule.h>

using namespace Editor;

IRenderLoopModule* InspectorViewLayerModule::GetRenderLoop(ModuleManager* moduleManager)
{
	return ExecuteBefore<UnlitRenderLoopModule>(moduleManager);
}
