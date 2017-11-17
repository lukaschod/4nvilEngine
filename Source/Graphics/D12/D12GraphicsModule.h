#pragma once

#include <Graphics\D12\D12Common.h>
#include <Graphics\IGraphicsModule.h>
#include <WindowsPlayer\Views\WinViewModule.h>
#include <Graphics\D12\D12Heap.h>

struct D12RenderPass : public IRenderPass
{
	D12RenderPass() :
		IRenderPass()
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

struct D12Filter : public IFilter
{
	D12Filter(const FilterOptions& options) : 
		IFilter(options)
	{
	}
	D12HeapMemory srvMemory;
};

struct D12Texture : public ITexture
{
	D12Texture(uint32_t width, uint32_t height) :
		ITexture(width, height),
		resource(nullptr),
		currentState(D3D12_RESOURCE_STATE_COPY_DEST)
	{
	}

	ID3D12Resource* resource;
	D12HeapMemory srvMemory;
	D12HeapMemory rtvMemory;
	D3D12_RESOURCE_STATES currentState;
};

struct D12Buffer : public IBuffer
{
	D12Buffer(size_t size) : 
		IBuffer(size),
		resource(nullptr),
		currentState(D3D12_RESOURCE_STATE_COPY_DEST)
	{}

	ID3D12Resource* resource;
	D3D12_RESOURCE_STATES currentState;
};

struct D12RootSubParamter
{
	D12RootSubParamter(const char* name, D3D12_DESCRIPTOR_RANGE_TYPE type, bool isTexture = false) :
		name(name),
		type(type),
		isTexture(isTexture)
	{ }
	const D3D12_DESCRIPTOR_RANGE_TYPE type;
	const char* const name;
	const bool isTexture;
};

enum D12RootParamterType
{
	D12RootParamterTypeTableSRV,
	D12RootParamterTypeTableSamplers,
	D12RootParamterTypeConstantBuffer,
};

struct D12RootParamter
{
	D12RootParamter() {}
	D12RootParamter(D12RootParamterType type) : type(type) {}

	static D12RootParamter AsConstantBuffer(const char* name)
	{
		D12RootParamter parameter(D12RootParamterTypeConstantBuffer);
		parameter.supParameters.push_back(D12RootSubParamter(name, (D3D12_DESCRIPTOR_RANGE_TYPE)-1));
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

	D12RootParamterType type;
	List<D12RootSubParamter> supParameters;
};

struct D12ShaderPipeline : public IShaderPipeline
{
	D12ShaderPipeline(const ShaderPipelineDesc* desc) :
		IShaderPipeline(desc)
	{
	}

	D12RootParamter& FindRootParameter(D12RootParamterType type)
	{
		for (auto& rootParameter : rootParameters)
		{
			if (rootParameter.type == type)
				return rootParameter;
		}
		rootParameters.push_back(D12RootParamter(type));
		return rootParameters.back();
	}

	List<D12RootParamter> rootParameters;
	ID3D12PipelineState* pipelineState;
	ID3D12RootSignature* rootSignature;
	D3D12_VERTEX_BUFFER_VIEW vertexBuffer;
};

struct D12RootArgument
{
	D12RootArgument(D12HeapMemory& memory) :
		memory(memory),
		IsCurrentlyUsedByDraw(false)
	{
		subData = new uint64_t(memory.size);
		memset(subData, 0, sizeof(uint64_t) * memory.size);
	}

	D12RootArgument(uint64_t data) :
		IsCurrentlyUsedByDraw(false)
	{
		subData = (uint64_t*)data;
	}

	D12HeapMemory memory;
	uint64_t* subData;
	bool IsCurrentlyUsedByDraw;
};

struct D12ShaderArguments : public IShaderArguments
{
	D12ShaderArguments(const IShaderPipeline* pipeline) :
		IShaderArguments(pipeline) {}

	List<D12RootArgument> rootArguments;
};

struct D12SwapChain : public ISwapChain
{
	D12SwapChain(const IView* view) :
		ISwapChain(view),
		bacBufferIndex(0),
		bacBuffers(nullptr),
		IDXGISwapChain3(nullptr)
	{
	}

	inline IDXGISwapChain3* GetIDXGISwapChain3() const { return IDXGISwapChain3; }
	inline D12Texture* GetBacBuffer() const { return bacBuffers[bacBufferIndex]; }

	D12Texture** bacBuffers;
	uint32_t bacBufferIndex;
	uint32_t width;
	uint32_t height;

