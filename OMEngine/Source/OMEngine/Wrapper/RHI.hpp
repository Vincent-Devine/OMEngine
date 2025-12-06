#pragma once

#include "OMEngine/Base.hpp"

#include <string>

// Direct3D 12
#include <DirectX12/d3dx12/d3dx12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

namespace OM::Wrapper
{
	class OM_ENGINE_API RHI
	{
	public:
		static RHI* GetInstance();

		bool Initialisation(HWND hwnd);
		void Render();
		void Destroy();

	private:
		static inline RHI* _instance = nullptr;

        struct Vertex
        {
            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT2 uv;
        };

        // Initilisation
        bool LoadPipeline(HWND hwnd);
        bool LoadAssets();

        // Device
        ComPtr<ID3D12Device> _device;
        bool _useWarpDevice = false;
        void GetDebugInterface(unsigned int* dxgiFactoryFlags);
        bool CreateDevice(ComPtr<IDXGIFactory4> factory);
        void GetHardwareAdapter(IDXGIFactory1* factory, IDXGIAdapter1** adapter, bool requestHighPerformanceAdapter = true);

        // Render
        static const unsigned int _FRAME_COUNT = 2;
        unsigned int _frameIndex;
        ComPtr<IDXGISwapChain3> _swapChain;
        ComPtr<ID3D12Resource> _renderTargets[_FRAME_COUNT];
        ComPtr<ID3D12DescriptorHeap> _rtvHeap; // rtv: Render Target View
        ComPtr<ID3D12DescriptorHeap> _srvHeap; // srv: Shader Resource View
        unsigned int _rtvDescriptorSize;
        bool CreateSwapChain(ComPtr<IDXGIFactory4> factory, HWND hwnd);
        bool CreateRTV();

        // Command (GPU)
        ComPtr<ID3D12CommandAllocator> _commandAllocator;
        ComPtr<ID3D12CommandQueue> _commandQueue;
        ComPtr<ID3D12GraphicsCommandList> _commandList;
        bool CreateQueue();
        bool CreateCommandList();
        void PopulateCommandList();

        // Pipeline
        ComPtr<ID3D12RootSignature> _rootSignature;
        ComPtr<ID3D12PipelineState> _pipelineState;
        bool CreateRootSignature();
        bool CreatePipelineState();
        
        // Geometry
        ComPtr<ID3D12Resource> _vertexBuffer;
        D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;
        bool CreateVertexBuffer();

        // Viewport
        CD3DX12_VIEWPORT _viewport;
        CD3DX12_RECT _scissorRect;
        float _aspectRatio;

        // Synchronization
        HANDLE _fenceEvent;
        ComPtr<ID3D12Fence> _fence;
        unsigned __int64 _fenceValue;
        bool CreateFence();
        void WaitForPreviousFrame();

        // Texture
        static const unsigned int _TEXTURE_WIDTH = 256;
        static const unsigned int _TEXTURE_HEIGHT = 256;
        static const unsigned int _TEXTURE_PIXEL_SIZE = 4;
        ComPtr<ID3D12Resource> _texture;
        ComPtr<ID3D12Resource> _textureUploadHeap;
        std::vector<unsigned __int8> GenerateTextureData();
	};
}