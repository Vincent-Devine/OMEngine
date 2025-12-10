#pragma once 

#include "OMEngine/Base.hpp"
#include "OMEngine/Graphics/CoreRender/DescriptorHeap.hpp"
#include <vector>
#include <mutex>

namespace OM::Graphics::CoreRender
{

	class OM_ENGINE_API DescriptorAllocator
	{
	public:
		DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type);

		D3D12_CPU_DESCRIPTOR_HANDLE Allocate(unsigned int count);

		static void DestroyAll();

	protected:
		D3D12_DESCRIPTOR_HEAP_TYPE _type;
		DescriptorHeap* _currentHeap;
		D3D12_CPU_DESCRIPTOR_HANDLE _currentHandle;

		unsigned int _descriptorSize;
		unsigned int _remainingFreeHandles;

		static std::vector<DescriptorHeap*> s_heapPool;
		static std::mutex s_allocationMutex;
		static const unsigned int S_NUM_DESCRIPTOR_PER_HEAP = 256;

		void RequestNewHeap();
	};
}