#include "DirectX12.h"

#include "Application.h"
#include "d3dx12.h"
#include "Game.h"
#include "Mesh.h"
#include "Engine/Utils/Logger.h"
#include "World/World.h"

bool Running = false;
#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }

//Todo: create a filemanager
bool LoadFile(const char* pszPath, unsigned char** pData, unsigned long& nSize)
{
    HANDLE hFile = CreateFileA(pszPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile != INVALID_HANDLE_VALUE)
    {
        DWORD dwFileSize = GetFileSize(hFile, NULL);

        if (dwFileSize != INVALID_FILE_SIZE)
        {
            *pData = new unsigned char[dwFileSize];
            nSize = dwFileSize;

            DWORD dwBytesRead;
            if (ReadFile(hFile, *pData, dwFileSize, &dwBytesRead, NULL))
            {
                CloseHandle(hFile);
                return true;
            }
        }

        CloseHandle(hFile);
    }

    *pData = nullptr;
    nSize = 0;
    return false;
}

bool DirectX12::InitD3D()
{

	HRESULT hr;

	// Step 1: Create the Direct3D 12 device
	
	// Enable the D3D12 debug layer
	ID3D12Debug* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}

	// Create a DXGI factory
	IDXGIFactory4* dxgiFactory;
	hr = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&dxgiFactory));
	if (FAILED(hr))
	{
		logger.Log(ERR, "Failed to create a DXGIFactory1!");
		return false;
	}

	// Step 2: Create the Direct3D 12 device
