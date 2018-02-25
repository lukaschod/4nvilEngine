#pragma once

#include <Graphics\IGraphicsModule.h>
#include <Foundation\MemoryModule.h>
#include <Windows\Views\ViewModule.h>
#include <Windows\Graphics\Directx12\Common.h>
#include <Windows\Graphics\Directx12\DescriptorHeap.h>
#include <Windows\Graphics\Directx12\BufferHeap.h>

#if !defined(ENABLED_D12_DEBUG_LAYER) && defined(ENABLED_DEBUG)
#define ENABLED_D12_DEBUG_LAYER
#endif

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
		Filter(const FilterOptions& options)
			: IFilter(options)
		{
		}
		HeapMemory srvMemory;
	};

	struct Texture : public ITexture
	{
		Texture(uint32_t width, uint32_t height)
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

	struct Buffer : public IBuffer
	{
		Buffer(size_t size)
			: IBuffer(size)
			, resource(nullptr)
			, currentState(D3D12_RESOURCE_STATE_COPY_DEST)
			, resourceMappedPointer(nullptr)
		{
		}

		HeapMemory memory;
		ID3D12Resource* resource;
		D3D12_RESOURCE_STATES currentState;
		D3D12_GPU_VIRTUAL_ADDRESS cachedResourceGpuVirtualAddress;
		uint8_t* resourceMappedPointer;
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

		void GetCounts(uint32_t* srvCount, uint32_t* samplersCount) const
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
			subData = new uint64_t(memory.size);
			memset(subData, 0, sizeof(uint64_t) * memory.size);
		}

		RootArgument(uint64_t data) :
			IsCurrentlyUsedByDraw(false)
		{
			subData = (uint64_t*) data;
		}

		HeapMemory memory;
		uint64_t* subData;
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
		SwapChain(const IView* view) :
			ISwapChain(view),
			bacBufferIndex(0),
			bacBuffers(nullptr),
			IDXGISwapChain3(nullptr)
		{
		}

		inline IDXGISwapChain3* GetIDXGISwapChain3() const { return IDXGISwapChain3; }
		inline Texture* GetBacBuffer() const { return bacBuffers[bacBufferIndex]; }

		Texture** bacBuffers;
		uint32_t bacBufferIndex;
		uint32_t width;
		uint32_t height;

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
		GraphicsModule();
		virtual void Execute(const ExecutionContext& context) override;
		virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
		virtual const char* GetName() { return "Directx12::GraphicsModule"; }
		virtual const IBuffer* AllocateBuffer(size_t size) override;
		virtual const ITexture* AllocateTexture(uint32_t width, uint32_t height) override;
		virtual const ISwapChain* AllocateSwapChain(const IView* view) override;

	public:
		virtual const ITexture* RecCreateITexture(const ExecutionContext& context, uint32_t width, uint32_t height, const ITexture* texture) override;

		virtual const IFilter* RecCreateIFilter(const ExecutionContext& context, const FilterOptions& options) override;

		virtual const IRenderPass* RecCreateIRenderPass(const ExecutionContext& context) override;
		virtual void RecSetColorAttachment(const ExecutionContext& context, const IRenderPass* target, uint32_t index, const ColorAttachment& attachment) override;
		virtual void RecSetDepthAttachment(const ExecutionContext& context, const IRenderPass* target, const DepthAttachment& attachment) override;
		virtual void RecSetViewport(const ExecutionContext& context, const IRenderPass* target, const Viewport& viewport) override;
		virtual void RecSetRenderPass(const ExecutionContext& context, const IRenderPass* target) override;

		virtual const IShaderPipeline* RecCreateIShaderPipeline(const ExecutionContext& context, const ShaderPipelineDesc* desc) override;
		virtual const IShaderArguments* RecCreateIShaderArguments(const ExecutionContext& context, const IShaderPipeline* pipeline) override;
		virtual void RecSetTexture(const ExecutionContext& context, const IShaderArguments* properties, const char* name, const ITexture* texture) override;
		virtual void RecSetFilter(const ExecutionContext& context, const IShaderArguments* properties, const char* name, const IFilter* filter) override;
		virtual void RecSetBuffer(const ExecutionContext& context, const IShaderArguments* properties, const char* name, const IBuffer* buffer) override;

		virtual const IBuffer* RecCreateIBuffer(const ExecutionContext& context, size_t size, const IBuffer* buffer = nullptr) override;
		virtual void RecUpdateBuffer(const ExecutionContext& context, const IBuffer* target, void* data, size_t size) override;

		virtual const ISwapChain* RecCreateISwapChain(const ExecutionContext& context, const IView* view, const ISwapChain* swapChain = nullptr) override;
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
		uint32_t GetSize(ColorFormat format);

		inline void Present(const ExecutionContext& context, SwapChain* swapChain, Texture* offscreen);
		inline void BlitCopy(const ExecutionContext& context, Texture* src, Texture* dest);
		inline void SetTextureState(const ExecutionContext& context, Texture* target, D3D12_RESOURCE_STATES state);
		inline void SetBufferState(const ExecutionContext& context, Buffer* target, D3D12_RESOURCE_STATES state);
		inline void SetBuffer(ShaderArguments* target, const char* name, const Buffer* buffer);
		inline void SetTexture(ShaderArguments* target, const char* name, const Texture* texture);
		inline void SetFilter(ShaderArguments* target, const char* name, const Filter* filter);
		inline void SetColorAttachment(const ExecutionContext& context, RenderPass* target, uint32_t index, const ColorAttachment& attachment);
		inline void SetDepthAttachment(const ExecutionContext& context, RenderPass* target, const DepthAttachment& attachment);
		inline void SetViewport(const ExecutionContext& context, RenderPass* target, const Viewport& viewport);
		inline void SetRenderPass(const ExecutionContext& context, const RenderPass* target);
		inline void UpdateBuffer(Buffer* target, uint32_t targetOffset, Range<uint8_t> data);
		inline void Draw(const ExecutionContext& context, const DrawDesc& target);
		inline void SetName(ID3D12Object* object, const wchar_t* format, ...);

	private:
		GraphicsPlannerModule* planner;
		DescriptorHeap* srvHeap;
		DescriptorHeap* rtvHeap;
		DescriptorHeap* samplersHeap;
		BufferHeap* bufferUploadHeap;
		List<std::pair<uint64_t, HeapMemory>> srvHeapMemoryToFree;
		List<std::pair<uint64_t, HeapMemory>> samplersHeapMemoryToFree;
		uint64_t resourceCounter;
		DescriptorHeap* srvCpuHeap;
		DescriptorHeap* samplersCpuHeap;

		ID3D12Device* device;
		IDXGIFactory4* factory;
		D3D12_FEATURE_DATA_ROOT_SIGNATURE rootSignatureFeatures;

		MemoryModule* memoryModule;
	};
}