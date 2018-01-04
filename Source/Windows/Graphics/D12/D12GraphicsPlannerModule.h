#pragma once

#include <Windows\Graphics\D12\D12Common.h>
#include <Windows\Graphics\D12\D12DescriptorHeap.h>
#include <Windows\Graphics\D12\D12CmdAllocatorPool.h>
#include <Windows\Graphics\D12\D12CmdQueue.h>
#include <Graphics\IGraphicsModule.h>
#include <Modules\PipeModule.h>

struct D12Texture;
struct D12Buffer;
struct D12SwapChain;
struct D12RenderPass;
struct D12ShaderArguments;
class D12GraphicsModule;
class D12GraphicsExecuterModule;

class D12GraphicsPlannerModule : public Module
{
public:
	D12GraphicsPlannerModule(ID3D12Device* device);
	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
	virtual void Execute(const ExecutionContext& context) override;
	virtual size_t GetExecutionSize() override;
	virtual size_t GetSplitExecutionSize(size_t currentSize) override;
	virtual const char* GetName() { return "D12GraphicsPlannerModule"; }

	void RecRequestSplit();
	void RecPushDebug(const char* name);
	void RecPopDebug();
	void RecSetTextureState(const D12Texture* target, D3D12_RESOURCE_STATES currentState, D3D12_RESOURCE_STATES nextState);
	void RecSetBufferState(const D12Buffer* target, D3D12_RESOURCE_STATES currentState, D3D12_RESOURCE_STATES nextState);
	void RecSetRenderPass(const D12RenderPass* target, bool ignoreLoadActions = false);
	void RecUpdateBuffer(const D12Buffer* target, uint32_t targetOffset, Range<uint8_t> data);
	void RecPresent(const D12SwapChain* swapchain);
	void RecDraw(const DrawDesc& target);
	void RecSetHeap(const D12DescriptorHeap** heap);

	void Reset();
	ID3D12CommandQueue* GetDirectQueue();
	uint64_t GetRecordingBufferIndex();
	uint64_t GetCompletedBufferIndex();

private:
	inline D12CmdBuffer* ContinueRecording();
	inline void SplitRecording();
	inline bool ExecuteCommand(const ExecutionContext& context, D12CmdBuffer* buffer, uint32_t commandCode);

private:
	D12CmdQueue* directQueue;
	D12CmdAllocatorPool* directAllocatorPool;
	List<D12CmdBuffer*> recordedCmdBuffers;

	struct RecingOptimizer
	{
		inline void MarSetRenderPass(D12RenderPass* renderPass) { lastRenderPass = renderPass; drawCount = 0; }
		inline void MarDraw() { drawCount++; }
		inline void MarSetHeap(D12DescriptorHeap** heaps) { memcpy((void*)lastHeaps, (void*)heaps, sizeof(D12DescriptorHeap*) * D12HeapTypeCount); }
		inline bool ShouldSplitRecording() { return drawCount == 500; }
		
		D12RenderPass* lastRenderPass;
		D12DescriptorHeap* lastHeaps[D12HeapTypeCount];
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

	D12GraphicsExecuterModule* executer;
	ID3D12CommandAllocator* commandAllocator;
	ID3D12Device* device;
};