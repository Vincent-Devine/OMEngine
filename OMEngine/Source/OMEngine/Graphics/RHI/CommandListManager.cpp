#include "pch.h"
#include "OMEngine/Graphics/RHI/CommandListManager.hpp"
#include "OMEngine/Utils/GraphicsUtils.hpp"

namespace OM::Graphics::RHI
{
	CommandListManager::CommandListManager()
		: _device(nullptr)
		, _graphicsQueue(D3D12_COMMAND_LIST_TYPE_DIRECT)
		, _computeQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE)
		, _copyQueue(D3D12_COMMAND_LIST_TYPE_COPY)
	{
	}

	CommandListManager::~CommandListManager()
	{
		Delete();
	}

	void CommandListManager::Create(ID3D12Device* device)
	{
		OM_ASSERTION(device, "device is null");
		_device = device;
		_graphicsQueue.Create(device);
		_computeQueue.Create(device);
		_copyQueue.Create(device);
	}

	void CommandListManager::Delete()
	{
		_graphicsQueue.Delete();
		_computeQueue.Delete();
		_copyQueue.Delete();
	}

	void CommandListManager::CreateNewCommandList(D3D12_COMMAND_LIST_TYPE type, ID3D12GraphicsCommandList** list, ID3D12CommandAllocator** allocator)
	{
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT: *allocator = _graphicsQueue.RequestAllocator(); break;
		case D3D12_COMMAND_LIST_TYPE_COMPUTE: *allocator = _computeQueue.RequestAllocator(); break;
		case D3D12_COMMAND_LIST_TYPE_COPY: *allocator = _copyQueue.RequestAllocator(); break;
		}

		if (!CHECK_HRESULT(_device->CreateCommandList(1, type, *allocator, nullptr, IID_PPV_ARGS(list)), "Failed to create command list")) return;
		(*list)->SetName(L"CommandList");
	}
}