#pragma once

#include "OMEngine/Base.hpp"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectX12/d3dx12/d3dx12.h>
#include <string>
#include <wrl.h>
#include <shellapi.h>

using Microsoft::WRL::ComPtr;

namespace OM::Wrapper
{
	class OM_ENGINE_API RHI
	{
	public:
		static RHI* GetInstance();

		bool Initialisation(HWND hwnd);
		void Render() const;
		void Destroy();

	private:
		static inline RHI* _instance = nullptr;
        static const UINT _frameCount = 2;

        struct Vertex
        {
            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT4 color;
        };

        // Pipeline objects.
        CD3DX12_VIEWPORT _viewport;
        CD3DX12_RECT _scissorRect;
        ComPtr<IDXGISwapChain3> _swapChain;
        ComPtr<ID3D12Device> _device;
        ComPtr<ID3D12Resource> _renderTargets[_frameCount];
        ComPtr<ID3D12CommandAllocator> _commandAllocator;
        ComPtr<ID3D12CommandQueue> _commandQueue;
        ComPtr<ID3D12RootSignature> _rootSignature;
        ComPtr<ID3D12DescriptorHeap> _rtvHeap;
        ComPtr<ID3D12PipelineState> _pipelineState;
        ComPtr<ID3D12GraphicsCommandList> _commandList;
        UINT _rtvDescriptorSize;

        // App resources.
        ComPtr<ID3D12Resource> _vertexBuffer;
        D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;

        // Synchronization objects.
        UINT _frameIndex;
        HANDLE _fenceEvent;
        ComPtr<ID3D12Fence> _fence;
        UINT64 _fenceValue;

        // Adapter info
        bool _useWarpDevice;

        bool LoadPipeline(HWND hwnd);
        bool LoadAssets();
        
        void GetHardwareAdapter(IDXGIFactory1* factory, IDXGIAdapter1** adapter, bool requestHighPerformanceAdapter = true);
        void PopulateCommandList();
        void WaitForPreviousFrame();
	};
}