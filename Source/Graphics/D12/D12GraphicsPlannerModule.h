#pragma once

#include <Graphics\D12\D12Common.h>
#include <Graphics\D12\D12Heap.h>
#include <Graphics\D12\D12CmdAllocatorPool.h>
#include <Graphics\D12\D12CmdQueue.h>
#include <Graphics\IGraphicsModule.h>
#include <Modules\CmdModule.h>

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
	void RecDrawSimple(const DrawSimple& target);
	void RecSetHeap(const D12Heap** heap);

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
		inline void MarSetHeap(D12Heap** heaps) { memcpy((void*)lastHeaps, (void*)heaps, sizeof(D12Heap*) * D12HeapTypeCount); }
		inline bool ShouldSplitRecording() { return drawCount == 25; }
		
		D12RenderPass* lastRenderPass;
		D12Heap* lastHeaps[D12HeapTypeCount];
		size_t drawCount;
	};
	RecingOptimizer recordingOptimizer;

	D12GraphicsExecuterModule* executer;
	ID3D12CommandAllocator* commandAllocator;
	ID3D12Device* device;
};