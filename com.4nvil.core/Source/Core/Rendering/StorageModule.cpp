#include <Core\Graphics\IGraphicsModule.h>
#include <Core\Graphics\IBuffer.h>
#include <Core\Rendering\StorageModule.h>

using namespace Core;
using namespace Core::Math;
using namespace Core::Graphics;

void StorageModule::SetupExecuteOrder(ModuleManager* moduleManager)
{
	base::SetupExecuteOrder(moduleManager);
	graphicsModule = ExecuteBefore<IGraphicsModule>(moduleManager);
}

const Storage* StorageModule::AllocateStorage(size_t size) const
{
	auto buffer = graphicsModule->AllocateBuffer(size);
	return new Storage(buffer);
}

SERIALIZE_METHOD_ARG1(StorageModule, CreateStorage, const Storage*);
SERIALIZE_METHOD_ARG2(StorageModule, SetUsage, const Storage*, BufferUsageFlags);
SERIALIZE_METHOD_ARG3(StorageModule, UpdateStorage, const Storage*, uint32, Range<void>&);
SERIALIZE_METHOD_ARG3(StorageModule, CopyStorage, const Storage*, const Storage*, size_t);

bool StorageModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(CreateStorage, Storage*, target);
		graphicsModule->RecCreateIBuffer(context, target->buffer);
		storages.push_back(target);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG2_START(SetUsage, Storage*, target, BufferUsageFlags, usage);
		graphicsModule->RecSetBufferUsage(context, target->buffer, usage);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG3_START(UpdateStorage, Storage*, target, uint32, targetOffset, Range<void>, data);
		graphicsModule->RecUpdateBuffer(context, target->buffer, (void*)data.pointer, data.size);
		DESERIALIZE_METHOD_END;

		DESERIALIZE_METHOD_ARG3_START(CopyStorage, Storage*, src, Storage*, dst, size_t, size);
		graphicsModule->RecCopyBuffer(context, src->buffer, dst->buffer, size);
		DESERIALIZE_METHOD_END;
	}
	return false;
}