#include "pch.h"
#include "OMEngine/Graphics/RHI/CommandAllocatorPool.hpp"
#include "OMEngine/Utils/GraphicsUtils.hpp"

namespace OM::Graphics::RHI
{
	CommandAllocatorPool::CommandAllocatorPool(const D3D12_COMMAND_LIST_TYPE type)
		: _commandListType(type)
		, _device(nullptr)
	{
	}

	CommandAllocatorPool::~CommandAllocatorPool()
	{
		Delete();
	}

	void CommandAllocatorPool::Create(ID3D12Device* device)
	{
		_device = device;
	}

	void CommandAllocatorPool::Delete()
	{
		for (size_t i = 0; i < _allocatorPool.size(); i++)
			_allocatorPool[i]->Release();
		_allocatorPool.clear();
	}

	ID3D12CommandAllocator* CommandAllocatorPool::RequestAllocator(uint64 completeFenceValue)
	{
		std::lock_guard<std::mutex> lockGuard(_allocatorMutex);
		ID3D12CommandAllocator* allocator = nullptr;
		if (!_readyAllocator.empty())
		{
			std::pair<uint64, ID3D12CommandAllocator*>& allocatorPair = _readyAllocator.front();
			if (allocatorPair.first <= completeFenceValue)
			{
				allocator = allocatorPair.second;
				OM_ASSERTION_HRESULT(allocator->Reset(), "Failed to reset command allocator");
				_readyAllocator.pop();
			}
		}

		if (!allocator)
		{
			OM_ASSERTION_HRESULT(_device->CreateCommandAllocator(_commandListType, IID_PPV_ARGS(&allocator)), "Failed to create new command allocator");
			allocator->SetName(L"CommandAllocator" + _allocatorPool.size());
			_allocatorPool.push_back(allocator);
		}

		return allocator;
	}
	
	void CommandAllocatorPool::DiscardAllocator(uint64 fenceValue, ID3D12CommandAllocator* allocator)
	{
		std::lock_guard<std::mutex> lockGuard(_allocatorMutex);
		_readyAllocator.push(std::make_pair(fenceValue, allocator));
	}

}