// 2.1: Find a compatible graphics adapter
	int adapterIndex = 0;
	bool adapterFound = false;

	while (dxgiFactory->EnumAdapters1(adapterIndex, &m_pAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC1 desc;
		m_pAdapter->GetDesc1(&desc);

		// Exclude software adapters
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			continue;
		}

		// Check if the device supports Direct3D 12 (feature level 11 or higher)
		hr = D3D12CreateDevice(m_pAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr);
		if (SUCCEEDED(hr))
		{
			DXGI_ADAPTER_DESC1 _AdapterDesc;
			m_pAdapter->GetDesc1(&_AdapterDesc);
			logger.Log(INFO, "Found a suitable adapter! %S", _AdapterDesc.Description);
			adapterFound = true;
			break;
		}

		adapterIndex++;
	}

	if (!adapterFound)
	{
		logger.Log(ERR, "Failed to find a compatible video adapter!");
		return false;
	}

	// 2.2: Create the device from the suitable adapter
	hr = D3D12CreateDevice(m_pAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_pDevice));
	if (FAILED(hr))
	{
		logger.Log(ERR, "Failed to create D3D12Device!");
		return false;
	}

	// Step 3: Create a Direct3D 12 command queue
	D3D12_COMMAND_QUEUE_DESC cqDesc = {};
	cqDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cqDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT; // Direct command queue type allows the GPU to execute commands directly

	hr = m_pDevice->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(&m_pCommandQueue));
	if (FAILED(hr))
	{
		logger.Log(ERR, "Failed to create a command queue!");
		return false;
	}

	// Step 4: Create the swap chain for double/triple buffering
	// Define the display mode and multi-sampling
	DXGI_MODE_DESC backBufferDesc = {};
	backBufferDesc.Width = Window::GetInstance().Width;
	backBufferDesc.Height = Window::GetInstance().Height;
	backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // RGBA 32-bit format, 8 bits per channel

	DXGI_SAMPLE_DESC sampleDesc = {};
	sampleDesc.Count = 1; // No multi-sampling, so set the count to 1

	// Describe and create the swap chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = frameBufferCount; // Number of frame buffers
	swapChainDesc.BufferDesc = backBufferDesc; // Back buffer description
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // Pipeline will render to this swap chain
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // DXGI will discard the buffer after calling Present
	swapChainDesc.OutputWindow = Window::GetInstance().hwnd; // Handle to the window
	swapChainDesc.SampleDesc = sampleDesc; // Multi-sampling description
	swapChainDesc.Windowed = !Window::GetInstance().FullScreen; // Set to true for windowed mode, false for fullscreen

	IDXGISwapChain* tempSwapChain;
	hr = dxgiFactory->CreateSwapChain(
		m_pCommandQueue, // The queue will be flushed once the swap chain is created
		&swapChainDesc, // Provide the swap chain description created above
		&tempSwapChain // Store the created swap chain in a temporary IDXGISwapChain interface
	);
	if (FAILED(hr))
	{
		logger.Log(ERR, "Failed to create a swapchain!");
		return false;
	}

	m_pSwapChain = static_cast<IDXGISwapChain3*>(tempSwapChain);
	m_nFrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	// Step 4.1: Create the back buffers (render target views) descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = frameBufferCount; // Number of descriptors for this heap
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // This heap is a render target view heap

	// This heap will not be directly referenced by the shaders (not shader visible), as this will store the output from the pipeline
	// otherwise we would set the heap's flag to D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	hr = m_pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_pDescriptorHeap[HEAP::RTV]));
	if (FAILED(hr))
	{
		logger.Log(ERR,"Failed to create a RTV Descriptor Heap!");
		return false;
	}

	// Get the size of a descriptor in the RTV heap. Descriptor sizes may vary between devices, so we must ask the device for the size.
	// We will use this size to increment a descriptor handle offset.
	m_nRTVDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// Get a handle to the first descriptor in the descriptor heap. A handle is similar to a pointer but cannot be used directly as a C++ pointer.
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pDescriptorHeap[HEAP::RTV]->GetCPUDescriptorHandleForHeapStart());

	// Create an RTV for each buffer (double buffering uses two buffers, triple buffering uses three).
	for (int i = 0; i < frameBufferCount; i++)
	{
		// First, get the i-th buffer in the swap chain and store it in the i-th position of our ID3D12Resource array.
		hr = m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pRenderTarget[i]));
		if (FAILED(hr))
		{
			logger.Log(ERR, "Failed to create an RTV for frame #%i!", i);
			return false;
		}

		// Then, create a render target view that binds the swap chain buffer (ID3D12Resource[i]) to the RTV handle.
		m_pDevice->CreateRenderTargetView(m_pRenderTarget[i], nullptr, rtvHandle);

		// Increment the RTV handle by the RTV descriptor size obtained earlier.
		rtvHandle.Offset(1, m_nRTVDescriptorSize);
	}

	// Step 5: Create command allocators for each frame buffer.
	for (int i = 0; i < frameBufferCount; i++)
	{
		hr = m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pCommandAllocator[i]));
		if (FAILED(hr))
		{
			logger.Log(ERR, "Failed to create a command allocator for frame #%i!", i);
			return false;
		}
	}

	// Step 6: Create a command list using the first allocator.
	hr = m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCommandAllocator[m_nFrameIndex], NULL, IID_PPV_ARGS(&m_pCommandList));
	if (FAILED(hr))
	{
		logger.Log(ERR, "Failed to create a command list");
		return false;
	}
	
	// Step 7: Create fences and a fence event for each frame buffer.
	for (int i = 0; i < frameBufferCount; i++)
	{
		hr = m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence[i]));
		if (FAILED(hr))
		{
			logger.Log(ERR, "Failed to create a fence for frame #%i!", i);
			return false;
		}
		m_nFenceValue[i] = 0; //Initial value
	}


	// Create a handle to a fence event.
	m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_FenceEvent == nullptr)
	{
		logger.Log(ERR, "Failed to create a fence event");
		return false;
	}
	m_pCommandList->Close();
	{
		ID3D12CommandList* ppCommandLists[] = { m_pCommandList };
		m_pCommandQueue->ExecuteCommandLists(1, ppCommandLists);
		m_nFenceValue[m_nFrameIndex]++;
		hr = m_pCommandQueue->Signal(m_pFence[m_nFrameIndex], m_nFenceValue[m_nFrameIndex]);
	}
	hr = m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCommandAllocator[m_nFrameIndex], NULL, IID_PPV_ARGS(&m_pImGUICommandList));
	if (FAILED(hr))
	{
		logger.Log(ERR, "Failed to create a imgui command list");
		return false;
	}
	m_pImGUICommandList->Close();
	WaitForPreviousFrame();
	{
		ID3D12CommandList* ppCommandLists[] = { m_pImGUICommandList };
		m_pCommandQueue->ExecuteCommandLists(1, ppCommandLists);
		m_nFenceValue[m_nFrameIndex]++;
		hr = m_pCommandQueue->Signal(m_pFence[m_nFrameIndex], m_nFenceValue[m_nFrameIndex]);
	}
	//8. Create a root signature
	WaitForPreviousFrame();
	// create a root descriptor, which explains where to find the data for this root parameter
	CD3DX12_DESCRIPTOR_RANGE range;
	range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // one SRV at register t0

	CD3DX12_ROOT_PARAMETER rootParameters[2];
	rootParameters[0].InitAsConstantBufferView(0);
	rootParameters[1].InitAsDescriptorTable(1, &range, D3D12_SHADER_VISIBILITY_PIXEL);

	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.ShaderRegister = 0;
	samplerDesc.RegisterSpace = 0;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(_countof(rootParameters), // we have 1 root parameter
		rootParameters, // a pointer to the beginning of our root parameters array
		1,
		&samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ID3DBlob* signature;
	hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);
	if (FAILED(hr))
	{
		logger.Log(ERR,"Failed to serialize a root signature");
		return false;
	}

	hr = m_pDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature));
	if (FAILED(hr))
	{
		logger.Log(ERR,"Failed to create a root signature");
		return false;
	}

	//6. Create vertex and pixel shaders

	// when debugging, we can compile the shader files at runtime.
	// but for release versions, we can compile the hlsl shaders
	// with fxc.exe to create .cso files, which contain the shader
	// bytecode. We can load the .cso files at runtime to get the
	// shader bytecode, which of course is faster than compiling
	// them at runtime

	// compile vertex shader
	if (!LoadFile("Shaders//VertexShader.cso", &pVertexShader, nVertexShaderSize))
	{
		logger.Log(ERR, "Failed to load a VertexShader!");
		return false;
	}

	// fill out a shader bytecode structure, which is basically just a pointer
	// to the shader bytecode and the size of the shader bytecode
	D3D12_SHADER_BYTECODE vertexShaderBytecode = {};
	vertexShaderBytecode.BytecodeLength = nVertexShaderSize;
	vertexShaderBytecode.pShaderBytecode = pVertexShader;

	// compile pixel shader
	if (!LoadFile("Shaders//PixelShader.cso", &pPixelShader, nPixelShaderSize))
	{
		logger.Log(ERR, "Failed to load a PixelShader!");
		return false;
	}


	// fill out shader bytecode structure for pixel shader
	D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
	pixelShaderBytecode.BytecodeLength = nPixelShaderSize;
	pixelShaderBytecode.pShaderBytecode = pPixelShader;

	// create input layout

	// The input layout is used by the Input Assembler so that it knows
	// how to read the vertex data bound to it.

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28 + (28-12), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

	// fill out an input layout description structure
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};

	// we can get the number of elements in an array by "sizeof(array) / sizeof(arrayElementType)"
	inputLayoutDesc.NumElements = sizeof(inputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	inputLayoutDesc.pInputElementDescs = inputLayout;

	// create a pipeline state object (PSO)

	// In a real application, you will have many pso's. for each different shader
	// or different combinations of shaders, different blend states or different rasterizer states,
	// different topology types (point, line, triangle, patch), or a different number
	// of render targets you will need a pso

	// VS is the only required shader for a pso. You might be wondering when a case would be where
	// you only set the VS. It's possible that you have a pso that only outputs data with the stream
	// output, and not on a render target, which means you would not need anything after the stream
	// output.

	
	

	// create a depth stencil descriptor heap so we can get a pointer to the depth stencil buffer
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	hr = m_pDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_pDescriptorHeap[DS]));
	if (FAILED(hr))
	{
		Running = false;
	}

	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	auto HeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto Tex = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, Window::GetInstance().Width, Window::GetInstance().Height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	m_pDevice->CreateCommittedResource(
		&HeapProp,
		D3D12_HEAP_FLAG_NONE,
		&Tex,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthOptimizedClearValue,
		IID_PPV_ARGS(&m_pDepthStencilBuffer)
		);
	m_pDescriptorHeap[DS]->SetName(L"Depth/Stencil Resource Heap");

	m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilDesc, m_pDescriptorHeap[DS]->GetCPUDescriptorHandleForHeapStart());

	// Create constant buffer resource heap for each frame
	// Since constant buffer will be updated multiple times per frame, we use an upload heap instead of a default heap.
	// This is more efficient for resources that are modified frequently.
	
	// Allocate 64KB for each resource, as buffer resource heaps need to be aligned to 64KB.
	// In this case, we're creating 3 resources, one for each frame. Each constant buffer contains 16 floats (a 4x4 matrix).
	// We store 2 constant buffers in each heap, one for each model, which means we only use 128 bits of each 64KB resource.

	for (int i = 0; i < frameBufferCount; ++i)
	{
		{
			auto HeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			auto Buffer = CD3DX12_RESOURCE_DESC::Buffer(1024 * 64);

			hr = m_pDevice->CreateCommittedResource(
				&HeapProp, // heap for uploading constant buffer data
				D3D12_HEAP_FLAG_NONE, // no flags
				&Buffer, // resource heap size (must be a multiple of 64KB for single-textures and constant buffers)
				D3D12_RESOURCE_STATE_GENERIC_READ, // data will be read from, so use generic read state
				nullptr, // no optimized clear value needed for constant buffers
				IID_PPV_ARGS(&m_pConstantBufferUploadHeap[i]));
		}
		m_pConstantBufferUploadHeap[i]->SetName(L"Constant Buffer Upload Resource Heap");

		CD3DX12_RANGE readRange(0, 0); // No intention to read from this resource on the CPU

		// Map resource heap to get a GPU virtual address at the beginning of the heap
		hr = m_pConstantBufferUploadHeap[i]->Map(0, &readRange, reinterpret_cast<void**>(&m_pCBVGPUAddress[i]));

		// Due to constant read alignment requirements, constant buffer views must be 256-bit aligned.
		// Our buffers are smaller than 256 bits, so we need to add spacing between the two buffers,
		// ensuring the second buffer starts at 256 bits from the beginning of the resource heap.
		ZeroMemory(m_pCBVGPUAddress[i], 1024 * 64);
	}
	m_pCommandList->Reset(m_pCommandAllocator[m_nFrameIndex], nullptr);
	//Todo: this should be lazyloaded on LoadContent!
	m_TextureManager.Init(m_pDevice, m_pCommandList);
	m_TextureManager.CacheTexture("white.jpg","white");
	m_TextureManager.CacheTexture("sphere.jpg","sphere");
	m_TextureManager.CacheTexture("oasis_Dirt.png", "oasis_Dirt");
	m_TextureManager.CacheTexture("oasis_PalmTree.png", "oasis_PalmTree");
	m_TextureManager.CacheTexture("oasis_Rock.png", "oasis_Rock");
	m_TextureManager.CacheTexture("oasis_Sand.png", "oasis_Sand");
	m_TextureManager.CacheTexture("oasis_Water.png", "oasis_Water");

	// Now we execute the command list to upload the initial assets (triangle data)
	m_pCommandList->Close();
	ID3D12CommandList* ppCommandLists[] = { m_pCommandList };
	m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// increment the m_pFence value now, otherwise the buffer might not be uploaded by the time we start drawing
	m_nFenceValue[m_nFrameIndex]++;
	hr = m_pCommandQueue->Signal(m_pFence[m_nFrameIndex], m_nFenceValue[m_nFrameIndex]);
	if (FAILED(hr))
	{
		Running = false;
	}

	// Fill out the Viewport
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;
	m_ViewPort.Width = static_cast<float>(Window::GetInstance().Width);
	m_ViewPort.Height = static_cast<float>(Window::GetInstance().Height);
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;

	// Fill out a scissor rect
	m_ScissorRect.left = 0;
	m_ScissorRect.top = 0;
	m_ScissorRect.right = Window::GetInstance().Width;
	m_ScissorRect.bottom = Window::GetInstance().Height;

	// set starting camera state
	Game::GetInstance().Camera.SetFOV(90.f);
	Game::GetInstance().Camera.Initialize();

	m_RenderStateManager.Init(m_pDevice);

	{

		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {}; // a structure to define a pso
		psoDesc.InputLayout = inputLayoutDesc; // the structure describing our input layout
		psoDesc.pRootSignature = m_pRootSignature; // the root signature that describes the input data this pso needs
		psoDesc.VS = vertexShaderBytecode; // structure describing where to find the vertex shader bytecode and how large it is
		psoDesc.PS = pixelShaderBytecode; // same as VS but for pixel shader
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // type of topology we are drawing
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the render target
		psoDesc.SampleDesc = sampleDesc; // must be the same sample description as the swapchain and depth/stencil buffer
		psoDesc.SampleMask = 0xffffffff; // sample mask has to do with multi-sampling. 0xffffffff means point sampling is done
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); // a default rasterizer state.
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // a default blent state.
		psoDesc.NumRenderTargets = 1; // we are only binding one render target
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // a default depth stencil state
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;


		D3D12_RENDER_TARGET_BLEND_DESC blendDesc = {};
		blendDesc.BlendEnable = TRUE;
		blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		psoDesc.BlendState.RenderTarget[0] = blendDesc;

		m_RenderStateManager.CreatePipelineState("default", psoDesc);
	}
	{
		D3D12_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {}; // a structure to define a pso
		psoDesc.InputLayout = inputLayoutDesc; // the structure describing our input layout
		psoDesc.pRootSignature = m_pRootSignature; // the root signature that describes the input data this pso needs
		psoDesc.VS = vertexShaderBytecode; // structure describing where to find the vertex shader bytecode and how large it is
		psoDesc.PS = pixelShaderBytecode; // same as VS but for pixel shader
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // type of topology we are drawing
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the render target
		psoDesc.SampleDesc = sampleDesc; // must be the same sample description as the swapchain and depth/stencil buffer
		psoDesc.SampleMask = 0xffffffff; // sample mask has to do with multi-sampling. 0xffffffff means point sampling is done
		psoDesc.RasterizerState = rasterizerDesc; // a default rasterizer state.
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // a default blent state.
		psoDesc.NumRenderTargets = 1; // we are only binding one render target
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // a default depth stencil state
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		m_RenderStateManager.CreatePipelineState("wireframe", psoDesc);
	}

	//LoadSimpleTexture
	bInited = true;
	return true;
}

