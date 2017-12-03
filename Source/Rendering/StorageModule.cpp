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
const Storage* StorageModule::RecCreateStorage(const ExecutionContext& context, uint32_t size)
{
	auto buffer = GetRecordingBuffer(context);
	auto& stream = buffer->stream;
	auto gfxBuffer = graphicsModule->RecCreateIBuffer(context, size);
	auto target = new Storage(gfxBuffer);
	stream.Write(CommandCodeCreateStorage);
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
		static int counter = 0;
		TRACE("%d", counter++);
		//target->buffer = graphicsModule->RecCreateIBuffer(context, target->size);
		storages.push_back(target);
		DESERIALIZE_METHOD_END

		DESERIALIZE_METHOD_ARG3_START(UpdateStorage, Storage*, target, uint32_t, targetOffset, Range<void>, data);
		graphicsModule->RecUpdateBuffer(context, target->buffer, (void*)data.pointer, data.size);
		DESERIALIZE_METHOD_END
	}
	return false;
}