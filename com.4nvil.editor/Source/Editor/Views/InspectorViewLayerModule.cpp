#include <Core\Rendering\RenderLoop\UnlitRenderLoopModule.hpp>
#include <Editor\Views\InspectorViewLayerModule.hpp>

using namespace Editor;

IRenderLoopModule* InspectorViewLayerModule::GetRenderLoop(ModuleManager* moduleManager)
{
	return ExecuteBefore<UnlitRenderLoopModule>(moduleManager);
}
