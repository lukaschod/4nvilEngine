#pragma once

#include <Common\EngineCommon.h>
#include <Modules\CmdModule.h>
#include <Common\Collections\List.h>

#define RECORD_CMD_CREATE2(Module, ReturnType, CreateType) \
	DECLARE_COMMAND_CODE(Create##ReturnType); \
	const ReturnType* Module::RecCreate##ReturnType(const ExecutionContext& context) \
	{ \
		ReturnType* unit = (ReturnType*)new CreateType(this); \
		auto buffer = GetRecordingBuffer(context); \
		auto& stream = buffer->stream; \
		stream.Write(CommandCodeCreate##ReturnType); \
		stream.Write(unit); \
		buffer->commandCount++; \
		return unit; \
	}

#define RECORD_CMD_CREATE(Module, UnitType) RECORD_CMD_CREATE2(Module, UnitType, UnitType)

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

class ComponentModule : public CmdModule
{
public:
	ComponentModule(uint32_t bufferCount, uint32_t workersCount) : CmdModule(bufferCount, workersCount)
	{
	}

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

class UnitModule : public CmdModule
{
public:
	UnitModule(uint32_t bufferCount, uint32_t workersCount);
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
	virtual bool ExecuteCommand(const ExecutionContext& context, IOStream& stream, uint32_t commandCode) override;

private:
	List<Unit*> units;
};