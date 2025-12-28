#pragma once

#include "OMEngine/Base.hpp"
#include <DirectX12/d3dx12/d3dx12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <wrl.h>
#include "OMEngine/Utils/TypeDef.hpp"
#include "OMEngine/Graphics/RHI/CommandListManager.hpp"

using Microsoft::WRL::ComPtr;

namespace OM::Graphics::RHI
{
	class OM_ENGINE_API RHI
	{
	public:
		static inline RHI* GetInstance()
		{
			if (!_instance)
				_instance = new RHI();
			return _instance;
		}

		bool Initialize(HWND hwnd);
		void Destroy();

		ID3D12Device* GetDevice() const { return _device.Get(); }

	private:
		static inline RHI* _instance = nullptr;

		// Device
		ComPtr<ID3D12Device> _device;
		bool _useWrapDevice = false;
		void CreateDevice(ComPtr<IDXGIFactory4> factory);
		
		// CommandList - CommandQueue - CommandAllocator
		CommandListManager _commandListManager;

		// Swap chain
		uint _frameIndex;
		ComPtr<IDXGISwapChain3> _swapChain;
		const uint _FRAME_COUNT = 3;
		void CreateSwapChain(ComPtr<IDXGIFactory4> factory, HWND hwnd);
	};
}