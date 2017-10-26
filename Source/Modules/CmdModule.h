#pragma once

#include <Common\EngineCommon.h>
#include <Common\IOStream.h>
#include <Modules\Module.h>
#include <Modules\CmdModuleHelper.h>
#include <Modules\CmdBufferPoolModule.h>

/*#define DESERIALIZE_METHOD_START(Name) \
	case kCommandCode##Name: { \
	EXT_TRACE("%s::%s index=%d", GetName(), #Name, context.workerIndex);

#define DESERIALIZE_METHOD_ARG1_START(Name, ArgumentType1, ArgumentName1) \
	case kCommandCode##Name: { \
	ArgumentType1 ArgumentName1; stream.Read(ArgumentName1); \
	EXT_TRACE("%s::%s index=%d", GetName(), #Name, context.workerIndex);

#define DESERIALIZE_METHOD_ARG2_START(Name, ArgumentType1, ArgumentName1, ArgumentType2, ArgumentName2) \
	case kCommandCode##Name: { \
	ArgumentType1 ArgumentName1; stream.Read(ArgumentName1); \
	ArgumentType2 ArgumentName2; stream.Read(ArgumentName2); \
	EXT_TRACE("%s::%s index=%d", GetName(), #Name, context.workerIndex);

#define DESERIALIZE_METHOD_ARG3_START(Name, ArgumentType1, ArgumentName1, ArgumentType2, ArgumentName2, ArgumentType3, ArgumentName3) \
	case kCommandCode##Name: { \
	ArgumentType1 ArgumentName1; stream.Read(ArgumentName1); \
	ArgumentType2 ArgumentName2; stream.Read(ArgumentName2); \
	ArgumentType3 ArgumentName3; stream.Read(ArgumentName3); \
	EXT_TRACE("%s::%s index=%d", GetName(), #Name, context.workerIndex);

#define DESERIALIZE_METHOD_ARG4_START(Name, ArgumentType1, ArgumentName1, ArgumentType2, ArgumentName2, ArgumentType3, ArgumentName3, ArgumentType4, ArgumentName4) \
	case kCommandCode##Name: { \
	ArgumentType1 ArgumentName1; stream.Read(ArgumentName1); \
	ArgumentType2 ArgumentName2; stream.Read(ArgumentName2); \
	ArgumentType3 ArgumentName3; stream.Read(ArgumentName3); \
	ArgumentType4 ArgumentName4; stream.Read(ArgumentName4); \
	EXT_TRACE("%s::%s index=%d", GetName(), #Name, context.workerIndex);*/

#define DESERIALIZE_METHOD_START(Name) \
	case kCommandCode##Name: { \
	EXT_TRACE("%s::%s index=%d", GetName(), #Name, context.workerIndex);

#define DESERIALIZE_METHOD_ARG1_START(Name, ArgumentType1, ArgumentName1) \
	case kCommandCode##Name: { \
	ArgumentType1& ArgumentName1 = stream.FastRead<ArgumentType1>(); \
	EXT_TRACE("%s::%s index=%d", GetName(), #Name, context.workerIndex);

#define DESERIALIZE_METHOD_ARG2_START(Name, ArgumentType1, ArgumentName1, ArgumentType2, ArgumentName2) \
	case kCommandCode##Name: { \
	ArgumentType1& ArgumentName1 = stream.FastRead<ArgumentType1>(); \
	ArgumentType2& ArgumentName2 = stream.FastRead<ArgumentType2>(); \
	EXT_TRACE("%s::%s index=%d", GetName(), #Name, context.workerIndex);

#define DESERIALIZE_METHOD_ARG3_START(Name, ArgumentType1, ArgumentName1, ArgumentType2, ArgumentName2, ArgumentType3, ArgumentName3) \
	case kCommandCode##Name: { \
	ArgumentType1& ArgumentName1 = stream.FastRead<ArgumentType1>(); \
	ArgumentType2& ArgumentName2 = stream.FastRead<ArgumentType2>(); \
	ArgumentType3& ArgumentName3 = stream.FastRead<ArgumentType3>(); \
	EXT_TRACE("%s::%s index=%d", GetName(), #Name, context.workerIndex);

#define DESERIALIZE_METHOD_ARG4_START(Name, ArgumentType1, ArgumentName1, ArgumentType2, ArgumentName2, ArgumentType3, ArgumentName3, ArgumentType4, ArgumentName4) \
	case kCommandCode##Name: { \
	ArgumentType1& ArgumentName1 = stream.FastRead<ArgumentType1>(); \
	ArgumentType2& ArgumentName2 = stream.FastRead<ArgumentType2>(); \
	ArgumentType3& ArgumentName3 = stream.FastRead<ArgumentType3>(); \
	ArgumentType4& ArgumentName4 = stream.FastRead<ArgumentType4>(); \
	EXT_TRACE("%s::%s index=%d", GetName(), #Name, context.workerIndex);

#define DESERIALIZE_METHOD_END return true; }

#define SERIALIZE_METHOD_RESOURCE(Module, ReturnType, CreateType) \
	DECLARE_COMMAND_CODE(Create##ReturnType); \
	const ReturnType* Module::RecordCreate##ReturnType(const ExecutionContext& context) \
	{ \
		auto unit = (ReturnType*)new CreateType(); \
		auto buffer = GetRecordingBuffer(context); \
		auto& stream = buffer->stream; \
		stream.Write(kCommandCodeCreate##ReturnType); \
		stream.Write(unit); \
		buffer->commandCount++; \
		return unit; \
	}

#define SERIALIZE_METHOD_RESOURCE2(Module, ReturnType, CreateType, ArgumentType1, ArgumentType2) \
	DECLARE_COMMAND_CODE(Create##ReturnType); \
	const ReturnType* Module::RecordCreate##ReturnType(const ExecutionContext& context, ArgumentType1 argument1, ArgumentType2 argument2) \
	{ \
		auto unit = (ReturnType*)new CreateType(); \
		auto buffer = GetRecordingBuffer(context); \
		auto& stream = buffer->stream; \
		stream.Write(kCommandCodeCreate##ReturnType); \
		stream.Write(unit); \
		stream.Write(argument1); \
		stream.Write(argument2); \
		buffer->commandCount++; \
		return unit; \
	}



class CmdModule : public Module
{
public:
	CmdModule(uint32_t bufferCount, uint32_t workersCount);
	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
	virtual void Execute(const ExecutionContext& context) override;
	virtual size_t GetExecutionkSize() override;

protected:
	virtual bool ExecuteCommand(const ExecutionContext& context, IOStream& stream, uint32_t commandCode) = 0;

	CmdBuffer* GetRecordingBuffer(const ExecutionContext& context);

private:
	struct Context
	{
		List<CmdBuffer*> buffers;
	};

	List<Context> contexts;
	uint32_t backbufferIndex;
	List<CmdBuffer*> cmdBuffersToExecute;

	CmdBufferPoolModule* cmdBufferPoolModule;

	uint64_t bufferCounter;
};