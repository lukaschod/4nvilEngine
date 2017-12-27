#pragma once

#include <Tools\Common.h>
#include <Foundation\UnitModule.h>
#include <Tools\Math\Matrix.h>
#include <Tools\Collections\List.h>
#include <queue>

class TransformModule;
class MemoryModule;

struct Transform : public Component
{
	Transform(const TransformModule* module) : 
		Component((ComponentModule*)module), 
		parent(nullptr)
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
};

class TransformModule : public ComponentModule
{
public:
	TransformModule(uint32_t bufferCount, uint32_t workersCount);
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
	virtual bool ExecuteCommand(const ExecutionContext& context, IOStream& stream, uint32_t commandCode) override;

private:
	MemoryModule* memoryModule;
	Transform* root;
	std::queue<Transform*> transformsToCalculate;
	std::vector<Transform*> worldToViewToCalculate;
};