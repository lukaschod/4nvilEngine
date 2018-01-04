#pragma once

#include <Tools\Common.h>
#include <Modules\PipeModule.h>
#include <Graphics\IGraphicsModule.h>
#include <Graphics\IBuffer.h>

struct Storage
{
	Storage(const IBuffer* buffer)
		: buffer(buffer)
		, size(buffer->GetSize())
	{}
	const IBuffer* buffer;
	const size_t size;
};

class StorageModule : public PipeModule
{
public:
	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
	const Storage* AllocateStorage(size_t size) const;

public:
	const Storage* RecCreateStorage(const ExecutionContext& context, uint32_t size, const Storage* storage = nullptr);
	void RecUpdateStorage(const ExecutionContext& context, const Storage* target, uint32_t targetOffset, Range<void>& data);

protected:
	virtual bool ExecuteCommand(const ExecutionContext& context, MemoryStream& stream, uint32_t commandCode) override;

private:
	List<Storage*> storages;
	IGraphicsModule* graphicsModule;
};