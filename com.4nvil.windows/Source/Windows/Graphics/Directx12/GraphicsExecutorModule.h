#pragma once

#include <Core\Foundation\ComputeModule.h>
#include <Windows\Graphics\Directx12\Common.h>
#include <Windows\Graphics\Directx12\CmdQueue.h>

namespace Windows::Directx12
{
	struct CmdBuffer;
}

namespace Windows::Directx12
{
	class GraphicsExecutorModule : public ComputeModule
	{
	public:
		BASE_IS(ComputeModule);

		GraphicsExecutorModule();
		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
		virtual void Execute(const ExecutionContext& context) override;
		virtual const char* GetName() override { return "GraphicsExecutorModule"; }

		void RecCmdBuffer(const ExecutionContext& context, const Directx12::CmdBuffer* buffer);

	private:
		struct Context
		{
			List<const Directx12::CmdBuffer*> recordedCmds;
		};
		List<Context> executerContexts;

		List<const Directx12::CmdBuffer*> cmdBuffersToExecute;
		List<ID3D12GraphicsCommandList*> batchedCommandLists;
		AUTOMATED_PROPERTY_GET(uint64, completedBufferIndex);
	};
}