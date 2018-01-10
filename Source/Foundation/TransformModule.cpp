#include <Foundation\TransformModule.h>
#include <Foundation\MemoryModule.h>
#include <Tools\Collections\FixedBlockHeap.h>
#include <Tools\Math\Math.h>

TransformModule::TransformModule() 
{
	root = new Transform(this);
	root->localPosition = Vector3f(0, 0, 0);
	root->localRotation = Quaternionf(0, 0, 0, 1);
	root->localScale = Vector3f(1, 1, 1);
	root->objectToWorld = Matrix4x4f::TRS(root->localPosition, root->localRotation, root->localScale);
}

void TransformModule::Execute(const ExecutionContext& context)
{
	PROFILE_FUNCTION;
	PipeModule::Execute(context);

	// Do BFS to re-calculate new transformations
	// TODO: Find performance BFS vs DFS, but my guess BFS better because cache might be used much more better
	for (auto child : root->childs)
		transformsToCalculate.push(child);
	while (!transformsToCalculate.empty())
	{
		// Get next transform to calculate
		auto next = transformsToCalculate.front();
		transformsToCalculate.pop();

		auto parent = next->parent;
		ASSERT(parent != nullptr);
		
		if (next->dirtyLocalObjectToWorldMatrix)
		{
			next->localObjectToWorld = Matrix4x4f::TRS(next->localPosition, next->localRotation, next->localScale);
			next->dirtyLocalObjectToWorldMatrix = false;
		}
		next->objectToWorld = parent->objectToWorld;
		next->objectToWorld.Multiply(next->localObjectToWorld);
		next->objectToWorld = Matrix4x4f::Transpose(next->objectToWorld);
		next->position = next->objectToWorld.Multiply(Vector3f(0, 0, 0));

		// Add transform childs
		for (auto child : next->childs)
			transformsToCalculate.push(child);
	}
}

void TransformModule::SetupExecuteOrder(ModuleManager * moduleManager)
{
	PipeModule::SetupExecuteOrder(moduleManager);
	memoryModule = ExecuteAfter<MemoryModule>(moduleManager);
	memoryModule->SetAllocator(0, new FixedBlockHeap(sizeof(Transform)));
}

const Transform* TransformModule::AllocateTransform() const
{
	return memoryModule->New<Transform>(0, this);
}

DECLARE_COMMAND_CODE(CreateTransform);
const Transform* TransformModule::RecCreateTransform(const ExecutionContext& context, const Transform* transform)
{
	auto buffer = GetRecordingBuffer(context);
	auto& stream = buffer->stream;
	auto target = transform == nullptr ? AllocateTransform() : transform;
	stream.Write(CommandCodeCreateTransform);
	stream.Write(target);
	buffer->commandCount++;
	return target;
}
//SERIALIZE_METHOD_CREATECMP(TransformModule, Transform);

SERIALIZE_METHOD_ARG1(TransformModule, Destroy, const Component*);
SERIALIZE_METHOD_ARG2(TransformModule, SetParent, const Transform*, const Transform*);
SERIALIZE_METHOD_ARG2(TransformModule, SetPosition, const Transform*, const Vector3f&);
SERIALIZE_METHOD_ARG1(TransformModule, CalculateWorldToView, const Transform*);

bool TransformModule::ExecuteCommand(const ExecutionContext& context, MemoryStream& stream, uint32_t commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(CreateTransform, Transform*, target);
		target->localPosition = Vector3f(0, 0, 0);
		target->localRotation = Quaternionf(0, 0, 0, 1);
		target->localScale = Vector3f(1, 1, 1);
		target->parent = root;
		root->childs.push_back(target);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG1_START(Destroy, Transform*, transform);
		auto parent = transform->parent;
		ASSERT(parent != nullptr);
		parent->childs.remove(transform);

		for (auto child : transform->childs)
		{
			auto unit = child->unit;
			unit->module->RecDestroy(context, unit);
		}
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG2_START(SetParent, Transform*, transform, Transform*, parent);
		auto oldParent = transform->parent;
		ASSERT(oldParent != nullptr);
		oldParent->childs.remove(transform);

		parent->childs.push_back(transform);
		transform->parent = parent;
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG2_START(SetPosition, Transform*, transform, Vector3f, position);
		transform->localPosition = position;
		transform->dirtyLocalObjectToWorldMatrix = true;
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG1_START(CalculateWorldToView, Transform*, target);

		// Find all target ancestors
		worldToViewToCalculate.clear();
		auto searchTransform = target;
		while (searchTransform != root)
		{
			worldToViewToCalculate.push_back(searchTransform);
			searchTransform = searchTransform->parent;
		}

		// Calculate view matrix for each of them
		Transform* parentTransform = nullptr;
		while (!worldToViewToCalculate.empty())
		{
			auto transformToCalculate = worldToViewToCalculate.back();
			worldToViewToCalculate.pop_back();

			if (parentTransform != nullptr)
				transformToCalculate->worldToView = parentTransform->worldToView;
			else
				transformToCalculate->worldToView = Matrix4x4f::Indentity<float>();
			transformToCalculate->worldToView.Multiply(Matrix4x4f::Rotate(-transformToCalculate->localRotation));
			transformToCalculate->worldToView.Multiply(Matrix4x4f::Translate(-transformToCalculate->localPosition));

			parentTransform = transformToCalculate;
		}

		DESERIALIZE_METHOD_END;
	}
	return false;
}