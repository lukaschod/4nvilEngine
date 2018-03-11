#pragma once

#include <Core\Tools\Enum.h>
#include <Core\Foundation\ComputeModule.h>
#include <Core\Graphics\IGraphicsModule.h>
#include <Windows\Graphics\Directx12\Common.h>
#include <Windows\Graphics\Directx12\DescriptorHeap.h>
#include <Windows\Graphics\Directx12\CmdAllocatorPool.h>
#include <Windows\Graphics\Directx12\CmdQueue.h>

using namespace Core::Math;
using namespace Core::Graphics;

namespace Windows::Directx12
{
	struct Texture;
	struct Buffer;
	struct SwapChain;
	struct RenderPass;
	struct ShaderArguments;
	class GraphicsModule;
	class GraphicsExecutorModule;
}

namespace Windows::Directx12
{
	class GraphicsPlannerModule : public ComputeModule
	{
	public:
		GraphicsPlannerModule(ID3D12Device* device);
		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
		virtual void Execute(const ExecutionContext& context) override;
		virtual size_t GetExecutionSize() override;
		virtual size_t GetSplitExecutionSize() override;
		virtual const char* GetName() { return "GraphicsPlannerModule"; }

		void RecRequestSplit();
		void RecPushDebug(const char* name);
		void RecPopDebug();
		void RecSetTextureState(const Texture* target, D3D12_RESOURCE_STATES currentState, D3D12_RESOURCE_STATES nextState);
		void RecSetBufferState(const Buffer* target, D3D12_RESOURCE_STATES currentState, D3D12_RESOURCE_STATES nextState);
		void RecSetRenderPass(const RenderPass* target, bool ignoreLoadAction = false);
		void RecUpdateBuffer(const Buffer* target, uint32 targetOffset, Range<uint8> data);
		void RecCopyBuffer(const Buffer* src, const Buffer* dst, size_t size);
		void RecPresent(const SwapChain* swapchain);
		void RecDraw(const DrawDesc& target);
		void RecSetHeap(const DescriptorHeap** heap);

		void Reset();
		ID3D12CommandQueue* GetDirectQueue();
		uint64 GetRecordingBufferIndex();
		uint64 GetCompletedBufferIndex();

	private:
		inline Directx12::CmdBuffer* ContinueRecording();
		inline void SplitRecording();
		inline bool ExecuteCommand(const ExecutionContext& context, Directx12::CmdBuffer* buffer, CommandCode commandCode);

	private:
		CmdQueue* directQueue;
		CmdAllocatorPool* directAllocatorPool;
		List<Directx12::CmdBuffer*> recordedCmdBuffers;

		struct RecingOptimizer
		{
			inline void MarSetRenderPass(RenderPass* renderPass) { lastRenderPass = renderPass; drawCount = 0; }
			inline void MarDraw() { drawCount++; }
			inline void MarSetHeap(DescriptorHeap** heaps) { memcpy((void*) lastHeaps, (void*) heaps, sizeof(DescriptorHeap*) * Enum::ToUnderlying(HeapType::Count)); }
			inline bool ShouldSplitRecording() { return drawCount == 500; }

			RenderPass* lastRenderPass;
			DescriptorHeap* lastHeaps[Enum::ToUnderlying(HeapType::Count)];
			size_t drawCount;
		};
		RecingOptimizer recordingOptimizer;

		struct DrawOptimizer
		{
			DrawOptimizer() { Clear(); }
			void Clear()
			{
				lastPipeline = nullptr;
				lastVertexBuffer = nullptr; memset(rootArguments, 0, sizeof(UINT64) * 30);
			}
			const IShaderPipeline* lastPipeline;
			const IBuffer* lastVertexBuffer;
			UINT64 rootArguments[30];

		};
		List<DrawOptimizer> drawOptimizers;

		GraphicsExecutorModule* executor;
		ID3D12CommandAllocator* commandAllocator;
		ID3D12Device* device;
	};
}