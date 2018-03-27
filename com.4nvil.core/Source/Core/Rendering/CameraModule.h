#pragma once

#include <Core\Tools\Common.h>
#include <Core\Tools\Math\Matrix.h>
#include <Core\Tools\Collections\List.h>
#include <Core\Foundation\UnitModule.h>

namespace Core
{
	struct Transform;
	struct Surface;
	struct Storage;
	class TransformModule;
	class SurfaceModule;
	class StorageModule;
}

namespace Core
{
	struct Camera : public Component
	{
		Camera(ComponentModule* module, const Storage* perCameraStorage)
			: Component(module)
			, perCameraStorage(perCameraStorage)
			, surface(nullptr)
			, aspect(1)
			, nearClipPlane(0.3f)
			, farClipPlane(1000)
			, fieldOfView(60)
			, created(false)
		{
		}

		const Surface* surface;
		const Storage* perCameraStorage;
		float nearClipPlane, farClipPlane;
		float aspect;
		float fieldOfView;

		Math::Matrix4x4f projectionMatrix;
		Math::Matrix4x4f worldToCameraMatrix;
		Math::Matrix4x4f cameraToWorldMatrix;

		bool created;
	};

	class CameraModule : public ComponentModule
	{
	public:
		BASE_IS(ComponentModule);

		virtual void Execute(const ExecutionContext& context) override;
		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;

		// Returns all cameras
		const List<Camera*>& GetCameras() const;

		// Allocates memory for camera, creation is still needed
		const Camera* AllocateCamera();

		// Convert screen space position into world space position
		Math::Vector3f ScreenToWorld(const Camera* camera, const Math::Vector3f& position);

		// Convert screen space position into viewport space position
		Math::Vector3f ScreenToViewport(const Camera* camera, const Math::Vector3f& position);

	public:
		void RecCreateCamera(const ExecutionContext& context, const Camera* target);
		virtual void RecDestroy(const ExecutionContext& context, const Component* target) override;
		void RecSetSurface(const ExecutionContext& context, const Camera* camera, const Surface* surface);

	protected:
		virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

	private:
		List<Camera*> cameras;
		SurfaceModule* surfaceModule;
		StorageModule* storageModule;
		TransformModule* transformModule;
		UnitModule* unitModule;
	};
}