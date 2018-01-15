#pragma once

#include <Tools\Common.h>
#include <Tools\Flags.h>
#include <Tools\Math\Matrix.h>
#include <Tools\Collections\List.h>
#include <Foundation\UnitModule.h>
#include <queue>

class TransformModule;
class MemoryModule;

enum TransformStateFlags
{
	TransformFlagsLocalObjectToWorldChanged = 1 << 0,
	TransformFlagsLocalObjectToWorldUnsetNextFrame = 1 << 1,
};

struct Transform : public Component
{
	Transform(const TransformModule* module) 
		: Component((ComponentModule*)module)
		, parent(nullptr)
		, dirtyLocalObjectToWorldMatrix(true)
	{ }

	List<Transform*> childs;
	Transform* parent;
	Matrix4x4f objectToWorld;
	Matrix4x4f worldToView;
	Matrix4x4f localObjectToWorld;
	Vector3f localPosition;
	Quaternionf localRotation;
	Vector3f localScale;
	Vector3f position;
	bool dirtyLocalObjectToWorldMatrix;
	Flags<TransformStateFlags> flags;
};

class TransformModule : public ComponentModule
{
public:
	TransformModule();
	virtual void Execute(const ExecutionContext& context) override;
	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
	const Transform* AllocateTransform() const;

public:
	const Transform* RecCreateTransform(const ExecutionContext& context, const Transform* transform = nullptr);
	virtual void RecDestroy(const ExecutionContext& context, const Component* unit) override;
	void RecSetParent(const ExecutionContext& context, const Transform* target, const Transform* parent);
	void RecSetPosition(const ExecutionContext& context, const Transform* target, const Vector3f& position);
	void RecCalculateWorldToView(const ExecutionContext& context, const Transform* target);

protected:
	virtual bool ExecuteCommand(const ExecutionContext& context, MemoryStream& stream, uint32_t commandCode) override;

private:
	MemoryModule* memoryModule;
	Transform* root;
	std::queue<Transform*> transformsToCalculate;
	std::vector<Transform*> worldToViewToCalculate;
};