#include "pch.h"
#include "OMEngine/Graphics/RHI/RHI.hpp"
#include <OMLogger/Logger.hpp>
#include "OMEngine/Utils/GraphicsUtils.hpp"
#include "OMEngine/Utils/Utils.hpp"

namespace OM::Graphics::RHI
{
	RHI* RHI::GetInstance()
	{
		if (!_instance)
			_instance = new RHI();
		return _instance;
	}

	bool RHI::Initialize(HWND hwnd)
	{
		_frameIndex = 0;
		_viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, Utils::SCREEN_WIDTH, Utils::SCREEN_HEIGHT);
		_scissorRect = CD3DX12_RECT(0, 0, static_cast<LONG>(Utils::SCREEN_WIDTH), static_cast<LONG>(Utils::SCREEN_HEIGHT));
		_rtvDescriptorSize = 0;
		_aspectRatio = static_cast<float>(Utils::SCREEN_WIDTH) / static_cast<float>(Utils::SCREEN_HEIGHT);

		if (!InitializePipeline(hwnd))
		{
			OM_LOG_CRITICAL_TAG("Failed to initialize pipeline!", OM::Logger::TagRender);
			return false;
		}

		if (!InitializeAssets())
		{
			OM_LOG_CRITICAL_TAG("Failed to initialize assets!", OM::Logger::TagRender);
			return false;
		}

