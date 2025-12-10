#pragma once

#include "OMEngine/Base.hpp"
#include "OMEngine/Graphics/RHI/GpuResource.hpp"
#include <string>

namespace OM::Graphics::RHI
{
	class PixelBuffer : public GpuResource
	{
	public:
		PixelBuffer();

		unsigned int GetWidth() const { return _width; }
		unsigned int GetHeight() const { return _height; }
		unsigned int GetDepthOrArraySize() const { return _depthOrArraySize; }
		DXGI_FORMAT GetFormat() const { return _format; }

	protected:
		unsigned int _width;
		unsigned int _height;
		unsigned int _depthOrArraySize;
		DXGI_FORMAT _format;

		D3D12_RESOURCE_DESC DescribeTexture2D(unsigned int width, unsigned int height, unsigned int depthOrArraySize, unsigned int numMips, DXGI_FORMAT format, unsigned int flags);
		void AssociateWithResource(const std::wstring& name, ID3D12Resource* resource, D3D12_RESOURCE_STATES currentState);
		void CreateTextureResource(const std::wstring& name, const D3D12_RESOURCE_DESC& resourceDesc, D3D12_CLEAR_VALUE clearValue, D3D12_GPU_VIRTUAL_ADDRESS vidMem = static_cast<D3D12_GPU_VIRTUAL_ADDRESS>(0));
	
		static DXGI_FORMAT GetBaseFormat(DXGI_FORMAT format);
		static DXGI_FORMAT GetUAVFormat(DXGI_FORMAT format);
		static DXGI_FORMAT GetDSVFormat(DXGI_FORMAT format);
		static DXGI_FORMAT GetDepthFormat(DXGI_FORMAT format);
		static DXGI_FORMAT GetStencilFormat(DXGI_FORMAT format);
		static size_t BytesPerPixel(DXGI_FORMAT format);
	};
}