#pragma once

#include "OMEngine/Base.hpp"
#include <d3d12.h>
#include <wrl/client.h>
#include <string>

using Microsoft::WRL::ComPtr;

namespace OM::Graphics::CoreRender
{
	class DescriptorHeap
	{
	public:
		DescriptorHeap();
		~DescriptorHeap();

		bool Create(const std::wstring& debugHeapName, D3D12_DESCRIPTOR_HEAP_TYPE type, unsigned int maxCount);

		ID3D12DescriptorHeap* GetHeap() const { return _heap.Get(); }
		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuStart() const { return _cpuHeapStart; }
		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuStart() const { return _gpuHeapStart; }
		unsigned int GetNumDescriptor() const { return _numDescriptor; }

	private:
		ComPtr<ID3D12DescriptorHeap> _heap;
		D3D12_CPU_DESCRIPTOR_HANDLE _cpuHeapStart;
		D3D12_GPU_DESCRIPTOR_HANDLE _gpuHeapStart;
		unsigned int _numDescriptor;

		void Destroy();
	};
}