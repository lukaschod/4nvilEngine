#include <Rendering\StorageModule.h>

StorageModule::StorageModule(uint32_t bufferCount, uint32_t bufferIndexStep) :
	CmdModule(bufferCount, bufferIndexStep)
{
}

void StorageModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	CmdModule::SetupExecuteOrder(moduleManager);
	graphicsModule = ExecuteBefore<IGraphicsModule>(moduleManager);
}

DECLARE_COMMAND_CODE(CreateStorage);
const Storage* StorageModule::RecordCreateStorage(const ExecutionContext& context, uint32_t size)
{
	auto buffer = GetRecordingBuffer(context);
	auto& stream = buffer->stream;
	auto gfxBuffer = graphicsModule->RecordCreateIBuffer(context, size);
	auto target = new Storage(gfxBuffer);
	stream.Write(kCommandCodeCreateStorage);
	stream.Write(target);
	buffer->commandCount++;
	return target;
}

SERIALIZE_METHOD_ARG3(StorageModule, UpdateStorage, const Storage*, uint32_t, Range<void>&);

bool StorageModule::ExecuteCommand(const ExecutionContext& context, IOStream& stream, uint32_t commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(CreateStorage, Storage*, target);
		target->buffer = graphicsModule->RecordCreateIBuffer(context, target->size);
		storages.push_back(target);
		DESERIALIZE_METHOD_END

		DESERIALIZE_METHOD_ARG3_START(UpdateStorage, Storage*, target, uint32_t, targetOffset, Range<void>, data);
		graphicsModule->RecordUpdateBuffer(context, target->buffer, (void*)data.pointer, data.size);
		DESERIALIZE_METHOD_END
	}
	return false;
}