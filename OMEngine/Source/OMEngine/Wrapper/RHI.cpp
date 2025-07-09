#include "pch.h"

#include "OMEngine/Wrapper/RHI.hpp"

#include <OMLogger/Logger.hpp>
#include "OMEngine/Utils.hpp"

namespace OM::Wrapper
{
	RHI* RHI::GetInstance()
	{
		if (!_instance)
			_instance = new RHI();
		return _instance;
	}

	bool RHI::Initialisation(HWND hwnd)
	{
		_frameIndex = 0;
		_viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, Utils::SCREEN_WIDTH, Utils::SCREEN_HEIGHT);
		_scissorRect = CD3DX12_RECT(0, 0, static_cast<LONG>(Utils::SCREEN_WIDTH), static_cast<LONG>(Utils::SCREEN_HEIGHT));
		_rtvDescriptorSize = 0;
		_aspectRatio = Utils::SCREEN_WIDTH / Utils::SCREEN_HEIGHT;

		if (!LoadPipeline(hwnd))
		{
			OM_LOG_CRITICAL_TAG("Failed to load pipeline!", OM::Logger::TagRender);
			return false;
		}

		if (!LoadAssets())
		{
			OM_LOG_CRITICAL_TAG("Failed to load assets!", OM::Logger::TagRender);
			return false;
		}

