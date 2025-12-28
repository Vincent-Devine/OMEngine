#pragma once

#include "OMEngine/Base.hpp"
#include <d3d12.h>
#include <wrl/client.h>
#include <vector>
#include <queue>
#include <mutex>
#include "OMEngine/Utils/TypeDef.hpp"

using Microsoft::WRL::ComPtr;

namespace OM::Graphics::RHI
{
	class OM_ENGINE_API CommandAllocatorPool
	{
	public:
		CommandAllocatorPool(const D3D12_COMMAND_LIST_TYPE type);
		~CommandAllocatorPool();

		void Create(ID3D12Device* device);
		void Delete();

		ID3D12CommandAllocator* RequestAllocator(uint64 completeFenceValue);
		void DiscardAllocator(uint64 fenceValue, ID3D12CommandAllocator* allocator);

		uint64 GetSize() const { return _allocatorPool.size(); }

	private:
		const D3D12_COMMAND_LIST_TYPE _commandListType;
		ID3D12Device* _device; // optimization
		std::vector<ComPtr<ID3D12CommandAllocator>> _allocatorPool;
		std::queue<std::pair<uint64, ID3D12CommandAllocator*>> _readyAllocator;
		std::mutex _allocatorMutex;
	};
}