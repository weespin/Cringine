#include "TextureManager.h"
#include "Engine/Utils/Logger.h"
#include "d3dx12.h"

void TextureManager::Init(ID3D12Device* device, ID3D12GraphicsCommandList* IncommandList)
{
	m_device = device;
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = MaxTextures;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&TextureSrvHeap));
	commandList = IncommandList;
}

HRESULT TextureManager::LoadImageDataFromFile(std::vector<unsigned char>& imageData, D3D12_RESOURCE_DESC& resourceDescription, std::string filename, int& bytesPerRow)
{
	imageData.clear();

	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr)) return hr;

	IWICImagingFactory* wicFactory;
	hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory));
	if (FAILED(hr)) return hr;

	IWICBitmapDecoder* wicDecoder;
	hr = wicFactory->CreateDecoderFromFilename(std::wstring(filename.begin(), filename.end()).c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &wicDecoder);
	if (FAILED(hr)) return hr;

	IWICBitmapFrameDecode* wicFrame;
	hr = wicDecoder->GetFrame(0, &wicFrame);
	if (FAILED(hr)) return hr;

	IWICFormatConverter* wicConverter;
	hr = wicFactory->CreateFormatConverter(&wicConverter);
	if (FAILED(hr)) return hr;

	hr = wicConverter->Initialize(wicFrame, GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeErrorDiffusion, nullptr, 0.f, WICBitmapPaletteTypeCustom);
	if (FAILED(hr)) return hr;

	UINT textureWidth, textureHeight;
	hr = wicConverter->GetSize(&textureWidth, &textureHeight);
	if (FAILED(hr)) return hr;

	bytesPerRow = (textureWidth * 32 + 7) / 8;
	UINT imageSize = bytesPerRow * textureHeight;

	imageData.resize(imageSize);

	hr = wicConverter->CopyPixels(nullptr, bytesPerRow, imageSize, imageData.data());
	if (FAILED(hr)) return hr;

	resourceDescription = {};
	resourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDescription.Alignment = 0;
	resourceDescription.Width = textureWidth;
	resourceDescription.Height = textureHeight;
	resourceDescription.DepthOrArraySize = 1;
	resourceDescription.MipLevels = 1;
	resourceDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	resourceDescription.SampleDesc.Count = 1;
	resourceDescription.SampleDesc.Quality = 0;
	resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE;

	return hr;
}

void TextureManager::CacheTexture(const std::string& FileName, const char* pzsName)
{
	if(m_Textures.find(pzsName) != m_Textures.end() )
	{
		logger.Log(INFO, "Skipping loading texture %s because its already cached!", FileName.c_str());
		return;
	}

	CreateTexture(FileName, pzsName);
}

bool TextureManager::CreateTexture(const std::string& FileName, const char* pzsName)
{
	// Load image data from a PNG file
	std::vector<unsigned char> imageData;
	D3D12_RESOURCE_DESC textureDesc;
	int bytesPerRow;
	HRESULT hr = LoadImageDataFromFile(imageData, textureDesc, TextureDir + FileName, bytesPerRow);
	if (FAILED(hr))
	{
		logger.Log(ERR, "Failed to load image %s!", FileName.c_str());
		return false;
	}

	// Create the texture resource
	D3D12_HEAP_PROPERTIES heapProps = { D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
	ID3D12Resource* textureResource;
	hr = m_device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&textureResource));
	if (FAILED(hr))
	{
		logger.Log(ERR, "Failed to create a texture resource!");
		return false;
	}

	// Create an upload heap for the texture data
	UINT64 uploadBufferSize = GetRequiredIntermediateSize(textureResource, 0, 1);
	D3D12_HEAP_PROPERTIES uploadHeapProps = { D3D12_HEAP_TYPE_UPLOAD, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
	D3D12_RESOURCE_DESC uploadBufferDesc = { D3D12_RESOURCE_DIMENSION_BUFFER, 0, uploadBufferSize, 1, 1, 1, DXGI_FORMAT_UNKNOWN, {1, 0}, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_NONE };
	ID3D12Resource* uploadHeap;
	hr = m_device->CreateCommittedResource(&uploadHeapProps, D3D12_HEAP_FLAG_NONE, &uploadBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uploadHeap));
	if (FAILED(hr))
	{
		logger.Log(ERR, "Failed to create a UploadHeap CreateCommittedResource!");
		return false;
	}

	// Copy the texture data to the upload heap and then to the texture resource
	D3D12_SUBRESOURCE_DATA textureData = { imageData.data(), bytesPerRow, static_cast<LONG_PTR>(bytesPerRow) * static_cast<LONG_PTR>(textureDesc.Height) };
	UpdateSubresources(commandList, textureResource, uploadHeap, 0, 0, 1, &textureData);

	// Transition the texture resource to the shader resource state
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = textureResource;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandList->ResourceBarrier(1, &barrier);

	// Create a shader resource view (SRV) for the texture
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	const auto& TexId = GetNextTextureId();
	CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(TextureSrvHeap->GetCPUDescriptorHandleForHeapStart(), static_cast<INT>(TexId), m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	m_device->CreateShaderResourceView(textureResource, &srvDesc, srvHandle);

	m_Textures[pzsName] = TexId;
	return true;
}

bool TextureManager::BindTexutre(const std::string& name)
{
	auto it = m_Textures.find(name);
	if(it == m_Textures.end())
	{
		logger.Log(ERR, "Failed to bind a texture because %s doesn't exist!", name.c_str());
		return false;
	}
	ID3D12DescriptorHeap* descriptorHeaps[] = { TextureSrvHeap };
	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuSrvHandle(TextureSrvHeap->GetGPUDescriptorHandleForHeapStart(), it->second, m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	commandList->SetGraphicsRootDescriptorTable(1, gpuSrvHandle);
	return true;
}
