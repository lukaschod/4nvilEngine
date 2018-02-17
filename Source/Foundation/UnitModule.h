#pragma once

#include <Tools\Common.h>
#include <Foundation\PipeModule.h>
#include <Tools\Collections\List.h>

class ComponentModule;
class UnitModule;
struct Unit;

struct Component
{
	Component(ComponentModule* module) : 
		module(module), 
		unit(nullptr) {}
	virtual ~Component() {}

	const Unit* unit;
	ComponentModule* module;
};

class ComponentModule : public PipeModule
{
public:
	virtual void RecDestroy(const ExecutionContext& context, const Component* unit) = 0;
};

struct Unit
{
	Unit(UnitModule* module) : 
		module(module) 
	{}

	List<const Component*> components;
	UnitModule* module;
};

class UnitModule : public PipeModule
{
public:
	const Unit* RecCreateUnit(const ExecutionContext& context);
	void RecDestroy(const ExecutionContext& context, const Unit* unit);
	void RecAddComponent(const ExecutionContext& context, const Unit* unit, const Component* component);

	template<class T>
	const T* GetComponent(const Component* target)
	{
		auto unit = target->unit;
		for (auto component : unit->components)
		{
			if (dynamic_cast<const T*>(component) != 0)
				return (const T*) component;
		}
		ERROR("Can't find specified component");
		return nullptr;
	}

	template<class T>
	const T* GetComponent(const Unit* target)
	{
		for (auto component : target->components)
		{
			if (dynamic_cast<const T*>(component) != 0)
				return (const T*) component;
		}
		ERROR("Can't find specified component");
		return nullptr;
	}

protected:
	virtual bool ExecuteCommand(const ExecutionContext& context, MemoryStream& stream, CommandCode commandCode) override;

private:
	List<Unit*> units;
};