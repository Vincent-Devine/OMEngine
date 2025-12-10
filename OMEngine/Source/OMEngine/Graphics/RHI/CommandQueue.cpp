#include "pch.h"
#include "OMEngine/Graphics/RHI/CommandQueue.hpp"
#include "OMEngine/Utils/GraphicsUtils.hpp"

namespace OM::Graphics::RHI
{
	CommandQueue::CommandQueue(const D3D12_COMMAND_LIST_TYPE type)
		: _type(type)
		, _commandQueue(nullptr)
		, _fence(nullptr)
		, _nextFenceValue(0)
		, _lastCompletedFenceValue(0)
		, _allocatorPool(type)
	{
	}

	CommandQueue::~CommandQueue()
	{
		Delete();
	}

	void CommandQueue::Create(ID3D12Device* device)
	{
		if (!device) { OM_LOG_CRITICAL_TAG("device null.", OM::Logger::TagRender); return; }
		if (IsReady()) { OM_LOG_WARNING_TAG("command queue already create.", OM::Logger::TagRender); return; }

		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = _type;
		queueDesc.NodeMask = 1;
		if (!CHECK_HRESULT(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_commandQueue)), "Failed to create command queue")) return;
		_commandQueue->SetName(L"CommandQueue");

		if (!CHECK_HRESULT(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence)), "Failed to create fence")) return;
		_fence->SetName(L"Fence");
		_fence->Signal(static_cast<unsigned __int64>(_type) << 56);

		_fenceEventHandle = CreateEvent(nullptr, false, false, nullptr);
		if (_fenceEventHandle == nullptr)
			if (!CHECK_HRESULT(HRESULT_FROM_WIN32(GetLastError()), "Fence is nullptr")) return;

		_allocatorPool.Create(device);
	}

	void CommandQueue::Delete()
	{
		if (!_commandQueue)
			return;
		_allocatorPool.Delete();
		
		CloseHandle(_fenceEventHandle);

		_fence->Release();
		_fence = nullptr;

		_commandQueue->Release();
		_commandQueue = nullptr;
	}

	unsigned __int64 CommandQueue::IncrementFence()
	{
		std::lock_guard<std::mutex> LockGuard(_fenceMutex);
		_commandQueue->Signal(_fence.Get(), _nextFenceValue);
		return _nextFenceValue++;
	}

	bool CommandQueue::IsFenceComplete(unsigned __int64 fenceValue)
	{
		if (fenceValue > _lastCompletedFenceValue)
			_lastCompletedFenceValue = _lastCompletedFenceValue > _fence->GetCompletedValue() ? _lastCompletedFenceValue : _fence->GetCompletedValue();
		return fenceValue <= _lastCompletedFenceValue;
	}

	void CommandQueue::WaitForFence(unsigned __int64 fenceValue)
	{
		if (IsFenceComplete(fenceValue)) return;

		std::lock_guard<std::mutex> lockGuard(_fenceMutex);
		_fence->SetEventOnCompletion(fenceValue, _fenceEventHandle);
		WaitForSingleObject(_fenceEventHandle, INFINITE);
		_lastCompletedFenceValue = fenceValue;
	}

	unsigned __int64 CommandQueue::ExecuteCommandList(ID3D12CommandList* list)
	{
		std::lock_guard<std::mutex> lockGuard(_fenceMutex);

		if (!CHECK_HRESULT(static_cast<ID3D12GraphicsCommandList*>(list)->Close(), "failed to close list")) return 0;
		_commandQueue->ExecuteCommandLists(1, &list);
		_commandQueue->Signal(_fence.Get(), _nextFenceValue);
		return _nextFenceValue++;
	}

	ID3D12CommandAllocator* CommandQueue::RequestAllocator()
	{
		unsigned __int64 completeFence = _fence->GetCompletedValue();
		return _allocatorPool.RequestAllocator(completeFence);
	}

	void CommandQueue::DiscardAllocator(unsigned __int64 fenceValueForReset, ID3D12CommandAllocator* allocator)
	{
		_allocatorPool.DiscardAllocator(fenceValueForReset, allocator);
	}
}