#include <Transforms\TransformsModule.h>
#include <Math\Math.h>

#include <Graphics\GraphicsModule.h>

TransformsModule::TransformsModule()
{
}

void TransformsModule::Execute(uint32_t offset, size_t size)
{
	auto graphicsModule = (GraphicsModule*)dependencies.at(0);
	graphicsModule->RecordPushDebug("Bla");
	graphicsModule->RecordPopDebug();
}
