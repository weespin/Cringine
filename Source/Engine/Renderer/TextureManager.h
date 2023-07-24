#pragma once
#include <d3d12.h>
#include <map>
#include <string>
#include <vector>
#include <wincodec.h> // Windows Imaging Component (WIC)

using TextureID = unsigned long;
constexpr TextureID MaxTextures = 1024;
const std::string TextureDir = "Textures\\";

class TextureManager
{
public:
	void Init(ID3D12Device* device, ID3D12GraphicsCommandList* IncommandList);

	HRESULT LoadImageDataFromFile(std::vector<unsigned char>& imageData, D3D12_RESOURCE_DESC& resourceDescription, std::string filename, int& bytesPerRow);

	void CacheTexture(const std::string& FileName, const char* pzsName);

	bool BindTexutre(const std::string& name);

private:
	bool CreateTexture(const std::string& FileName, const char* pzsName);

	TextureID GetNextTextureId() { return ++CurrentTextureId; }

	ID3D12Device* m_device;
	ID3D12GraphicsCommandList* commandList;
	std::map<std::string,TextureID> m_Textures = {};
	ID3D12DescriptorHeap* TextureSrvHeap;
	TextureID CurrentTextureId = -1;
};

