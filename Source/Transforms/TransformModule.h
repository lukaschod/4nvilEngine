#pragma once

#include <Common\EngineCommon.h>
#include <Modules\UnitModule.h>
#include <Math\Matrix.h>
#include <Common\Collections\List.h>
#include <queue>

class TransformModule;

struct Transform : public Component
{
	Transform(TransformModule* module) : 
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

	const Transform* RecCreateTransform(const ExecutionContext& context);
	virtual void RecDestroy(const ExecutionContext& context, const Component* unit) override;
	void RecSetParent(const ExecutionContext& context, const Transform* target, const Transform* parent);
	void RecSetPosition(const ExecutionContext& context, const Transform* target, const Vector3f& position);
	void RecCalculateWorldToView(const ExecutionContext& context, const Transform* target);

protected:
	virtual bool ExecuteCommand(const ExecutionContext& context, IOStream& stream, uint32_t commandCode) override;

private:
	Transform* root;
	std::queue<Transform*> transformsToCalculate;
	std::vector<Transform*> worldToViewToCalculate;
};