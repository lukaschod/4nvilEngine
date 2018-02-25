#pragma once

#include <Tools\Common.h>
#include <Foundation\PipeModule.h>

namespace Core::Graphics
{
	struct IBuffer;
	class IGraphicsModule;
}

namespace Core
{
	struct Storage
	{
		Storage(const Graphics::IBuffer* buffer)
			: buffer(buffer)
			, size(buffer->GetSize())
		{
		}
		const Graphics::IBuffer* buffer;
		const size_t size;
	};

	class StorageModule : public PipeModule
	{
	public:
		virtual void Execute(const ExecutionContext& context) override { MARK_FUNCTION; base::Execute(context); }
		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
		const Storage* AllocateStorage(size_t size) const;

		const Storage* RecCreateStorage(const ExecutionContext& context, uint32 size, const Storage* storage = nullptr);
		void RecUpdateStorage(const ExecutionContext& context, const Storage* target, uint32 targetOffset, Range<void>& data);

	protected:
		virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

	private:
		List<Storage*> storages;
		Graphics::IGraphicsModule* graphicsModule;
	};
}