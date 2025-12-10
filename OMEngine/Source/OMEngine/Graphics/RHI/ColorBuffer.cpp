#include "pch.h"
#include "OMEngine/Graphics/RHI/ColorBuffer.hpp"

namespace OM::Graphics::RHI
{
	void ColorBuffer::CreateFromSwapChain(const std::wstring& name, ID3D12Resource* baseResource)
	{
	}

	void ColorBuffer::Create(const std::wstring& name, const unsigned int width, const unsigned int height, const unsigned int numMips, DXGI_FORMAT format)
	{
	}
}