#include <Foundation\UnitModule.h>

UnitModule::UnitModule(uint32_t bufferCount, uint32_t workersCount) : CmdModule(bufferCount, workersCount) {}

SERIALIZE_METHOD_CREATECMP(UnitModule, Unit);
SERIALIZE_METHOD_ARG1(UnitModule, Destroy, const Unit*);
SERIALIZE_METHOD_ARG2(UnitModule, AddComponent, const Unit*, const Component*);

bool UnitModule::ExecuteCommand(const ExecutionContext& context, MemoryStream& stream, uint32_t commandCode)
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