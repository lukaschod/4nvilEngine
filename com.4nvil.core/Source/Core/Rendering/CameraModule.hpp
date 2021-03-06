/*
* Copyright (c) Lukas Chodosevicius
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#pragma once

#include <Core/Tools/Common.hpp>
#include <Core/Tools/Math/Matrix.hpp>
#include <Core/Tools/Collections/List.hpp>
#include <Core/Foundation/UnitModule.hpp>

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
    struct Camera : Component
    {
        IMPLEMENT_TRANSFERABLE(Core, Camera);

        Camera(const Storage* perCameraStorage)
            : perCameraStorage(perCameraStorage)
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
        Float nearClipPlane, farClipPlane;
        Float aspect;
        Float fieldOfView;

        Math::Matrix4x4f projectionMatrix;
        Math::Matrix4x4f worldToCameraMatrix;
        Math::Matrix4x4f cameraToWorldMatrix;

        Bool created;
    };

    class CameraModule : public ComponentModule
    {
    public:
        IMPLEMENT_TRANSFERER(Core, Camera);
        BASE_IS(ComponentModule);

        CORE_API virtual Void Execute(const ExecutionContext& context) override;
        CORE_API virtual Void SetupExecuteOrder(ModuleManager* moduleManager) override;

        // Allocates memory for camera, creation is still needed
        CORE_API const Camera* AllocateCamera();

        // Convert screen space position into world space position
        CORE_API Math::Vector3f ScreenToWorld(const Camera* camera, const Math::Vector3f& position);

        // Convert screen space position into viewport space position
        CORE_API Math::Vector3f ScreenToViewport(const Camera* camera, const Math::Vector3f& position);

        // Returns all cameras
        const List<Camera*>& GetCameras() const { return cameras; }

    public:
        CORE_API Void RecCreateCamera(const ExecutionContext& context, const Camera* target);
        CORE_API virtual Void RecDestroy(const ExecutionContext& context, const Component* target) override;
        CORE_API Void RecSetSurface(const ExecutionContext& context, const Camera* camera, const Surface* surface);

    protected:
        CORE_API virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        List<Camera*> cameras;
        SurfaceModule* surfaceModule;
        StorageModule* storageModule;
        TransformModule* transformModule;
    };
}