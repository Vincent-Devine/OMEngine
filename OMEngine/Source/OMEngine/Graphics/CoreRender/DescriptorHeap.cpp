#include "pch.h"
#include "OMEngine/Graphics/CoreRender/DescriptorHeap.hpp"
#include "OMEngine/Graphics/RHI/RHI.hpp"
#include "OMEngine/Utils/GraphicsUtils.hpp"


namespace OM::Graphics::CoreRender
{
	DescriptorHeap::DescriptorHeap()
	{
	}

	DescriptorHeap::~DescriptorHeap()
	{
		Destroy();
	}

	void DescriptorHeap::Create(const std::wstring& debugHeapName, D3D12_DESCRIPTOR_HEAP_TYPE type, unsigned int maxCount)
	{
		_numDescriptor = maxCount;

		D3D12_DESCRIPTOR_HEAP_DESC desc;
		desc.Type = type;
		desc.NumDescriptors = _numDescriptor;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask = 1;

		OM_ASSERTION_HRESULT(RHI::RHI::GetInstance()->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_heap)), "Failed to create descriptor heap");

		_heap->SetName(debugHeapName.c_str());
		_cpuHeapStart = _heap->GetCPUDescriptorHandleForHeapStart();
		if (desc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
			_gpuHeapStart = _heap->GetGPUDescriptorHandleForHeapStart();
	}

	void DescriptorHeap::Destroy()
	{
		_heap.Reset();
	}
}