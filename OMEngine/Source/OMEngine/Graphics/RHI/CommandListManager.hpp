#pragma once

#include "OMEngine/Base.hpp"
#include <d3d12.h>
#include "OMEngine/Graphics/RHI/CommandQueue.hpp"

namespace OM::Graphics::RHI
{
	class OM_ENGINE_API CommandListManager
	{
	public:
		CommandListManager();
		~CommandListManager();

		void Create(ID3D12Device* device);
		void Delete();

		void CreateNewCommandList(D3D12_COMMAND_LIST_TYPE type, ID3D12GraphicsCommandList** list, ID3D12CommandAllocator** allocator);

		CommandQueue& GetGraphicsQueue() { return _graphicsQueue; }
		CommandQueue& GetComputeQueue()  { return _computeQueue; }
		CommandQueue& GetCopyQueue()	 { return _copyQueue; }
		CommandQueue& GetQueue(D3D12_COMMAND_LIST_TYPE type)
		{
			switch (type)
			{
			case D3D12_COMMAND_LIST_TYPE_COMPUTE:	return _computeQueue;
			case D3D12_COMMAND_LIST_TYPE_COPY:		return _copyQueue;
			default:								return _graphicsQueue;
			}
		}

	private:
		ID3D12Device* _device;
		
		CommandQueue _graphicsQueue;
		CommandQueue _computeQueue;
		CommandQueue _copyQueue;
	};
}