void DirectX12::UpdatePipeline()
{
	HRESULT hr;

	// reset the command list. by resetting the command list we are putting it into
	// a recording state so we can start recording commands into the command allocator.
	// the command allocator that we reference here may have multiple command lists
	// associated with it, but only one can be recording at any time. Make sure
	// that any other command lists associated to this command allocator are in
	// the closed state (not recording).
	// Here you will pass an initial pipeline state object as the second parameter,
	// but in this tutorial we are only clearing the rtv, and do not actually need
	// anything but an initial default pipeline, which is what we get by setting
	// the second parameter to NULL

	
	hr = m_pCommandList->Reset(m_pCommandAllocator[m_nFrameIndex], m_RenderStateManager.GetPipelineState("default"));
	//m_pCommandList->SetPipelineState()
	if (FAILED(hr))
	{
		Running = false;
	}

	// here we start recording commands into the m_pCommandList (which all the commands will be stored in the m_pCommandAllocator)
	auto transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTarget[m_nFrameIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	// transition the "m_nFrameIndex" render target from the present state to the render target state so the command list draws to it starting from here

	m_pCommandList->ResourceBarrier(1, &transitionBarrier);

	// here we again get the handle to our current render target view so we can set it as the render target in the output merger stage of the pipeline
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pDescriptorHeap[RTV]->GetCPUDescriptorHandleForHeapStart(), m_nFrameIndex, m_nRTVDescriptorSize);

	// get a handle to the depth/stencil buffer
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_pDescriptorHeap[DS]->GetCPUDescriptorHandleForHeapStart());

	// set the render target for the output merger stage (the output of the pipeline)
	m_pCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	// Clear the render target by using the ClearRenderTargetView command
	ColorHelper::RGBA ConvertedColor = ColorHelper::HSV2RGB(Game::GetInstance().BGColorHSV);
	FLOAT clearColor[] = { ConvertedColor.R, ConvertedColor.G , ConvertedColor.B, 1.0f };
	m_pCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	

	// clear the depth/stencil buffer
	m_pCommandList->ClearDepthStencilView(m_pDescriptorHeap[DS]->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// set root signature
	m_pCommandList->SetGraphicsRootSignature(m_pRootSignature); // set the root signature

	// draw triangle
	m_pCommandList->RSSetViewports(1, &m_ViewPort); // set the viewports
	m_pCommandList->RSSetScissorRects(1, &m_ScissorRect); // set the scissor rects

	m_pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // set the primitive topology
	if (auto* World = Application::GetInstance().CurrentWorld)
{

    EntityManager& EntityManager = World->GetEntityManager();
    const auto& PipelineStates = m_RenderStateManager.GetPipelineStates();
    const auto& ConstantBufferHeap = m_pConstantBufferUploadHeap[m_nFrameIndex];

    for (const auto& RenderState : PipelineStates)
    {
        m_pCommandList->SetPipelineState(RenderState.second);

        for (auto& Entity : EntityManager.GetEntites())
        {
            if (!Entity->m_bIsVisible)
            {
                continue;
            }

            std::vector<std::shared_ptr<PrimitiveComponent>> RenderableComponents;
        	auto& ComponentManager = Entity->GetComponentManager();
        	auto& Components = ComponentManager.GetAllComponents();

            RenderableComponents.reserve(Components.size());

            for (const auto& component : Components)
            {
                std::vector<std::shared_ptr<BaseComponent>> AllSubComponents;
                ComponentManager.GetAllSubComponents(component, AllSubComponents);

                for (const auto& sub_component : AllSubComponents)
                {
                    if (const auto& derivedSubComponent = std::dynamic_pointer_cast<PrimitiveComponent>(sub_component))
                    {
                        RenderableComponents.emplace_back(derivedSubComponent);
                    }
                }
            }

            for (const auto& renderable_component : RenderableComponents)
            {
                if (auto* Mesh = renderable_component->GetMesh())
                {
                    const auto cbOffset = renderable_component->GetConstantBufferOffset();
                    const auto cbvAddress = ConstantBufferHeap->GetGPUVirtualAddress() + (ConstantBufferPerObjectAlignedSize * cbOffset);

                    m_pCommandList->SetGraphicsRootConstantBufferView(0, cbvAddress);

                    if (renderable_component->GetRenderMode() == RenderState.first)
                    {
                        if (Mesh->GetIsDirty())
                        {
                            Mesh->Prepare(m_pCommandList);
                        }

                        m_pCommandList->IASetVertexBuffers(0, 1, &Mesh->GetVertexBufferView());
                        m_pCommandList->IASetIndexBuffer(&Mesh->GetIndexBufferView());

                        if (const auto& TexturedComponent = std::dynamic_pointer_cast<TexturedMeshComponent>(renderable_component))
                        {
                            const auto& ConstantBuffer = TexturedComponent->GetConstantBuffer();
                            if (ConstantBuffer.m_UseTexture)
                            {
                                m_TextureManager.BindTexutre(TexturedComponent->GetTextureName());
                            }
                        }

                        m_pCommandList->DrawIndexedInstanced(Mesh->GetModelIndices(), 1, 0, 0, 0);
                    }
                }
            }
        }
    }
}
	//ImGUIContext::GetInstance().RenderIMGUI();
	auto TransitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTarget[m_nFrameIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	m_pCommandList->ResourceBarrier(1, &TransitionBarrier);

	hr = m_pCommandList->Close();
	if (FAILED(hr))
	{
		Running = false;
	}
}

void DirectX12::UpdateImGuiPipeline()
{
	HRESULT hr;

	hr = m_pImGUICommandList->Reset(m_pCommandAllocator[m_nFrameIndex], nullptr);
	//m_pCommandList->SetPipelineState()
	if (FAILED(hr))
	{
		Running = false;
	}

	// here we start recording commands into the m_pCommandList (which all the commands will be stored in the m_pCommandAllocator)
	auto transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTarget[m_nFrameIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	// transition the "m_nFrameIndex" render target from the present state to the render target state so the command list draws to it starting from here

	m_pImGUICommandList->ResourceBarrier(1, &transitionBarrier);

	// here we again get the handle to our current render target view so we can set it as the render target in the output merger stage of the pipeline
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pDescriptorHeap[RTV]->GetCPUDescriptorHandleForHeapStart(), m_nFrameIndex, m_nRTVDescriptorSize);

	// get a handle to the depth/stencil buffer
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_pDescriptorHeap[DS]->GetCPUDescriptorHandleForHeapStart());

	// set the render target for the output merger stage (the output of the pipeline)
	m_pImGUICommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
	
	ImGUIContext::GetInstance().RenderIMGUI();
	auto TransitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTarget[m_nFrameIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	m_pImGUICommandList->ResourceBarrier(1, &TransitionBarrier);

	hr = m_pImGUICommandList->Close();
	if (FAILED(hr))
	{
		Running = false;
	}
}

void DirectX12::Resize(int w, int h)
{
	Game::GetInstance().Camera.SetFOV(Game::GetInstance().Camera.m_FOV);
	if (!IsInited())
	{
		return;
	}
	WaitForPreviousFrame();
	Flush();

	// Release resources associated with the current window size
	for (UINT i = 0; i < frameBufferCount; ++i)
	{
		if (m_pRenderTarget[i])
		{
			m_pRenderTarget[i]->Release();
			m_pRenderTarget[i] = nullptr;
		}
	}
	if (m_pDepthStencilBuffer)
	{
		m_pDepthStencilBuffer->Release();
		m_pDepthStencilBuffer = nullptr;
	}

	// Update swap chain and device resources for the new window size
	DXGI_SWAP_CHAIN_DESC desc;
	m_pSwapChain->GetDesc(&desc);
	desc.Windowed = !Window::GetInstance().FullScreen;
	m_pSwapChain->SetFullscreenState(!desc.Windowed, NULL);
	m_pSwapChain->ResizeBuffers(desc.BufferCount, w, h, desc.BufferDesc.Format, desc.Flags);
	// Recreate render target views
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pDescriptorHeap[RTV]->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < frameBufferCount; ++i)
	{
		m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pRenderTarget[i]));
		m_pDevice->CreateRenderTargetView(m_pRenderTarget[i], nullptr, rtvHandle);
		rtvHandle.Offset(1, m_nRTVDescriptorSize);
	}
	

	// Recreate depth/stencil buffer
	D3D12_RESOURCE_DESC depthDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, w, h, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	D3D12_CLEAR_VALUE depthClearValue = {};
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.DepthStencil.Stencil = 0;
	{
		auto HeapPropertyDefault = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		m_pDevice->CreateCommittedResource(
			&HeapPropertyDefault,
			D3D12_HEAP_FLAG_NONE,
			&depthDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthClearValue,
			IID_PPV_ARGS(&m_pDepthStencilBuffer)
		);
	}
	

	// Recreate viewport and scissor rect
	m_ViewPort.Width = static_cast<float>(w);
	m_ViewPort.Height = static_cast<float>(h);
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;
	m_ScissorRect = CD3DX12_RECT(0, 0, w, h);
	
	// Reset the command allocator and command list.
	for (int i = 0; i < frameBufferCount; ++i)
	{
		// Close the command list before resetting it
		

		// Reset the command allocator
		m_pCommandAllocator[i]->Reset();
		m_pCommandList->Close();
		// Reset the command list
		m_pCommandList->Reset(m_pCommandAllocator[i], nullptr);
	}
	// Transition the swap chain and depth/stencil buffer to the appropriate states
	D3D12_RESOURCE_BARRIER barrier[2];
	barrier[0] = CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTarget[m_nFrameIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	//barrier[1] = CD3DX12_RESOURCE_BARRIER::Transition(m_pDepthStencilBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_DEPTH_READ);
	m_pCommandList->ResourceBarrier(1, barrier);

	// Set the new render target and depth/stencil buffer views
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandleNew(m_pDescriptorHeap[RTV]->GetCPUDescriptorHandleForHeapStart(), m_nFrameIndex, m_nRTVDescriptorSize);
	m_pCommandList->OMSetRenderTargets(1, &rtvHandleNew, FALSE, nullptr);
	FLOAT c[4] = { 1,1,1,1 };
	m_pCommandList->ClearRenderTargetView(rtvHandleNew, c, 0, nullptr);
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_pDescriptorHeap[DS]->GetCPUDescriptorHandleForHeapStart());
	m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, nullptr, dsvHandle);
	m_pCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	// Set the new viewport and scissor rect
	m_pCommandList->RSSetViewports(1, &m_ViewPort);
	m_pCommandList->RSSetScissorRects(1, &m_ScissorRect);

	// Close and execute the command list
	//m_pCommandList->Close();

	ID3D12CommandList* ppCommandLists[] = { m_pCommandList };
	m_pCommandList->Close();
	m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	m_nFenceValue[m_nFrameIndex]++;
	m_pCommandQueue->Signal(m_pFence[m_nFrameIndex], m_nFenceValue[m_nFrameIndex]);
	NeedsReize = false;

}
void DirectX12::UpdateRenderTargetViews(ID3D12Device* device, IDXGISwapChain3* swapChain,
	ID3D12DescriptorHeap* descriptorHeap)
{
	auto rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart());

	for (int i = 0; i < frameBufferCount; ++i)
	{
		ID3D12Resource* backBuffer;
		swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer));

		device->CreateRenderTargetView(backBuffer, nullptr, rtvHandle);

		m_pConstantBufferUploadHeap[i] = backBuffer;

		rtvHandle.Offset(rtvDescriptorSize);
	}
}

