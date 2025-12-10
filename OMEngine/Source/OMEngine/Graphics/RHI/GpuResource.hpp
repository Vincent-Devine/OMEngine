#pragma once

#include "OMEngine/Base.hpp"

#include <d3d12.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace OM::Graphics::RHI
{
	class OM_ENGINE_API GpuResource
	{
		friend class CommandContext;
		friend class GraphicsContext;

	public:
		GpuResource()
			: _gpuVirtualAddress(static_cast<D3D12_GPU_VIRTUAL_ADDRESS>(0))
			, _usageState(D3D12_RESOURCE_STATE_COMMON)
			, _transitioningState(static_cast<D3D12_RESOURCE_STATES>(-1))
		{
		}

		~GpuResource()
		{
			Destroy();
		}

		virtual void Destroy()
		{
			_resource = nullptr;
			_gpuVirtualAddress = static_cast<D3D12_GPU_VIRTUAL_ADDRESS>(0);
		}

		ID3D12Resource* GetResource() { return _resource.Get(); }
		const ID3D12Resource* GetResource() const { return _resource.Get(); }
		D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const { return _gpuVirtualAddress; }
		D3D12_RESOURCE_STATES GetState() const { return _usageState; }

	protected:
		ComPtr<ID3D12Resource> _resource;
		D3D12_GPU_VIRTUAL_ADDRESS _gpuVirtualAddress;

		// State
		D3D12_RESOURCE_STATES _usageState;
		D3D12_RESOURCE_STATES _transitioningState;
	};
}