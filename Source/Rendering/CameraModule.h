#pragma once

#include <Tools\Common.h>
#include <Foundation\UnitModule.h>
#include <Tools\Math\Rect.h>
#include <Tools\Math\Matrix.h>
#include <Rendering\SurfaceModule.h>
#include <Graphics\IGraphicsModule.h>
#include <Graphics\IRenderPass.h>
#include <Graphics\ITexture.h>
#include <Tools\Collections\List.h>

class StorageModule; struct Storage;
class CameraModule;
class TransformModule; struct Transform;

struct Camera : public Component
{
	Camera(const CameraModule* module, const Storage* perCameraStorage)
		:  Component((ComponentModule*)module)
		, perCameraStorage(perCameraStorage)
		, surface(nullptr)
		, aspect(1)
		, nearClipPlane(0.3f)
		, farClipPlane(1000)
		, fieldOfView(60)
	{}

	const Surface* surface;
	const Storage* perCameraStorage;
	float nearClipPlane, farClipPlane;
	float aspect;
	float fieldOfView;

	Matrix4x4f projectionMatrix;
	Matrix4x4f worldToCameraMatrix;
};

class CameraModule : public ComponentModule
{
public:
	virtual void Execute(const ExecutionContext& context) override;
	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
	const List<Camera*>& GetCameras() const;
	const Camera* AllocateCamera() const;

public:
	const Camera* RecCreateCamera(const ExecutionContext& context, const Camera* camera = nullptr);
	virtual void RecDestroy(const ExecutionContext& context, const Component* target) override;
	void RecSetSurface(const ExecutionContext& context, const Camera* camera, const Surface* surface);

protected:
	virtual bool ExecuteCommand(const ExecutionContext& context, MemoryStream& stream, uint32_t commandCode) override;

private:
	List<Camera*> cameras;
	SurfaceModule* surfaceModule;
	StorageModule* storageModule;
	TransformModule* transformModule;
	UnitModule* unitModule;
};