void DirectX12::Render()
{
	HRESULT hr;
	WaitForPreviousFrame();

	// we can only reset an allocator once the gpu is done with it
	// resetting an allocator frees the memory that the command list was stored in
	hr = m_pCommandAllocator[m_nFrameIndex]->Reset();
	if (FAILED(hr))
	{
		Running = false;
	}

	UpdatePipeline(); // update the pipeline by sending commands to the commandqueue
	{
		// create an array of command lists (only one command list here)
		static ID3D12CommandList* ppCommandLists[] = { m_pCommandList };

		// execute the array of command lists
		m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	}
	hr = m_pCommandQueue->Signal(m_pFence[m_nFrameIndex], m_nFenceValue[m_nFrameIndex]);
	if (FAILED(hr))
	{
		Running = false;
	}
	WaitForPreviousFrame();
	UpdateImGuiPipeline();
	{
		// create an array of command lists (only one command list here)
		static ID3D12CommandList* ppCommandLists[] = { m_pImGUICommandList };
	
		// execute the array of command lists
		m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	}
	// this command goes in at the end of our command queue. we will know when our command queue 
	// has finished because the m_pFence value will be set to "m_nFenceValue" from the GPU since the command
	// queue is being executed on the GPU
	hr = m_pCommandQueue->Signal(m_pFence[m_nFrameIndex], m_nFenceValue[m_nFrameIndex]);
	if (FAILED(hr))
	{
		Running = false;
	}
	// present the current backbuffer
	hr = m_pSwapChain->Present(Window::GetInstance().bUseVSYNC ? 1 : 0, 0);
	if (FAILED(hr))
	{
		Running = false;
	}
}

