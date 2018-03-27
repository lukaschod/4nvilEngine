#include <Core\Graphics\IGraphicsModule.hpp>
#include <Core\Foundation\TransformModule.hpp>
#include <Core\Rendering\CameraModule.hpp>
#include <Core\Rendering\StorageModule.hpp>
#include <Core\Rendering\SurfaceModule.hpp>
#include <Core\Rendering\ImageModule.hpp>

using namespace Core;
using namespace Core::Math;

void CameraModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	base::SetupExecuteOrder(moduleManager);
	surfaceModule = ExecuteAfter<SurfaceModule>(moduleManager);
	storageModule = ExecuteBefore<StorageModule>(moduleManager);
	transformModule = ExecuteBefore<TransformModule>(moduleManager);
	unitModule = ExecuteAfter<UnitModule>(moduleManager);
}

void CameraModule::Execute(const ExecutionContext& context)
{
	MARK_FUNCTION;
	base::Execute(context);

	for (auto target : cameras)
	{
		auto transform = unitModule->GetComponent<Transform>(target);
		transformModule->RecCalculateWorldToView(context, transform);
		target->worldToCameraMatrix = transform->worldToView;
		target->worldToCameraMatrix.Multiply(target->projectionMatrix);
		target->cameraToWorldMatrix = Matrix4x4f::Inverted(target->worldToCameraMatrix);
		storageModule->RecUpdateStorage(context, target->perCameraStorage, 0, Range<void>(&target->worldToCameraMatrix, sizeof(Matrix4x4f)));
	}
}

const List<Camera*>& CameraModule::GetCameras() const { return cameras; }

const Camera* CameraModule::AllocateCamera()
{
	auto storage = storageModule->AllocateStorage(sizeof(Matrix4x4f));
	return new Camera(this, storage);
}

Vector3f CameraModule::ScreenToWorld(const Camera* camera, const Vector3f& position)
{
	auto normalizedPosition = ScreenToViewport(camera, position);
	auto worldPosition = camera->cameraToWorldMatrix.TransformPosition(normalizedPosition);
	return Vector3f(worldPosition.x, worldPosition.y, worldPosition.z) / worldPosition.w;
}

Vector3f CameraModule::ScreenToViewport(const Camera* camera, const Vector3f& position)
{
	// Fetch width and height
	auto surface = camera->surface;
	ASSERT(camera->surface != nullptr);
	ASSERT(!camera->surface->colors.empty());
	auto& color = surface->colors[0];
	auto image = color.image;
	ASSERT(image != nullptr);

	auto width = image->width;
	auto height = image->height;

	auto viewPortPosition = Vector3f(
		(position.x / width) * 2 - 1, 
		(1 - (position.y / height)) * 2 - 1, 
		position.z);
	return viewPortPosition;
}

SERIALIZE_METHOD_ARG1(CameraModule, CreateCamera, const Camera*);
SERIALIZE_METHOD_ARG2(CameraModule, SetSurface, const Camera*, const Surface*);
SERIALIZE_METHOD_ARG1(CameraModule, Destroy, const Component*);

bool CameraModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(CreateCamera, Camera*, target);
		target->created = true;
		storageModule->RecCreateStorage(context, target->perCameraStorage);
		cameras.push_back(target);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG2_START(SetSurface, Camera*, target, const Surface*, surface);
		ASSERT(target->created);
		target->surface = surface;

		auto image = target->surface->colors[0].image;
		target->aspect = (float)image->width / image->height;
		target->projectionMatrix = Matrix4x4f::Perspective(target->aspect, Math::DegToRad(target->fieldOfView), target->nearClipPlane, target->farClipPlane); // TODO: sync matrix if surface changes
		DESERIALIZE_METHOD_END;
	}
	return false;
}