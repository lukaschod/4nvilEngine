#include <Core\Foundation\UnitModule.hpp>
#include <Core\Foundation\MemoryModule.hpp>

using namespace Core;

static const char* memoryLabelUnit = "Core::Unit";

void UnitModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	base::SetupExecuteOrder(moduleManager);
	memoryModule = ExecuteAfter<MemoryModule>(moduleManager);
	memoryModule->SetAllocator(memoryLabelUnit, new FixedBlockHeap(sizeof(Unit)));
}

const Unit* UnitModule::AllocateUnit() const
{
	return memoryModule->New<Unit>(memoryLabelUnit);
}

SERIALIZE_METHOD_ARG1(UnitModule, CreateUnit, const Unit*);
SERIALIZE_METHOD_ARG1(UnitModule, Destroy, const Unit*);
SERIALIZE_METHOD_ARG2(UnitModule, AddComponent, const Unit*, const Component*);

bool UnitModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(CreateUnit, Unit*, unit);
		units.push_back(unit);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG1_START(Destroy, Unit*, unit);
		units.remove(unit);
		for (auto component : unit->components)
			component->module->RecDestroy(context, component);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG2_START(AddComponent, Unit*, unit, Component*, component);
		auto oldUnit = (Unit*) component->unit;
		if (oldUnit != nullptr)
			oldUnit->components.remove(component);
		unit->components.push_back(component);
		component->unit = unit;
		DESERIALIZE_METHOD_END;
	}
	return false;
}