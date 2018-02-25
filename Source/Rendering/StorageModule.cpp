#include <Graphics\IGraphicsModule.h>
#include <Graphics\IBuffer.h>
#include <Rendering\StorageModule.h>

using namespace Core;
using namespace Core::Math;
using namespace Core::Graphics;

void StorageModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	PipeModule::SetupExecuteOrder(moduleManager);
	graphicsModule = ExecuteBefore<IGraphicsModule>(moduleManager);
}

const Storage* StorageModule::AllocateStorage(size_t size) const
{
	auto buffer = graphicsModule->AllocateBuffer(size);
	return new Storage(buffer);
}

DECLARE_COMMAND_CODE(CreateStorage);
const Storage* StorageModule::RecCreateStorage(const ExecutionContext& context, uint32 size, const Storage* storage)
{
	auto buffer = GetRecordingBuffer(context);
	auto& stream = buffer->stream;
	auto target = storage == nullptr ? AllocateStorage(size) : storage;
	stream.Write(TO_COMMAND_CODE(CreateStorage));
	stream.Write(target);
	stream.Align();
	buffer->commandCount++;
	return target;
}

SERIALIZE_METHOD_ARG3(StorageModule, UpdateStorage, const Storage*, uint32, Range<void>&);

bool StorageModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(CreateStorage, Storage*, target);
		graphicsModule->RecCreateIBuffer(context, target->buffer->GetSize(), target->buffer);
		storages.push_back(target);
		DESERIALIZE_METHOD_END

		DESERIALIZE_METHOD_ARG3_START(UpdateStorage, Storage*, target, uint32, targetOffset, Range<void>, data);
		graphicsModule->RecUpdateBuffer(context, target->buffer, (void*)data.pointer, data.size);
		DESERIALIZE_METHOD_END
	}
	return false;
}