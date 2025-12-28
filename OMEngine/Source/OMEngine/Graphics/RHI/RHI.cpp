#include "pch.h"
#include "OMEngine/Graphics/RHI/RHI.hpp"
#include "OMEngine/Utils/GraphicsUtils.hpp"
#include "OMEngine/Utils/Utils.hpp"

namespace OM::Graphics::RHI
{
	void EnableDebugLayer(uint* factoryFlags)
	{
		ComPtr<ID3D12Debug> debugInterface;
		OM_ASSERTION_HRESULT(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)), "Failed to get D3D12 debug interface.");
		debugInterface->EnableDebugLayer();
		*factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}

	ComPtr<IDXGIAdapter1> GetAdapter(ComPtr<IDXGIFactory4> factory, bool useWrapDevice)
	{
		ComPtr<IDXGIAdapter1> adapter;

		if (useWrapDevice)
		{
			OM_ASSERTION_HRESULT(factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter)), "Failed to enumerate wrap adapter");
			return adapter;
		}
		else
		{
			for (uint i = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(i, &adapter); ++i)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
					continue;

				if (FAILED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
					continue;

				return adapter;
			}
		}
		OM_ASSERTION_TAG(false, "Failed to get adapter.", OM::Logger::TagRender);
	}

	bool RHI::Initialize(HWND hwnd)
	{
		uint factoryFlags;
	
	#if defined(_DEBUG)
		EnableDebugLayer(&factoryFlags);
	#endif
	
		ComPtr<IDXGIFactory4> factory;
		OM_ASSERTION_HRESULT(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory)), "Failed to create factory");

		CreateDevice(factory);
		_commandListManager.Create(_device.Get());

		CreateSwapChain(factory, hwnd);
		_frameIndex = _swapChain->GetCurrentBackBufferIndex();

		return true;
	}

	void RHI::Destroy()
	{
	}

	void RHI::CreateDevice(ComPtr<IDXGIFactory4> factory)
	{
		ComPtr<IDXGIAdapter1> adapter = GetAdapter(factory, _useWrapDevice);
		OM_ASSERTION_HRESULT(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&_device)), "Failed to create device");
	}

	void RHI::CreateSwapChain(ComPtr<IDXGIFactory4> factory, HWND hwnd)
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

		OM_ASSERTION_HRESULT(factory->CreateSwapChainForHwnd(_commandListManager.GetGraphicsQueue().GetD3D12CommandQueue(), hwnd, &swapChainDesc, nullptr, nullptr, &swapChain), "Failed to create swap chain.");
		OM_ASSERTION_HRESULT(swapChain.As(&_swapChain), "Failed to convert swap chain.");
	}
}