		return true;
	}

	void RHI::Render()
	{
		PopulateCommandList();

		ID3D12CommandList* ppCommandLists[] = { _commandList.Get() };
		_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		if (FAILED(_swapChain->Present(1, 0)))
		{
			OM_LOG_CRITICAL_TAG("Present the frame", OM::Logger::TagRender);
			return;
		}

		WaitForPreviousFrame();
	}

	void RHI::Destroy()
	{
		WaitForPreviousFrame();
		CloseHandle(_fenceEvent);
	}

	bool RHI::LoadPipeline(HWND hwnd)
	{
		UINT dxgiFactoryFlags = 0;

	#if defined(_DEBUG)
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	#endif

		ComPtr<IDXGIFactory4> factory;
		if (FAILED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory))))
		{
			OM_LOG_CRITICAL_TAG("Failed to create factory!", OM::Logger::TagRender);
			return false;
		}

		if (_useWarpDevice)
		{
			ComPtr<IDXGIAdapter> warpAdapter;
			if (FAILED(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter))))
			{
				OM_LOG_CRITICAL_TAG("Failed to give wrap adapter!", OM::Logger::TagRender);
				return false;
			}

			if (FAILED(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&_device))))
			{
				OM_LOG_CRITICAL_TAG("Failed to create device with wrap adapter!", OM::Logger::TagRender);
				return false;
			}
		}
		else
		{
			ComPtr<IDXGIAdapter1> hardwareAdapter;
			GetHardwareAdapter(factory.Get(), &hardwareAdapter);

			if (FAILED(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&_device))))
			{
				OM_LOG_CRITICAL_TAG("Failed to create device!", OM::Logger::TagRender);
				return false;
			}
		}

		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		if (FAILED(_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_commandQueue))))
		{
			OM_LOG_CRITICAL_TAG("Failed to create command queue!", OM::Logger::TagRender);
			return false;
		}

		// Describe and create the swap chain.
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = _frameCount;
		swapChainDesc.Width = Utils::SCREEN_WIDTH;
		swapChainDesc.Height = Utils::SCREEN_HEIGHT;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;

		ComPtr<IDXGISwapChain1> swapChain;
		if (FAILED(factory->CreateSwapChainForHwnd(_commandQueue.Get(), hwnd, &swapChainDesc, nullptr, nullptr, &swapChain)))
		{
			OM_LOG_CRITICAL_TAG("Failed to create swap chain (for hwnd)!", OM::Logger::TagRender);
			return false;
		}

		if (FAILED(factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER)))
		{
			OM_LOG_CRITICAL_TAG("This does not support fullscreen transitions", OM::Logger::TagRender);
			return false;
		}

		if (FAILED(swapChain.As(&_swapChain)))
		{
			OM_LOG_CRITICAL_TAG("Cast failed IDXGISwapChain1 to IDXGISwapChain3", OM::Logger::TagRender);
			return false;
		}

		_frameIndex = _swapChain->GetCurrentBackBufferIndex();

		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = _frameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		if (FAILED(_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&_rtvHeap))))
		{
			OM_LOG_CRITICAL_TAG("Failed to create descriptor heap", OM::Logger::TagRender);
			return false;
		}

		_rtvDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_rtvHeap->GetCPUDescriptorHandleForHeapStart());

			for (UINT n = 0; n < _frameCount; n++)
			{
				if (FAILED(_swapChain->GetBuffer(n, IID_PPV_ARGS(&_renderTargets[n]))))
				{
					OM_LOG_CRITICAL_TAG("Failed to get render target view buffer", OM::Logger::TagRender);
					return false;
				}
				_device->CreateRenderTargetView(_renderTargets[n].Get(), nullptr, rtvHandle);
				rtvHandle.Offset(1, _rtvDescriptorSize);
			}
		}

		if (FAILED(_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_commandAllocator))))
		{
			OM_LOG_CRITICAL_TAG("Failed to create command allocator", OM::Logger::TagRender);
			return false;
		}

		return true;
	}

	bool RHI::LoadAssets()
	{
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		if (FAILED(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error)))
		{
			OM_LOG_CRITICAL_TAG("Root signature serialize failed", OM::Logger::TagRender);
			return false;
		}

		if (FAILED(_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&_rootSignature))))
		{
			OM_LOG_CRITICAL_TAG("Failed to create root signature", OM::Logger::TagRender);
			return false;
		}

		ComPtr<ID3DBlob> vertexShader;
		ComPtr<ID3DBlob> pixelShader;

	#if defined(_DEBUG)
		// Enable better shader debugging with the graphics debugging tools.
		UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	#else
		UINT compileFlags = 0;
	#endif

		if (FAILED(D3DCompileFromFile(L"Assets/Shaders/shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr)))
		{
			OM_LOG_CRITICAL_TAG("Failed to compile vertex shader", OM::Logger::TagRender);
			return false;
		}

		if (FAILED(D3DCompileFromFile(L"Assets/Shaders/shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr)))
		{
			OM_LOG_CRITICAL_TAG("Failed to compile pixel shader", OM::Logger::TagRender);
			return false;
		}

		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
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
		if (FAILED(_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&_pipelineState))))
		{
			OM_LOG_CRITICAL_TAG("Failed to create graphics pipeline", OM::Logger::TagRender);
			return false;
		}

		if (FAILED(_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocator.Get(), _pipelineState.Get(), IID_PPV_ARGS(&_commandList))))
		{
			OM_LOG_CRITICAL_TAG("Failed to create command list", OM::Logger::TagRender);
			return false;
		}

		if (FAILED(_commandList->Close()))
		{
			OM_LOG_CRITICAL_TAG("Failed to close command list", OM::Logger::TagRender);
			return false;
		}

		Vertex triangleVertices[] =
		{
			{ { 0.0f, 0.25f * _aspectRatio, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
			{ { 0.25f, -0.25f * _aspectRatio, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
			{ { -0.25f, -0.25f * _aspectRatio, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
		};

		const UINT vertexBufferSize = sizeof(triangleVertices);

		CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);

		if (FAILED(_device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&_vertexBuffer))))
		{
			OM_LOG_CRITICAL_TAG("Failed to create committed resource", OM::Logger::TagRender);
			return false;
		}

		UINT8* pVertexDataBegin;
		CD3DX12_RANGE readRange(0, 0);        
		if (FAILED(_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin))))
		{
			OM_LOG_CRITICAL_TAG("Failed to cast vertex data", OM::Logger::TagRender);
			return false;
		}

		memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
		_vertexBuffer->Unmap(0, nullptr);

		_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
		_vertexBufferView.StrideInBytes = sizeof(Vertex);
		_vertexBufferView.SizeInBytes = vertexBufferSize;

		if (FAILED(_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence))))
		{
			OM_LOG_CRITICAL_TAG("Failed to create fence", OM::Logger::TagRender);
			return false;
		}

		_fenceValue = 1;

		_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (_fenceEvent == nullptr)
		{
			if (FAILED(HRESULT_FROM_WIN32(GetLastError())))
			{
				OM_LOG_CRITICAL_TAG("Fence is nullptr", OM::Logger::TagRender);
				return false;
			}
		}

		WaitForPreviousFrame();
		return true;
	}

	void RHI::PopulateCommandList()
	{
		if (FAILED(_commandAllocator->Reset()))
			return;

		if (FAILED(_commandList->Reset(_commandAllocator.Get(), _pipelineState.Get())))
			return;

		_commandList->SetGraphicsRootSignature(_rootSignature.Get());
		_commandList->RSSetViewports(1, &_viewport);
		_commandList->RSSetScissorRects(1, &_scissorRect);

		auto barrierToRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(_renderTargets[_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		_commandList->ResourceBarrier(1, &barrierToRenderTarget);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_rtvHeap->GetCPUDescriptorHandleForHeapStart(), _frameIndex, _rtvDescriptorSize);
		_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

		const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		_commandList->IASetVertexBuffers(0, 1, &_vertexBufferView);
		_commandList->DrawInstanced(3, 1, 0, 0);

		auto barrierToPresent = CD3DX12_RESOURCE_BARRIER::Transition(_renderTargets[_frameIndex].Get(),D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		_commandList->ResourceBarrier(1, &barrierToPresent);

		if (FAILED(_commandList->Close()))
			return;
	}

	void RHI::WaitForPreviousFrame()
	{
		const UINT64 fence = _fenceValue;
		if (FAILED(_commandQueue->Signal(_fence.Get(), fence)))
			return;

		_fenceValue++;

		if (_fence->GetCompletedValue() < fence)
		{
			if (FAILED(_fence->SetEventOnCompletion(fence, _fenceEvent)))
				return;

			WaitForSingleObject(_fenceEvent, INFINITE);
		}

		_frameIndex = _swapChain->GetCurrentBackBufferIndex();
	}

	void RHI::GetHardwareAdapter(IDXGIFactory1* factory, IDXGIAdapter1** adapter, bool requestHighPerformanceAdapter)
	{
		*adapter = nullptr;
		ComPtr<IDXGIAdapter1> potentialAdapter;

		ComPtr<IDXGIFactory6> factory6;
		if (SUCCEEDED(factory->QueryInterface(IID_PPV_ARGS(&factory6))))
		{
			for (UINT adapterIndex = 0;
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
			for (UINT adapterIndex = 0; SUCCEEDED(factory->EnumAdapters1(adapterIndex, &potentialAdapter)); ++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				potentialAdapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
					continue;

				if (SUCCEEDED(D3D12CreateDevice(potentialAdapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
					break;
			}
		}

		*adapter = potentialAdapter.Detach();
	}
}