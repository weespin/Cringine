#pragma once
#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_4.h>

#include "RenderStateManager.h"
#include "TextureManager.h"

class DirectX12
{
public:
	static DirectX12& GetInstance()
	{
		static DirectX12 _instance;
		return _instance;
	}
	bool InitD3D();
	void UpdatePipeline();
	void UpdateImGuiPipeline();
	void Resize(int w, int h);
	void UpdateRenderTargetViews(ID3D12Device* device, IDXGISwapChain3* swapChain, ID3D12DescriptorHeap* descriptorHeap);
	void Render();
	void CheckInvalidation();
	void Shutdown();
	void Flush();
	void ExecuteCommandList();
	void WaitForPreviousFrame();
	bool IsInited() const {return bInited;}
	bool NeedsReize = false;
public:
	enum HEAP : int
	{
		RTV, // Render Target View Heap
		SRV, // Shared Resource View Heap
		DS,  // Depth/Stencil Buffer Heap
		MAX
	};

	//TODO: Remove this?
	bool bInited = false;

	// a graphics card (this includes the embedded graphics on the motherboard)
	IDXGIAdapter1* m_pAdapter;

	// number of buffers we want, 2 for double buffering, 3 for tripple buffering
	static const int frameBufferCount = 3; 

	// Direct3d Device
	ID3D12Device* m_pDevice; 

	// swapchain used to switch between render targets
	IDXGISwapChain3* m_pSwapChain; 

	// container for command lists
	ID3D12CommandQueue* m_pCommandQueue;

	/**
	 *	Descriptor Heaps
	 *	HEAP::RTV - a descriptor heap to hold resources like the render targets
	 *	HEAP::SRV - Shared Resource View Heap
	 *  HEAP::ÂÛ - This is a heap for our depth/stencil buffer descriptor
	 */
	ID3D12DescriptorHeap* m_pDescriptorHeap[static_cast<int>(HEAP::MAX)];

	// number of render targets equal to buffer count
	ID3D12Resource* m_pRenderTarget[frameBufferCount]; 

	// we want enough allocators for each buffer * number of threads (we only have one thread)
	ID3D12CommandAllocator* m_pCommandAllocator[frameBufferCount]; 

	// A command list we can record commands into, then execute them to render the frame
	ID3D12GraphicsCommandList* m_pCommandList; 
	// A command list we can record commands into, then execute them to render the frame
	ID3D12GraphicsCommandList* m_pImGUICommandList;

	/**
	 *  An object that is locked while our command list is being executed by the gpu. We need as many
	 *  as we have allocators (more if we want to know when the gpu is finished with an asset)
	 */
	ID3D12Fence* m_pFence[frameBufferCount];    

	// a handle to an event when our fence is unlocked by the gpu
	HANDLE m_FenceEvent; 

	// this value is incremented each frame. each m_pFence will have its own value
	UINT64 m_nFenceValue[frameBufferCount]; 

	// current rtv we are on
	unsigned int m_nFrameIndex; 

	// size of the rtv descriptor on the device (all front and back buffers will be the same size)
	UINT m_nRTVDescriptorSize;

	// root signature defines data shaders will access
	ID3D12RootSignature* m_pRootSignature; 

	// area that output from rasterizer will be stretched to.
	D3D12_VIEWPORT m_ViewPort; 

	// the area to draw in. pixels outside that area will not be drawn onto
	D3D12_RECT m_ScissorRect; 

	// This is the memory for our depth buffer. it will also be used for a stencil buffer in a later tutorial
	ID3D12Resource* m_pDepthStencilBuffer; 

	// this is the memory on the gpu where constant buffers for each frame will be placed
	ID3D12Resource* m_pConstantBufferUploadHeap[frameBufferCount]; 

	// this is a pointer to each of the constant buffer resource heaps
	UINT8* m_pCBVGPUAddress[frameBufferCount]; 

	unsigned char* pVertexShader = nullptr;
	unsigned long  nVertexShaderSize;

	unsigned char* pPixelShader = nullptr;
	unsigned long  nPixelShaderSize;

	RenderStateManager m_RenderStateManager;
	TextureManager m_TextureManager;
};

