#include <Rendering\CameraModule.h>
#include <Rendering\StorageModule.h>
#include <Graphics\IGraphicsModule.h>
#include <Transforms\TransformModule.h>

CameraModule::CameraModule(uint32_t bufferCount, uint32_t bufferIndexStep) : 
	ComponentModule(bufferCount, bufferIndexStep)
{
}

void CameraModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	CmdModule::SetupExecuteOrder(moduleManager);
	surfaceModule = ExecuteAfter<SurfaceModule>(moduleManager);
	storageModule = ExecuteBefore<StorageModule>(moduleManager);
	transformModule = ExecuteBefore<TransformModule>(moduleManager);
	unitModule = ExecuteAfter<UnitModule>(moduleManager);
}

void CameraModule::Execute(const ExecutionContext& context)
{
	CmdModule::Execute(context);

	for (auto target : cameras)
	{
		auto transform = unitModule->GetComponent<Transform>(target);
		transformModule->RecCalculateWorldToView(context, transform);
		target->worldToCameraMatrix = transform->worldToView;
		target->worldToCameraMatrix.Multiply(target->projectionMatrix);
		target->worldToCameraMatrix = Matrix4x4f::Transpose(target->worldToCameraMatrix);
		storageModule->RecUpdateStorage(context, target->perCameraStorage, 0, Range<void>(&target->worldToCameraMatrix, sizeof(Matrix4x4f)));
	}
}

const List<Camera*>& CameraModule::GetCameras() { return cameras; }

DECLARE_COMMAND_CODE(CreateCamera);
const Camera* CameraModule::RecCreateCamera(const ExecutionContext& context)
{
	auto buffer = GetRecordingBuffer(context);
	auto& stream = buffer->stream;
	auto storage = storageModule->RecCreateStorage(context, sizeof(Matrix4x4f));
	auto target = new Camera(this, storage);
	stream.Write(kCommandCodeCreateCamera);
	stream.Write(target);
	buffer->commandCount++;
	return target;
}

SERIALIZE_METHOD_ARG2(CameraModule, SetSurface, const Camera*, const Surface*);
SERIALIZE_METHOD_ARG1(CameraModule, Destroy, const Component*);

bool CameraModule::ExecuteCommand(const ExecutionContext& context, IOStream& stream, uint32_t commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(CreateCamera, Camera*, target);
		cameras.push_back(target);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG2_START(SetSurface, Camera*, target, const Surface*, surface);
		target->surface = surface;

		auto image = target->surface->colors[0].image;
		target->aspect = (float)image->width / image->height;
		target->projectionMatrix = Matrix4x4f::Perspective(target->aspect, Math::DegToRad(target->fieldOfView), target->nearClipPlane, target->farClipPlane);
		DESERIALIZE_METHOD_END;
	}
	return false;
}
