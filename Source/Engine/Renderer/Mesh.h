#pragma once
#include <d3d12.h>
#include <string>
#include <vector>

#include "Application.h"
struct Vertex {
	Vertex() = default;

	Vertex(float x, float y, float z, float r, float g, float b, float a, float u, float v, float normalx, float normaly, float normalz)
		: pos(x, y, z), color(r, g, b, a), uv(u, v), normal(normalx,normaly,normalz) {}

	XMFLOAT3 pos{};
	XMFLOAT4 color{};
	XMFLOAT2 uv{};
	XMFLOAT3 normal{};
};
class Mesh
{
public:
	void Init();
	~Mesh();
	void LoadFromFile(const std::string& FileName);
	int GetVertexBufferSize() const { return static_cast<int>(Vertexes.size() * sizeof(Vertex)); }
	bool GetIsDirty() const { return m_bIsDirty; }
	void SetIsDirty(bool IsDirty) { m_bIsDirty = IsDirty; }
	void Prepare(ID3D12GraphicsCommandList* _commandList);

	int GetIndexBufferSize() const { return static_cast<int>(Indexes.size() * sizeof(DWORD)); };
	std::vector<DWORD>& GetIndexes()  { return Indexes; }
	std::vector<Vertex>& GetVertexes()  { return Vertexes; }
	ID3D12Resource* GetVertexBuffer() const { return vertexBuffer; }
	ID3D12Resource* GetIndexBuffer() const { return indexBuffer; }
	D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() { return vertexBufferView; }
	D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() { return indexBufferView; }
	ID3D12Resource* GetVertexBufferUploadHeap() const { return vBufferUploadHeap; }
	ID3D12Resource* GetIndexBufferUploadHeap() const { return iBufferUploadHeap; }
	DWORD GetModelIndices() const { return GetIndexBufferSize() / sizeof(DWORD); }


private:
	std::vector<DWORD> Indexes{};
	std::vector<Vertex> Vertexes{};
	ID3D12Resource* vBufferUploadHeap{};
	ID3D12Resource* iBufferUploadHeap{};
	ID3D12Resource* vertexBuffer{}; // a default buffer in GPU memory that we will load vertex data for our triangle into
	ID3D12Resource* indexBuffer{}; // a default buffer in GPU memory that we will load index data for our triangle into
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{}; // a structure containing a pointer to the vertex data in gpu memory
	// the total size of the buffer, and the size of each element (vertex)
	D3D12_INDEX_BUFFER_VIEW indexBufferView{}; // a structure holding information about the index buffer
	bool m_bIsDirty = true;
};