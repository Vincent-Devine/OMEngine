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

	void RHI::Render() const
	{
	}

	void RHI::Destroy()
	{
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

	bool LoadAssets()
	{
		return true;
	}

	void PopulateCommandList()
	{
	}

	void WaitForPreviousFrame()
	{
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
					IID_PPV_ARGS(&adapter)));
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