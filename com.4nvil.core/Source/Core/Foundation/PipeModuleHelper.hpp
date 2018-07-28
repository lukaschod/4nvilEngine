/*
* Copyright (c) Lukas Chodosevicius
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#pragma once

#define NOARG
#define COMMA ,

#define TO_COMMAND_CODE(Name) CommandCode##Name
#define DECLARE_COMMAND_CODE(Name) static const int TO_COMMAND_CODE(Name) = __COUNTER__;

#define SERIALIZE_METHOD_TEMPLATE(Module, Name, ReturnType, DeclareArguments, WriteArguments, BodyPrefix, BodyPostFix) \
    DECLARE_COMMAND_CODE(Name); \
    ReturnType Module::Rec##Name(const ExecutionContext& context DeclareArguments) \
    { \
        auto buffer = GetRecordingBuffer(context); \
        auto& stream = buffer->stream; \
        BodyPrefix \
        stream.Write(TO_COMMAND_CODE(Name)); \
        WriteArguments \
        stream.Align(); \
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
    SERIALIZE_METHOD_TEMPLATE(Module, Name, Void, NOARG, NOARG, NOARG, NOARG)
#define SERIALIZE_METHOD_ARG1(Module, Name, ArgumentType1) \
    SERIALIZE_METHOD_TEMPLATE(Module, Name, Void, DECLARE_ARG1(ArgumentType1), WRITE_ARG1, NOARG, NOARG)
#define SERIALIZE_METHOD_ARG2(Module, Name, ArgumentType1, ArgumentType2) \
    SERIALIZE_METHOD_TEMPLATE(Module, Name, Void, DECLARE_ARG2(ArgumentType1, ArgumentType2), WRITE_ARG2, NOARG, NOARG)
#define SERIALIZE_METHOD_ARG3(Module, Name, ArgumentType1, ArgumentType2, ArgumentType3) \
    SERIALIZE_METHOD_TEMPLATE(Module, Name, Void, DECLARE_ARG3(ArgumentType1, ArgumentType2, ArgumentType3), WRITE_ARG3, NOARG, NOARG)
#define SERIALIZE_METHOD_ARG4(Module, Name, ArgumentType1, ArgumentType2, ArgumentType3, ArgumentType4) \
    SERIALIZE_METHOD_TEMPLATE(Module, Name, Void, DECLARE_ARG4(ArgumentType1, ArgumentType2, ArgumentType3, ArgumentType4), WRITE_ARG3, NOARG, NOARG)

#define DESERIALIZE_METHOD_START(Name) \
    case CommandCode##Name: { 

#define DESERIALIZE_METHOD_ARG1_START(Name, ArgumentType1, ArgumentName1) \
    case CommandCode##Name: { \
    ArgumentType1& ArgumentName1 = stream.FastRead<ArgumentType1>(); 

#define DESERIALIZE_METHOD_ARG2_START(Name, ArgumentType1, ArgumentName1, ArgumentType2, ArgumentName2) \
    case CommandCode##Name: { \
    ArgumentType1& ArgumentName1 = stream.FastRead<ArgumentType1>(); \
    ArgumentType2& ArgumentName2 = stream.FastRead<ArgumentType2>(); 

#define DESERIALIZE_METHOD_ARG3_START(Name, ArgumentType1, ArgumentName1, ArgumentType2, ArgumentName2, ArgumentType3, ArgumentName3) \
    case CommandCode##Name: { \
    ArgumentType1& ArgumentName1 = stream.FastRead<ArgumentType1>(); \
    ArgumentType2& ArgumentName2 = stream.FastRead<ArgumentType2>(); \
    ArgumentType3& ArgumentName3 = stream.FastRead<ArgumentType3>(); 

#define DESERIALIZE_METHOD_ARG4_START(Name, ArgumentType1, ArgumentName1, ArgumentType2, ArgumentName2, ArgumentType3, ArgumentName3, ArgumentType4, ArgumentName4) \
    case CommandCode##Name: { \
    ArgumentType1& ArgumentName1 = stream.FastRead<ArgumentType1>(); \
    ArgumentType2& ArgumentName2 = stream.FastRead<ArgumentType2>(); \
    ArgumentType3& ArgumentName3 = stream.FastRead<ArgumentType3>(); \
    ArgumentType4& ArgumentName4 = stream.FastRead<ArgumentType4>(); 

#define DESERIALIZE_METHOD_END return true; }

#define SERIALIZE_METHOD_HEADER(Module, Name, ReturnType) ReturnType Module::Rec##Name

#define SERIALIZE_METHOD_BODY_HEADER(Name) \
{ \
    auto buffer = GetRecordingBuffer(context); \
    auto& stream = buffer->stream; \
    stream.Write(TO_COMMAND_CODE(Name));

#define SERIALIZE_METHOD_BODY_FOOTER \
    stream.Align(); \
    buffer->commandCount++; \
}

#define SERIALIZE_METHOD_BODY_FOOTER_RET(ReturnType) \
    ReturnType& ret = *(ReturnType*)stream.data; \
    stream.Write(ReturnType()); \
    stream.Align(); \
    buffer->commandCount++; \
    return ret; \
}

#define SERIALIZE_METHOD_ARG1_RET(Module, Name, ArgumentName1, ReturnType) \
    ReturnType& Module::Rec##Name(const ExecutionContext& context, ArgumentType1 argument1) \
    SERIALIZE_METHOD_BODY_HEADER(Name) \
    stream.Write(argument1); \
    SERIALIZE_METHOD_BODY_FOOTER_RET(ReturnType)