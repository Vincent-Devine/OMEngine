#pragma once

#include "OMEngine/Base.hpp"
#include <d3d12.h>
#include <wrl/client.h>
#include <mutex>
#include "OMEngine/Utils/TypeDef.hpp"
#include "OMEngine/Graphics/RHI/CommandAllocatorPool.hpp"

using Microsoft::WRL::ComPtr;

namespace OM::Graphics::RHI
{
	class OM_ENGINE_API CommandQueue
	{
		friend class CommandListManager;

	public:
		CommandQueue(const D3D12_COMMAND_LIST_TYPE type);
		~CommandQueue();

		void Create(ID3D12Device* device);
		void Delete();

		inline bool IsReady() { return _commandQueue != nullptr; }

		uint64 IncrementFence();
		bool IsFenceComplete(uint64 fenceValue);
		void WaitForFence(uint64 fenceValue);

		ID3D12CommandQueue* GetD3D12CommandQueue() { return _commandQueue.Get(); }

	private:
		ComPtr<ID3D12CommandQueue> _commandQueue;
		const D3D12_COMMAND_LIST_TYPE _type;

		CommandAllocatorPool _allocatorPool;

		std::mutex _fenceMutex;
		std::mutex _eventMutex;

		ComPtr<ID3D12Fence> _fence;
		uint64 _nextFenceValue;
		uint64 _lastCompletedFenceValue;
		HANDLE _fenceEventHandle;

		uint64 ExecuteCommandList(ID3D12CommandList* list);
		ID3D12CommandAllocator* RequestAllocator();
		void DiscardAllocator(uint64 fenceValueForReset, ID3D12CommandAllocator* allocator);
	};
}