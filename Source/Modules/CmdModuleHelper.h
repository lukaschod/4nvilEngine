#pragma once

#define NOARG
#define COMMA ,

#define DECLARE_COMMAND_CODE(Name) static const int kCommandCode##Name = __COUNTER__;

#define SERIALIZE_METHOD_TEMPLATE(Module, Name, ReturnType, DeclareArguments, WriteArguments, BodyPrefix, BodyPostFix) \
	DECLARE_COMMAND_CODE(Name); \
	ReturnType Module::Record##Name(const ExecutionContext& context DeclareArguments) \
	{ \
		auto buffer = GetRecordingBuffer(context); \
		auto& stream = buffer->stream; \
		BodyPrefix \
		stream.Write(kCommandCode##Name); \
		WriteArguments \
		buffer->commandCount++; \
		BodyPostFix \
	}

#define DECLARE_ARG1(ArgumentType1) COMMA ArgumentType1 argument1
#define DECLARE_ARG2(ArgumentType1, ArgumentType2) COMMA ArgumentType1 argument1 COMMA ArgumentType2 argument2
#define DECLARE_ARG3(ArgumentType1, ArgumentType2, ArgumentType3) COMMA ArgumentType1 argument1 COMMA ArgumentType2 argument2 COMMA ArgumentType3 argument3
#define DECLARE_ARG4(ArgumentType1, ArgumentType2, ArgumentType3, ArgumentType4) COMMA ArgumentType1 argument1 COMMA ArgumentType2 argument2 COMMA ArgumentType3 argument3 COMMA ArgumentType4 argument4
#define WRITE_ARG1 stream.Write(argument1);
#define WRITE_ARG2 stream.Write(argument1);stream.Write(argument2);
#define WRITE_ARG3 stream.Write(argument1);stream.Write(argument2);stream.Write(argument3);
#define WRITE_ARG4 stream.Write(argument1);stream.Write(argument2);stream.Write(argument3);stream.Write(argument4);

#define SERIALIZE_METHOD(Module, Name) \
	SERIALIZE_METHOD_TEMPLATE(Module, Name, void, NOARG, NOARG, NOARG, NOARG)
#define SERIALIZE_METHOD_ARG1(Module, Name, ArgumentType1) \
	SERIALIZE_METHOD_TEMPLATE(Module, Name, void, DECLARE_ARG1(ArgumentType1), WRITE_ARG1, NOARG, NOARG)
#define SERIALIZE_METHOD_ARG2(Module, Name, ArgumentType1, ArgumentType2) \
	SERIALIZE_METHOD_TEMPLATE(Module, Name, void, DECLARE_ARG2(ArgumentType1, ArgumentType2), WRITE_ARG2, NOARG, NOARG)
#define SERIALIZE_METHOD_ARG3(Module, Name, ArgumentType1, ArgumentType2, ArgumentType3) \
	SERIALIZE_METHOD_TEMPLATE(Module, Name, void, DECLARE_ARG3(ArgumentType1, ArgumentType2, ArgumentType3), WRITE_ARG3, NOARG, NOARG)
#define SERIALIZE_METHOD_ARG4(Module, Name, ArgumentType1, ArgumentType2, ArgumentType3, ArgumentType4) \
	SERIALIZE_METHOD_TEMPLATE(Module, Name, void, DECLARE_ARG4(ArgumentType1, ArgumentType2, ArgumentType3, ArgumentType4), WRITE_ARG3, NOARG, NOARG)

#define WRITE_TARGET stream.Write(target);
#define RETURN_TARGET return target;

#define SERIALIZE_METHOD_CREATEGEN(Module, Name, CreateType) \
	SERIALIZE_METHOD_TEMPLATE(Module, Create##Name, const Name*, NOARG, WRITE_TARGET, auto target = new CreateType();, RETURN_TARGET)
#define SERIALIZE_METHOD_CREATEGEN_ARG1(Module, Name, CreateType, ArgumentType1) \
	SERIALIZE_METHOD_TEMPLATE(Module, Create##Name, const Name*, DECLARE_ARG1(ArgumentType1), WRITE_TARGET, auto target = new CreateType(argument1);, RETURN_TARGET)
#define SERIALIZE_METHOD_CREATEGEN_ARG2(Module, Name, CreateType, ArgumentType1, ArgumentType2) \
	SERIALIZE_METHOD_TEMPLATE(Module, Create##Name, const Name*, DECLARE_ARG2(ArgumentType1, ArgumentType2), WRITE_TARGET, auto target = new CreateType(argument1, argument2);, RETURN_TARGET)

#define SERIALIZE_METHOD_CREATE(Module, Name) SERIALIZE_METHOD_CREATEGEN(Module, Name, Name)
#define SERIALIZE_METHOD_CREATE_ARG1(Module, Name, ArgumentType1) SERIALIZE_METHOD_CREATEGEN_ARG1(Module, Name, Name, ArgumentType1)
#define SERIALIZE_METHOD_CREATE_ARG2(Module, Name, ArgumentType1, ArgumentType2) SERIALIZE_METHOD_CREATEGEN_ARG2(Module, Name, Name, ArgumentType1, ArgumentType2)

#define SERIALIZE_METHOD_CREATECMP(Module, Name) \
	SERIALIZE_METHOD_TEMPLATE(Module, Create##Name, const Name*, NOARG, WRITE_TARGET, auto target = new Name(this);, RETURN_TARGET)

#define SERIALIZE_BATCH(Index, Target) \
	if (buffer->lastRecordingBatchIndex != Index || buffer->lastRecordingBatchTarget != Target) \
	{ \
		stream.Write(buffer->lastRecordingBatchIndex); \
		stream.Write(buffer->lastRecordingBatchTarget); \
		buffer->commandCount++; \
		buffer->lastRecordingBatchIndex = Index; \
		buffer->lastRecordingBatchTarget = Target; \
	}

#define SERIALIZE_METHOD_BATCH_ARG1(Module, Name, BatchName, ArgumentType1) \
	 SERIALIZE_METHOD_TEMPLATE(Module, Name, void, COMMA ArgumentType1 argument1, stream.Write(argument1);, SERIALIZE_BATCH(kCommandCode##BatchName, argument1), NOARG)
#define SERIALIZE_METHOD_BATCH_ARG2(Module, Name, BatchName, ArgumentType1, ArgumentType2) \
	 SERIALIZE_METHOD_TEMPLATE(Module, Name, void, COMMA ArgumentType1 argument1 COMMA ArgumentType2 argument2, stream.Write(argument1); stream.Write(argument2);, SERIALIZE_BATCH(kCommandCode##BatchName, argument1), NOARG)
#define SERIALIZE_METHOD_BATCH_ARG3(Module, Name, BatchName, ArgumentType1, ArgumentType2, ArgumentType3) \
	 SERIALIZE_METHOD_TEMPLATE(Module, Name, void, COMMA ArgumentType1 argument1 COMMA ArgumentType2 argument2 COMMA ArgumentType3 argument3, stream.Write(argument1); stream.Write(argument2); stream.Write(argument3);, SERIALIZE_BATCH(kCommandCode##BatchName, argument1), NOARG)