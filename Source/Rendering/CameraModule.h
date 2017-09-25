#pragma once

#include <Common\EngineCommon.h>
#include <Modules\UnitModule.h>
#include <Math\Rect.h>
#include <Math\Matrix.h>
#include <Rendering\SurfaceModule.h>
#include <Graphics\IGraphicsModule.h>
#include <Graphics\IRenderPass.h>
#include <Graphics\ITexture.h>
#include <Common\Collections\List.h>

class StorageModule; struct Storage;
class CameraModule;
class TransformModule; struct Transform;

struct Camera : public Component
{
	Camera(CameraModule* module, const Storage* perCameraStorage) : 
		Component((ComponentModule*)module),
		perCameraStorage(perCameraStorage),
		surface(nullptr),
		aspect(1),
		nearClipPlane(0.3f),
		farClipPlane(1000),
		fieldOfView(60)
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
	CameraModule(uint32_t bufferCount, uint32_t workersCount);
	virtual void Execute(const ExecutionContext& context) override;
	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
	const Camera* RecordCreateCamera(const ExecutionContext& context);
	virtual void RecordDestroy(const ExecutionContext& context, const Component* target) override;
	void RecordSetSurface(const ExecutionContext& context, const Camera* camera, const Surface* surface);
	const List<Camera*>& GetCameras();

protected:
	virtual bool ExecuteCommand(const ExecutionContext& context, IOStream& stream, uint32_t commandCode) override;

private:
	List<Camera*> cameras;
	SurfaceModule* surfaceModule;
	StorageModule* storageModule;
	TransformModule* transformModule;
	UnitModule* unitModule;
};