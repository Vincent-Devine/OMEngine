#pragma once

#include "OMEngine/Base.hpp"
#include "OMEngine/Graphics/CoreRender/PixelBuffer.hpp"
#include <string>

namespace OM::Graphics::CoreRender
{
	class ColorBuffer : public PixelBuffer
	{
	public:
		void CreateFromSwapChain(const std::wstring& name, ID3D12Resource* baseResource);
		void Create(const std::wstring& name, const unsigned int width, const unsigned int height, const unsigned int numMips, DXGI_FORMAT format);

		void SetClearColor(float color[4]) { memcpy(_clearColor, color, sizeof(_clearColor)); }

		D3D12_CPU_DESCRIPTOR_HANDLE GetRTV() const { return _rtvHandle; }
		D3D12_CPU_DESCRIPTOR_HANDLE GetSRV() const { return _srvHandle; }
		const float* GetClearColor() const { return _clearColor; }

	protected:
		D3D12_CPU_DESCRIPTOR_HANDLE _rtvHandle; // render traget view
		D3D12_CPU_DESCRIPTOR_HANDLE _srvHandle; // shader resource view
	
		float _clearColor[4];
	};
}