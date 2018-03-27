#pragma once

#include <Core\Tools\Common.h>
#include <Core\Foundation\PipeModule.h>
#include <Core\Graphics\IBuffer.h>

namespace Core::Graphics
{
	class IGraphicsModule;
}

namespace Core
{
	struct Storage
	{
		Storage(const Graphics::IBuffer* buffer)
			: buffer(buffer)
			, size(buffer->GetSize())
			, created(false)
		{
		}
		const Graphics::IBuffer* buffer;
		const size_t size;
		bool created;
	};

	class StorageModule : public PipeModule
	{
	public:
		BASE_IS(PipeModule);

		virtual void Execute(const ExecutionContext& context) override { MARK_FUNCTION; base::Execute(context); }
		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
		const Storage* AllocateStorage(size_t size) const;

		void RecCreateStorage(const ExecutionContext& context, const Storage* storage);
		void RecSetUsage(const ExecutionContext& context, const Storage* target, Graphics::BufferUsageFlags usage);
		void RecUpdateStorage(const ExecutionContext& context, const Storage* target, uint32 targetOffset, Range<void>& data);
		void RecCopyStorage(const ExecutionContext& context, const Storage* src, const Storage* dst, size_t size);

	protected:
		virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

	private:
		List<Storage*> storages;
		Graphics::IGraphicsModule* graphicsModule;
	};
}