		return true;
	}

	void RHI::Render()
	{
		PopulateCommandList();

		if (!_commandList) { OM_LOG_CRITICAL_TAG("CommandList null.", OM::Logger::TagRender); return; }
		if (!_swapChain) { OM_LOG_CRITICAL_TAG("SwapChain null.", OM::Logger::TagRender); return; }

		ID3D12CommandList* ppCommandLists[] = { _commandList.Get() };
		_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		if (!CHECK_HRESULT(_swapChain->Present(1, 0), "Failed to present swap chain"))
			return;

		WaitForPreviousFrame();
	}

	void RHI::Destroy()
	{
		WaitForPreviousFrame();
		CloseHandle(_fenceEvent);
	}

	bool RHI::InitializePipeline(HWND hwnd)
	{
		unsigned int dxgiFactoryFlags = 0;

#if defined(_DEBUG)
		GetDebugInterface(&dxgiFactoryFlags);
#endif

		ComPtr<IDXGIFactory4> factory;
		if (!CHECK_HRESULT(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)), "Failed to create factory"))
			return false;

		if (!CreateDevice(factory))
			return false;

		if (!CreateQueue())
			return false;

		if (!CreateSwapChain(factory, hwnd))
			return false;

		_frameIndex = _swapChain->GetCurrentBackBufferIndex();

		if (!CreateRTV())
			return false;

		if (!CHECK_HRESULT(factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER), "Failed to disable Alt+Enter"))
			return false;

		return true;
	}

	bool RHI::InitializeAssets()
	{
		if (!CreateRootSignature())
			return false;

		if (!CreatePipelineState())
			return false;

		if (!CreateCommandList())
			return false;

		if (!CreateVertexBuffer())
			return false;

		if (!CreateFence())
			return false;

		WaitForPreviousFrame();
		return true;
	}

	void RHI::GetDebugInterface(unsigned int* dxgiFactoryFlags)
	{
		ComPtr<ID3D12Debug> debugInterface;

		if (CHECK_HRESULT(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)), "Failed to get D3D12 debug interface"))
		{
			debugInterface->EnableDebugLayer();
			*dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}

	bool RHI::CreateDevice(ComPtr<IDXGIFactory4> factory)
	{
		if (_useWarpDevice)
		{
			ComPtr<IDXGIAdapter> warpAdapter;
			if (!CHECK_HRESULT(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)), "Failed to enumerate WRAP adapter"))
				return false;

			if (!CHECK_HRESULT(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&_device)), "Failed to create device with WRAP"))
				return false;
		}
		else
		{
			ComPtr<IDXGIAdapter1> hardwareAdapter;
			GetHardwareAdapter(factory.Get(), &hardwareAdapter);
			if (!CHECK_HRESULT(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&_device)), "Failed to create device"))
				return false;
		}

		return true;
	}

	void RHI::GetHardwareAdapter(IDXGIFactory1* factory, IDXGIAdapter1** adapter, bool requestHighPerformanceAdapter)
	{
		*adapter = nullptr;
		ComPtr<IDXGIAdapter1> potentialAdapter;

		ComPtr<IDXGIFactory6> factory6;
		if (SUCCEEDED(factory->QueryInterface(IID_PPV_ARGS(&factory6))))
		{
			for (unsigned int adapterIndex = 0;
				SUCCEEDED(factory6->EnumAdapterByGpuPreference(
					adapterIndex,
					requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
					IID_PPV_ARGS(&potentialAdapter)));
					++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				potentialAdapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
					continue;

				if (SUCCEEDED(D3D12CreateDevice(potentialAdapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
					break;
			}
		}

		if (potentialAdapter.Get() == nullptr)
		{
			for (unsigned int adapterIndex = 0; SUCCEEDED(factory->EnumAdapters1(adapterIndex, &potentialAdapter)); ++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				potentialAdapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
					continue;

				if (SUCCEEDED(D3D12CreateDevice(potentialAdapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
					break;
			}
		}

		if (potentialAdapter.Get() == nullptr)
		{
			OM_LOG_ERROR_TAG("No compatible GPU found.", OM::Logger::TagRender);
		}

		*adapter = potentialAdapter.Detach();
	}

	bool RHI::CreateSwapChain(ComPtr<IDXGIFactory4> factory, HWND hwnd)
	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = _FRAME_COUNT;
		swapChainDesc.Width = Utils::SCREEN_WIDTH;
		swapChainDesc.Height = Utils::SCREEN_HEIGHT;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;

		ComPtr<IDXGISwapChain1> swapChain;

		if (!CHECK_HRESULT(factory->CreateSwapChainForHwnd(_commandQueue.Get(), hwnd, &swapChainDesc, nullptr, nullptr, &swapChain), "Failed to create swap chain"))
			return false;

		if (!CHECK_HRESULT(swapChain.As(&_swapChain), "Failed to convert swap chain"))
			return false;

		return true;
	}

	bool RHI::CreateRTV()
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = _FRAME_COUNT;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		if (!CHECK_HRESULT(_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&_rtvHeap)), "Failed to create render target view heap"))
			return false;

		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		if (!CHECK_HRESULT(_device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&_srvHeap)), "Failed to create shader ressource view heap"))
			return false;

		_rtvDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_rtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (unsigned int i = 0; i < _FRAME_COUNT; i++)
		{
			if (!CHECK_HRESULT(_swapChain->GetBuffer(i, IID_PPV_ARGS(&_renderTargets[i])), "Failed to get render target buffer"))
				return false;
			
			_device->CreateRenderTargetView(_renderTargets[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, _rtvDescriptorSize);
		}

		return true;
	}

	bool RHI::CreateQueue()
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		if (!CHECK_HRESULT(_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_commandQueue)), "Failed to create command queue"))
			return false;

		if (!CHECK_HRESULT(_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_commandAllocator)), "Failed to create command allocator"))
			return false;

		return true;
	}

	bool RHI::CreateCommandList()
	{
		if (!CHECK_HRESULT(_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocator.Get(), _pipelineState.Get(), IID_PPV_ARGS(&_commandList)), "Failed to create command list"))
			return false;

		return true;
	}

	void RHI::PopulateCommandList()
	{
		if (!_commandAllocator) { OM_LOG_CRITICAL_TAG("CommandAllocator null.", OM::Logger::TagRender); return; }
		if (!_commandList) { OM_LOG_CRITICAL_TAG("CommandList null.", OM::Logger::TagRender); return; }
		if (!_pipelineState) { OM_LOG_CRITICAL_TAG("PipelineState null.", OM::Logger::TagRender); return; }
		if (!_rootSignature) { OM_LOG_CRITICAL_TAG("RootSignature null.", OM::Logger::TagRender); return; }
		if (!_renderTargets) { OM_LOG_CRITICAL_TAG("RenderTargets null.", OM::Logger::TagRender); return; }
		if (!_rtvHeap) { OM_LOG_CRITICAL_TAG("RtvHeap null.", OM::Logger::TagRender); return; }

		if (!CHECK_HRESULT(_commandAllocator->Reset(), "Failed to reset command allocator"))
			return;

		if (!CHECK_HRESULT(_commandList->Reset(_commandAllocator.Get(), _pipelineState.Get()), "Failed to reset command list"))
			return;

		_commandList->SetGraphicsRootSignature(_rootSignature.Get());
		ID3D12DescriptorHeap* ppHeaps[] = { _srvHeap.Get() };
		_commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
		_commandList->SetGraphicsRootDescriptorTable(0, _srvHeap->GetGPUDescriptorHandleForHeapStart());
		_commandList->RSSetViewports(1, &_viewport);
		_commandList->RSSetScissorRects(1, &_scissorRect);

		CD3DX12_RESOURCE_BARRIER barrierToRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(_renderTargets[_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		_commandList->ResourceBarrier(1, &barrierToRenderTarget);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_rtvHeap->GetCPUDescriptorHandleForHeapStart(), _frameIndex, _rtvDescriptorSize);
		_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

		const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		_commandList->IASetVertexBuffers(0, 1, &_vertexBufferView);
		_commandList->DrawInstanced(3, 1, 0, 0);

		auto barrierToPresent = CD3DX12_RESOURCE_BARRIER::Transition(_renderTargets[_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		_commandList->ResourceBarrier(1, &barrierToPresent);

		if (!CHECK_HRESULT(_commandList->Close(), "Failed to close command list"))
			return;
	}

	bool RHI::CreateRootSignature()
	{
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
		if (!CHECK_HRESULT(_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData)), "device doesn't support 1.1 version"))
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;

		CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
		ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

		CD3DX12_ROOT_PARAMETER1 rootParameters[1];
		rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);

		D3D12_STATIC_SAMPLER_DESC sampler = {};
		sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler.MipLODBias = 0;
		sampler.MaxAnisotropy = 0;
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		sampler.MinLOD = 0.0f;
		sampler.MaxLOD = D3D12_FLOAT32_MAX;
		sampler.ShaderRegister = 0;
		sampler.RegisterSpace = 0;
		sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;

		if (!CHECK_HRESULT(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error), "Root signature serialize failed"))
			return false;

		if (!CHECK_HRESULT(_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&_rootSignature)), "Failed to create root signature"))
			return false;

		return true;
	}

	bool RHI::CreatePipelineState()
	{
		ComPtr<ID3DBlob> vertexShader;
		ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
		unsigned int compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		unsigned int compileFlags = 0;
#endif

		if (!CHECK_HRESULT(D3DCompileFromFile(L"Assets/Shaders/shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr), "Failed to compile vertex shader"))
			return false;

		if (!CHECK_HRESULT(D3DCompileFromFile(L"Assets/Shaders/shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr), "Failed to compile pixel shader"))
			return false;

		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
		psoDesc.pRootSignature = _rootSignature.Get();
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState.DepthEnable = FALSE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;

		if (!CHECK_HRESULT(_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&_pipelineState)), "Failed to create graphics pipeline"))
			return false;

		return true;
	}

	bool RHI::CreateVertexBuffer()
	{
		// TODO: Temp
		Vertex triangleVertices[] =
		{
			{ { 0.0f,	 0.25f * _aspectRatio, 0.0f }, { 0.5f, 0.0f } },
			{ { 0.25f,  -0.25f * _aspectRatio, 0.0f }, { 1.0f, 1.0f } },
			{ { -0.25f, -0.25f * _aspectRatio, 0.0f }, { 0.0f, 1.0f } }
		};

		const unsigned int vertexBufferSize = sizeof(triangleVertices);

		CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);

		if (!CHECK_HRESULT(_device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&_vertexBuffer)), "Failed to create committed resource"))
			return false;

		unsigned __int8* pVertexDataBegin;
		CD3DX12_RANGE readRange(0, 0);

		if (!CHECK_HRESULT(_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)), "Failed to cast vertex data"))
			return false;

		memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
		_vertexBuffer->Unmap(0, nullptr);

		_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
		_vertexBufferView.StrideInBytes = sizeof(Vertex);
		_vertexBufferView.SizeInBytes = vertexBufferSize;

		D3D12_RESOURCE_DESC textureDesc = {};
		textureDesc.MipLevels = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.Width = _TEXTURE_WIDTH;
		textureDesc.Height = _TEXTURE_HEIGHT;
		textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		textureDesc.DepthOrArraySize = 1;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		CD3DX12_HEAP_PROPERTIES heapType = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		if (!CHECK_HRESULT(_device->CreateCommittedResource(&heapType, D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&_texture)), "Failed to create the texture"))
			return false;

		const unsigned __int64 uploadBufferSize = GetRequiredIntermediateSize(_texture.Get(), 0, 1);
		heapType = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
		if (!CHECK_HRESULT(_device->CreateCommittedResource(&heapType, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&_textureUploadHeap)), "Failed to create the GPU upload buffer"))
			return false;

		std::vector<unsigned __int8> texture = GenerateTextureData();

		D3D12_SUBRESOURCE_DATA textureData = {};
		textureData.pData = &texture[0];
		textureData.RowPitch = _TEXTURE_WIDTH * _TEXTURE_PIXEL_SIZE;
		textureData.SlicePitch = textureData.RowPitch * _TEXTURE_HEIGHT;

		UpdateSubresources(_commandList.Get(), _texture.Get(), _textureUploadHeap.Get(), 0, 0, 1, &textureData);
		CD3DX12_RESOURCE_BARRIER barrierToShaderResource = CD3DX12_RESOURCE_BARRIER::Transition(_texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		_commandList->ResourceBarrier(1, &barrierToShaderResource);

		// Describe and create a SRV for the texture.
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		_device->CreateShaderResourceView(_texture.Get(), &srvDesc, _srvHeap->GetCPUDescriptorHandleForHeapStart());

		if (!CHECK_HRESULT(_commandList->Close(), "Failed to close command list"))
			return false;

		ID3D12CommandList* ppCommandLists[] = { _commandList.Get() };
		_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		return true;
	}

	bool RHI::CreateFence()
	{
		if (!CHECK_HRESULT(_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence)), "Failed to create fence"))
			return false;

		_fenceValue = 1;

		_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (_fenceEvent == nullptr)
		{
			if (!CHECK_HRESULT(HRESULT_FROM_WIN32(GetLastError()), "Fence is nullptr"))
				return false;
		}

		return true;
	}

	void RHI::WaitForPreviousFrame()
	{
		if (!_commandQueue) { OM_LOG_CRITICAL_TAG("CommandQueue null.", OM::Logger::TagRender); return; }
		if (!_fence) { OM_LOG_CRITICAL_TAG("Fence null.", OM::Logger::TagRender); return; }
		if (!_swapChain) { OM_LOG_CRITICAL_TAG("SwapChain null.", OM::Logger::TagRender); return; }

		const unsigned __int64 fence = _fenceValue;
		if (!CHECK_HRESULT(_commandQueue->Signal(_fence.Get(), fence), "Fence signal failed"))
			return;

		_fenceValue++;

		if (_fence->GetCompletedValue() < fence)
		{
			if (!CHECK_HRESULT(_fence->SetEventOnCompletion(fence, _fenceEvent), "Fence event completion failed"))
				return;

			WaitForSingleObject(_fenceEvent, INFINITE);
		}

		_frameIndex = _swapChain->GetCurrentBackBufferIndex();
	}

	std::vector<unsigned __int8> RHI::GenerateTextureData()
	{
		const unsigned int rowPitch = _TEXTURE_WIDTH * _TEXTURE_PIXEL_SIZE;
		const unsigned int cellPitch = rowPitch >> 3;        
		const unsigned int cellHeight = _TEXTURE_WIDTH >> 3;    
		const unsigned int textureSize = rowPitch * _TEXTURE_HEIGHT;

		std::vector<unsigned __int8> data(textureSize);
		unsigned __int8* pData = &data[0];

		for (unsigned int n = 0; n < textureSize; n += _TEXTURE_PIXEL_SIZE)
		{
			unsigned int x = n % rowPitch;
			unsigned int y = n / rowPitch;
			unsigned int i = x / cellPitch;
			unsigned int j = y / cellHeight;

			if (i % 2 == j % 2)
			{
				pData[n] = 0x00;        // R
				pData[n + 1] = 0x00;    // G
				pData[n + 2] = 0x00;    // B
				pData[n + 3] = 0xff;    // A
			}
			else
			{
				pData[n] = 0xff;        // R
				pData[n + 1] = 0xff;    // G
				pData[n + 2] = 0xff;    // B
				pData[n + 3] = 0xff;    // A
			}
		}

		return data;
	}
}