	IDXGISwapChain3* IDXGISwapChain3;
};

struct D12BlitCopy : public DrawDesc
{
	D12RenderPass* renderPass;
	D12Filter* filter;
};

class D12GraphicsPlannerModule;

class D12GraphicsModule : public IGraphicsModule
{
public:
	D12GraphicsModule(uint32_t bufferCount, uint32_t workersCount);
	virtual void Execute(const ExecutionContext& context) override;
	virtual void SetupExecuteOrder(ModuleManager* moduleManager) override;
	virtual const char* GetName() { return "D12GraphicsModule"; }

	virtual const ITexture* RecCreateITexture(const ExecutionContext& context, uint32_t width, uint32_t height) override;

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

	virtual const IBuffer* RecCreateIBuffer(const ExecutionContext& context, size_t size) override;
	virtual void RecUpdateBuffer(const ExecutionContext& context, const IBuffer* target, void* data, size_t size) override;

	virtual const ISwapChain* RecCreateISwapChain(const ExecutionContext& context, const IView* view) override;
	virtual void RecPresent(const ExecutionContext& context, const ISwapChain* swapchain, const ITexture* offscreen) override;
	virtual void RecFinalBlit(const ExecutionContext& context, const ISwapChain* swapchain, const ITexture* offscreen) override;

	virtual void RecPushDebug(const ExecutionContext& context, const char* name) override;
	virtual void RecPopDebug(const ExecutionContext& context) override;

	virtual void RecDraw(const ExecutionContext& context, const DrawDesc& target) override;

protected:
	virtual bool ExecuteCommand(const ExecutionContext& context, IOStream& stream, uint32_t commandCode) override;

private:
	inline bool Initialize();
	inline void InitializeSwapCain(D12SwapChain* swapChain);
	inline void InitializeTexture(D12Texture* texure);
	inline void InitializeFilter(D12Filter* filter);
	inline void InitializeRenderPass(D12RenderPass* renderPass);
	inline void InitializeBuffer(D12Buffer* buffer);
	inline void InitializePipeline(D12ShaderPipeline* pipeleine);
	inline void CompilePipeline(D12ShaderPipeline* pipeleine);
	inline void InitializeProperties(D12ShaderArguments* target);
	inline void InitializeBlitCopy(D12BlitCopy* target);

	DXGI_FORMAT Convert(ColorFormat format);
	const char* Convert(VertexAttributeType type);
	uint32_t GetSize(ColorFormat format);

	inline void Present(const ExecutionContext& context, D12SwapChain* swapChain, D12Texture* offscreen);
	inline void BlitCopy(const ExecutionContext& context, D12Texture* src, D12Texture* dest);
	inline void SetTextureState(const ExecutionContext& context, D12Texture* target, D3D12_RESOURCE_STATES state);
	inline void SetBufferState(const ExecutionContext& context, D12Buffer* target, D3D12_RESOURCE_STATES state);
	inline void SetBuffer(D12ShaderArguments* target, const char* name, const D12Buffer* buffer);
	inline void SetTexture(D12ShaderArguments* target, const char* name, const D12Texture* texture);
	inline void SetFilter(D12ShaderArguments* target, const char* name, const D12Filter* filter);
	inline void SetColorAttachment(const ExecutionContext& context, D12RenderPass* target, uint32_t index, const ColorAttachment& attachment);
	inline void SetDepthAttachment(const ExecutionContext& context, D12RenderPass* target, const DepthAttachment& attachment);
	inline void SetViewport(const ExecutionContext& context, D12RenderPass* target, const Viewport& viewport);
	inline void SetRenderPass(const ExecutionContext& context, const D12RenderPass* target);
	inline void UpdateBuffer(D12Buffer* target, uint32_t targetOffset, Range<uint8_t> data);
	inline void Draw(const ExecutionContext& context, const DrawDesc& target);
	inline void SetName(ID3D12Object* object, const wchar_t* format, ...);

private:
	D12GraphicsPlannerModule* planner;
	D12Heap* srvHeap;
	D12Heap* rtvHeap;
	D12Heap* samplersHeap;
	List<std::pair<uint64_t, D12HeapMemory>> srvHeapMemoryToFree;
	List<std::pair<uint64_t, D12HeapMemory>> samplersHeapMemoryToFree;
	uint64_t resourceCounter;
	D12Heap* srvCpuHeap;
	D12Heap* samplersCpuHeap;

	ID3D12Device* device;
	IDXGIFactory4* factory;
	D3D12_FEATURE_DATA_ROOT_SIGNATURE rootSignatureFeatures;
};