void DirectX12::Shutdown()
{
	WaitForPreviousFrame();
	CloseHandle(m_FenceEvent);
	// wait for the gpu to finish all frames
	for (int i = 0; i < frameBufferCount; ++i)
	{
		m_nFrameIndex = i;
		WaitForPreviousFrame();
	}

	// get swapchain out of full screen before exiting
	BOOL fs = false;
	if (m_pSwapChain->GetFullscreenState(&fs, NULL))
		m_pSwapChain->SetFullscreenState(false, NULL);

	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pCommandQueue);
	SAFE_RELEASE(m_pDescriptorHeap[RTV]);
	SAFE_RELEASE(m_pCommandList);

	for (int i = 0; i < frameBufferCount; ++i)
	{
		//SAFE_RELEASE(m_pRenderTarget[i]);
		SAFE_RELEASE(m_pCommandAllocator[i]);
		SAFE_RELEASE(m_pFence[i]);
	};
	
	SAFE_RELEASE(m_pRootSignature);
	//SAFE_RELEASE(vertexBuffer);
	//SAFE_RELEASE(indexBuffer);

	SAFE_RELEASE(m_pDepthStencilBuffer);
	SAFE_RELEASE(m_pDescriptorHeap[DS]);

	for (int i = 0; i < frameBufferCount; ++i)
	{
		SAFE_RELEASE(m_pConstantBufferUploadHeap[i]);
	};
}

