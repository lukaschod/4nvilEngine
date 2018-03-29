/*
* Copyright (c) Lukas Chodosevicius
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#pragma once

#include <Core\Graphics\IGraphicsModule.hpp>
#include <Core\Foundation\MemoryModule.hpp>
#include <Windows\Views\ViewModule.hpp>
#include <Windows\Graphics\Directx12\Common.hpp>
#include <Windows\Graphics\Directx12\DescriptorHeap.hpp>
#include <Windows\Graphics\Directx12\BufferHeap.hpp>

using namespace Core::Math;
using namespace Core::Graphics;

namespace Windows::Directx12
{
	struct RenderPass : public IRenderPass
	{
		RenderPass() : IRenderPass()
		{
			memset(colorDescriptors, 0, sizeof(D3D12_CPU_DESCRIPTOR_HANDLE));
			colorDescriptorsCount = 0;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE colorDescriptors[COLOR_ATTACHMENT_MAX_COUNT];
		D3D12_CPU_DESCRIPTOR_HANDLE depthDescriptor;
		size_t colorDescriptorsCount;
		D3D12_VIEWPORT d12Viewport;
		D3D12_RECT d12ScissorRect;
	};

	struct Filter : public IFilter
	{
		Filter()
		{
		}
		HeapMemory srvMemory;
	};

	struct Texture : public ITexture
	{
		Texture(uint32 width, uint32 height)
			: ITexture(width, height)
			, resource(nullptr)
			, currentState(D3D12_RESOURCE_STATE_COPY_DEST)
		{
		}

		ID3D12Resource* resource;
		HeapMemory srvMemory;
		HeapMemory rtvMemory;
		D3D12_RESOURCE_STATES currentState;
	};

	struct ResourceMemory
	{
		ResourceMemory()
			: memory(0, 0)
			, resource(nullptr)
			, state(nullptr)
			, cachedMappedPointer(nullptr)
			, cachedGpuVirtualAddress(0)
		{ }

		void SetState(D3D12_RESOURCE_STATES& newState)
		{
			state = (memory.address == 0) ? (D3D12_RESOURCE_STATES*) newState : &newState;
		}

		D3D12_RESOURCE_STATES GetState()
		{
			return (memory.address == 0) ? static_cast<D3D12_RESOURCE_STATES>((uint64)state) : *state;
		}

		HeapMemory memory;
		ID3D12Resource* resource;
		D3D12_RESOURCE_STATES* state;
		uint8* cachedMappedPointer;
		D3D12_GPU_VIRTUAL_ADDRESS cachedGpuVirtualAddress;
	};

	struct SrvMemory
	{
		HeapMemory memory;
	};

	struct RtvMemory
	{
		HeapMemory memory;
	};

	struct Buffer : public IBuffer
	{
		Buffer(size_t size)
			: IBuffer(size)
			, resource(nullptr)
			//, currentState(D3D12_RESOURCE_STATE_COPY_DEST)
			, resourceMappedPointer(nullptr)
			, resourceOffset(0)
		{
		}

		void SetState(D3D12_RESOURCE_STATES newState)
		{
			if (memory.address == 0)
				state = (void*) newState;
			else
				*(D3D12_RESOURCE_STATES*) state = newState;
		}

		D3D12_RESOURCE_STATES GetState() const
		{
			return (memory.address == 0) ? (D3D12_RESOURCE_STATES)(uint64)state : *(D3D12_RESOURCE_STATES*)state;
		}

		HeapMemory memory;
		ID3D12Resource* resource;
		void* state;
		//D3D12_RESOURCE_STATES currentState;
		D3D12_GPU_VIRTUAL_ADDRESS cachedResourceGpuVirtualAddress;
		uint8* resourceMappedPointer;
		uint64 resourceOffset;
	};

	struct RootSubParamter
	{
		RootSubParamter(const char* name, D3D12_DESCRIPTOR_RANGE_TYPE type, bool isTexture = false)
			: name(name)
			, type(type)
			, isTexture(isTexture)
		{
		}
		const D3D12_DESCRIPTOR_RANGE_TYPE type;
		const char* const name;
		const bool isTexture;
	};

	enum class RootParamterType
	{
		TableSRV,
		TableSamplers,
		ConstantBuffer,
	};

	struct RootParamter
	{
		RootParamter() {}
		RootParamter(RootParamterType type) : type(type) {}

		static RootParamter AsConstantBuffer(const char* name)
		{
			RootParamter parameter(RootParamterType::ConstantBuffer);
			parameter.supParameters.push_back(RootSubParamter(name, (D3D12_DESCRIPTOR_RANGE_TYPE) -1));
			return parameter;
		}

		void GetCounts(uint32* srvCount, uint32* samplersCount) const
		{
			for (auto& supParameter : supParameters)
			{
				switch (supParameter.type)
				{
				case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
					if (srvCount != nullptr)
						(*srvCount)++;
					break;
				case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER:
					if (samplersCount != nullptr)
						(*samplersCount)++;
					break;
				}
			}
		}

		RootParamterType type;
		List<RootSubParamter> supParameters;
	};

	struct ShaderPipeline : public IShaderPipeline
	{
		ShaderPipeline(const ShaderPipelineDesc* desc)
			: IShaderPipeline(desc)
		{
		}

		RootParamter& FindRootParameter(RootParamterType type)
		{
			for (auto& rootParameter : rootParameters)
			{
				if (rootParameter.type == type)
					return rootParameter;
			}
			rootParameters.push_back(RootParamter(type));
			return rootParameters.back();
		}

		List<RootParamter> rootParameters;
		ID3D12PipelineState* pipelineState;
		ID3D12RootSignature* rootSignature;
		D3D12_VERTEX_BUFFER_VIEW vertexBuffer;
	};

	struct RootArgument
	{
		RootArgument(HeapMemory& memory)
			: memory(memory)
			, IsCurrentlyUsedByDraw(false)
		{
			subData = new uint64(memory.size);
			memset(subData, 0, sizeof(uint64) * memory.size);
		}

		RootArgument(uint64 data) :
			IsCurrentlyUsedByDraw(false)
		{
			subData = (uint64*) data;
		}

		HeapMemory memory;
		uint64* subData;
		bool IsCurrentlyUsedByDraw;
	};

	struct ShaderArguments : public IShaderArguments
	{
		ShaderArguments(const IShaderPipeline* pipeline) :
			IShaderArguments(pipeline)
		{
		}

		List<RootArgument> rootArguments;
	};

	struct SwapChain : public ISwapChain
	{
		SwapChain(const IView* view) 
			: ISwapChain(view)
			, bacBufferIndex(0)
			, bacBuffers(nullptr)
			, IDXGISwapChain3(nullptr)
		{
		}

		inline IDXGISwapChain3* GetIDXGISwapChain3() const { return IDXGISwapChain3; }
		inline Texture* GetBacBuffer() const { return bacBuffers[bacBufferIndex]; }

		Texture** bacBuffers;
		uint32 bacBufferIndex;
		uint32 width;
		uint32 height;

		IDXGISwapChain3* IDXGISwapChain3;
	};

	struct BlitCopyDesc : public DrawDesc
	{
		RenderPass* renderPass;
		Filter* filter;
	};

	class GraphicsPlannerModule;

	class GraphicsModule : public IGraphicsModule
	{
	public:
		BASE_IS(IGraphicsModule);

		GraphicsModule();
		virtual void Execute(const ExecutionContext& context) override;
		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
		virtual const char* GetName() { return "Directx12::GraphicsModule"; }
		virtual const IBuffer* AllocateBuffer(size_t size) override;
		virtual const ITexture* AllocateTexture(uint32 width, uint32 height) override;
		virtual const IFilter* AllocateFilter() override;
		virtual const ISwapChain* AllocateSwapChain(const IView* view) override;
		virtual const IRenderPass* AllocateRenderPass() override;

	public:
		virtual void RecCreateITexture(const ExecutionContext& context, const ITexture* target) override;

		virtual void RecCreateIFilter(const ExecutionContext& context, const IFilter* target) override;

		virtual void RecCreateIRenderPass(const ExecutionContext& context, const IRenderPass* target) override;
		virtual void RecSetColorAttachment(const ExecutionContext& context, const IRenderPass* target, uint32 index, const ColorAttachment& attachment) override;
		virtual void RecSetDepthAttachment(const ExecutionContext& context, const IRenderPass* target, const DepthAttachment& attachment) override;
		virtual void RecSetViewport(const ExecutionContext& context, const IRenderPass* target, const Viewport& viewport) override;
		virtual void RecSetRenderPass(const ExecutionContext& context, const IRenderPass* target) override;

		virtual const IShaderPipeline* RecCreateIShaderPipeline(const ExecutionContext& context, const ShaderPipelineDesc* desc) override;
		virtual const IShaderArguments* RecCreateIShaderArguments(const ExecutionContext& context, const IShaderPipeline* pipeline) override;
		virtual void RecSetTexture(const ExecutionContext& context, const IShaderArguments* properties, const char* name, const ITexture* texture) override;
		virtual void RecSetFilter(const ExecutionContext& context, const IShaderArguments* properties, const char* name, const IFilter* filter) override;
		virtual void RecSetBuffer(const ExecutionContext& context, const IShaderArguments* properties, const char* name, const IBuffer* buffer) override;

		virtual void RecCreateIBuffer(const ExecutionContext& context, const IBuffer* target) override;
		virtual void RecSetBufferUsage(const ExecutionContext& context, const IBuffer* target, BufferUsageFlags usage) override;
		virtual void RecUpdateBuffer(const ExecutionContext& context, const IBuffer* target, void* data, size_t size) override;
		virtual void RecCopyBuffer(const ExecutionContext& context, const IBuffer* src, const IBuffer* dst, size_t size) override;

		virtual void RecCreateISwapChain(const ExecutionContext& context, const ISwapChain* target) override;
		virtual void RecPresent(const ExecutionContext& context, const ISwapChain* swapchain, const ITexture* offscreen) override;
		virtual void RecFinalBlit(const ExecutionContext& context, const ISwapChain* swapchain, const ITexture* offscreen) override;

		virtual void RecPushDebug(const ExecutionContext& context, const char* name) override;
		virtual void RecPopDebug(const ExecutionContext& context) override;

		virtual void RecDraw(const ExecutionContext& context, const DrawDesc& target) override;

	protected:
		virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

	private:
		inline bool Initialize();
		inline void InitializeSwapCain(SwapChain* swapChain);
		inline void InitializeTexture(Texture* texure);
		inline void InitializeFilter(Filter* filter);
		inline void InitializeRenderPass(RenderPass* renderPass);
		inline void InitializeBuffer(Buffer* buffer);
		inline void InitializePipeline(ShaderPipeline* pipeleine);
		inline void CompilePipeline(ShaderPipeline* pipeleine);
		inline void InitializeProperties(ShaderArguments* target);
		inline void InitializeBlitCopy(BlitCopyDesc* target);

		DXGI_FORMAT Convert(ColorFormat format);
		const char* Convert(VertexAttributeType type);
		uint32 GetSize(ColorFormat format);

		inline void Present(const ExecutionContext& context, SwapChain* swapChain, Texture* offscreen);
		inline void BlitCopy(const ExecutionContext& context, Texture* src, Texture* dest);
		inline void SetTextureState(const ExecutionContext& context, Texture* target, D3D12_RESOURCE_STATES state);
		inline void SetBufferState(const ExecutionContext& context, Buffer* target, D3D12_RESOURCE_STATES state);
		inline void SetBuffer(ShaderArguments* target, const char* name, const Buffer* buffer);
		inline void SetTexture(ShaderArguments* target, const char* name, const Texture* texture);
		inline void SetFilter(ShaderArguments* target, const char* name, const Filter* filter);
		inline void SetColorAttachment(const ExecutionContext& context, RenderPass* target, uint32 index, const ColorAttachment& attachment);
		inline void SetDepthAttachment(const ExecutionContext& context, RenderPass* target, const DepthAttachment& attachment);
		inline void SetViewport(const ExecutionContext& context, RenderPass* target, const Viewport& viewport);
		inline void SetRenderPass(const ExecutionContext& context, const RenderPass* target);
		inline void UpdateBuffer(Buffer* target, uint32 targetOffset, Range<uint8> data);
		inline void Draw(const ExecutionContext& context, const DrawDesc& target);
		inline void SetName(ID3D12Object* object, const wchar_t* format, ...);

	private:
		GraphicsPlannerModule* planner;
		DescriptorHeap* srvHeap;
		DescriptorHeap* rtvHeap;
		DescriptorHeap* samplersHeap;
		BufferHeap* bufferUploadHeap;
		BufferHeap* bufferDefaultHeap;
		List<std::pair<uint64, HeapMemory>> srvHeapMemoryToFree;
		List<std::pair<uint64, HeapMemory>> samplersHeapMemoryToFree;
		uint64 resourceCounter;
		DescriptorHeap* srvCpuHeap;
		DescriptorHeap* samplersCpuHeap;

		ID3D12Device* device;
		IDXGIFactory4* factory;
		D3D12_FEATURE_DATA_ROOT_SIGNATURE rootSignatureFeatures;

		MemoryModule* memoryModule;
	};
}