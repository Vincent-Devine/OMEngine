#include "pch.h"
#include "OMEngine/Graphics/CoreRender/DescriptorAllocator.hpp"
#include "OMEngine/Graphics/RHI/RHI.hpp"

namespace OM::Graphics::CoreRender
{
	std::vector<DescriptorHeap*> DescriptorAllocator::s_heapPool;
	std::mutex DescriptorAllocator::s_allocationMutex;

	DescriptorAllocator::DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type)
		: _type(type)
		, _currentHeap(nullptr)
		, _descriptorSize(0)
		, _remainingFreeHandles(0)
	{
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocator::Allocate(unsigned int count)
	{
		std::lock_guard<std::mutex> Lock(s_allocationMutex);
		if (!_currentHeap || _remainingFreeHandles < count)
			RequestNewHeap();

		D3D12_CPU_DESCRIPTOR_HANDLE res = _currentHandle;
		_currentHandle.ptr += count * _descriptorSize;
		_remainingFreeHandles -= count;
		return res;
	}

	void DescriptorAllocator::RequestNewHeap()
	{
		DescriptorHeap* newHeap = new DescriptorHeap();
		newHeap->Create(L"RTV heap", _type, S_NUM_DESCRIPTOR_PER_HEAP);

		s_heapPool.push_back(newHeap);
		_currentHeap = newHeap;
		_currentHandle = _currentHeap->GetCpuStart();
		_remainingFreeHandles = S_NUM_DESCRIPTOR_PER_HEAP;

		_descriptorSize = RHI::RHI::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(_type);
	}
}