void DirectX12::Flush()
{
	//https://gamedev.stackexchange.com/a/175443
	for (int i = 0; i < frameBufferCount; i++)
	{
		uint64_t fenceValueForSignal = ++m_nFenceValue[i];
		m_pCommandQueue->Signal(m_pFence[i], fenceValueForSignal);
		if (m_pFence[i]->GetCompletedValue() < m_nFenceValue[i])
		{
			m_pFence[i]->SetEventOnCompletion(fenceValueForSignal, m_FenceEvent);
			WaitForSingleObject(m_FenceEvent, INFINITE);
		}
	}
	m_nFrameIndex = 0;
}

void DirectX12::WaitForPreviousFrame()
{
	HRESULT hr;

	// swap the current rtv buffer index so we draw on the correct buffer
	m_nFrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	// if the current m_pFence value is still less than "m_nFenceValue", then we know the GPU has not finished executing
	// the command queue since it has not reached the "m_pCommandQueue->Signal(m_pFence, m_nFenceValue)" command
	if (m_pFence[m_nFrameIndex]->GetCompletedValue() < m_nFenceValue[m_nFrameIndex])
	{
		// we have the m_pFence create an event which is signaled once the m_pFence's current value is "m_nFenceValue"
		hr = m_pFence[m_nFrameIndex]->SetEventOnCompletion(m_nFenceValue[m_nFrameIndex], m_FenceEvent);
		if (FAILED(hr))
		{
			Running = false;
		}

		// We will wait until the m_pFence has triggered the event that it's current value has reached "m_nFenceValue". once it's value
		// has reached "m_nFenceValue", we know the command queue has finished executing
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}

	// increment m_nFenceValue for next frame
	m_nFenceValue[m_nFrameIndex]++;
}
