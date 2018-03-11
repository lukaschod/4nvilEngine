#pragma once

#include <queue>
#include <Core\Tools\Common.h>
#include <Core\Tools\Flags.h>
#include <Core\Tools\Math\Matrix.h>
#include <Core\Tools\Collections\List.h>
#include <Core\Foundation\UnitModule.h>

namespace Core
{
	class MemoryModule;
}

namespace Core
{
	enum class TransformStateFlags
	{
		None = 0,
		LocalObjectToWorldChanged = 1 << 0,
		LocalObjectToWorldUnsetNextFrame = 1 << 1,
	};

	struct Transform : public Component
	{
		Transform(ComponentModule* module)
			: Component(module)
			, parent(nullptr)
		{
		}

		List<Transform*> childs;
		Transform* parent;
		Math::Matrix4x4f objectToWorld;
		Math::Matrix4x4f worldToView;
		Math::Matrix4x4f localObjectToWorld;
		Math::Vector3f localPosition;
		Math::Quaternionf localRotation;
		Math::Vector3f localScale;
		Math::Vector3f position;
		Flags<TransformStateFlags> flags;
	};

	class TransformModule : public ComponentModule
	{
	public:
		TransformModule();
		virtual void Execute(const ExecutionContext& context) override;
		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
		const Transform* AllocateTransform();

	public:
		const Transform* RecCreateTransform(const ExecutionContext& context, const Transform* transform = nullptr);
		virtual void RecDestroy(const ExecutionContext& context, const Component* unit) override;
		void RecSetParent(const ExecutionContext& context, const Transform* target, const Transform* parent);
		void RecSetPosition(const ExecutionContext& context, const Transform* target, const Math::Vector3f& position);
		void RecAddPosition(const ExecutionContext& context, const Transform* target, const Math::Vector3f& position);
		void RecSetRotation(const ExecutionContext& context, const Transform* target, const Math::Vector3f& rotation);
		void RecCalculateWorldToView(const ExecutionContext& context, const Transform* target);

	protected:
		virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

	private:
		MemoryModule* memoryModule;
		Transform* root;
		std::queue<Transform*> transformsToCalculate;
		std::vector<Transform*> worldToViewToCalculate;
	};
}