#include "Mesh.h"
#include <iterator>
#include <sstream>
#include <string>

#include "ModelPredCompiler.h"
#include "Engine/Renderer/d3dx12.h"
#include "Engine/Renderer/DirectX12.h"
const std::string ModelDir = "Models\\";

//void Mesh::GetBBOX(std::vector<Vertex>& VertexesOut)
//{
//	if(m_CachedBBOX.size())
//	{
//		VertexesOut = m_CachedBBOX;
//	}
//	//CalculateBBOX
//	//   .0------1  		   Y│
//	// .' |    .'| 		   		│		  /Z
//	//3---+--2'  | 		   		│		 /
//	//|   |  |   | 		   		│		/
//	//|  ,4--+---5 		   		│      /
//	//|.'    | .'  		   		│     /
//	//7------6'			   		│    /
//	//					   		│   /
//	//					   		│  /
//	//					   		│ /
//	//					   		────────────────>X DirectX has left-handed coordinate system!
//	//
//		
//	VertexesOut.resize(8);
//	for (const auto& Vertex : Vertexes)
//	{
//		VertexesOut[0].pos = XMFLOAT3(min(VertexesOut[0].pos.x, Vertex.pos.x), max(VertexesOut[0].pos.y, Vertex.pos.y), min(VertexesOut[0].pos.z, Vertex.pos.z));
//		VertexesOut[1].pos = XMFLOAT3(max(VertexesOut[1].pos.x, Vertex.pos.x), max(VertexesOut[1].pos.y, Vertex.pos.y), min(VertexesOut[1].pos.z, Vertex.pos.z));
//		VertexesOut[2].pos = XMFLOAT3(max(VertexesOut[2].pos.x, Vertex.pos.x), min(VertexesOut[2].pos.y, Vertex.pos.y), min(VertexesOut[2].pos.z, Vertex.pos.z));
//		VertexesOut[3].pos = XMFLOAT3(min(VertexesOut[3].pos.x, Vertex.pos.x), min(VertexesOut[3].pos.y, Vertex.pos.y), min(VertexesOut[3].pos.z, Vertex.pos.z));
//		VertexesOut[4].pos = XMFLOAT3(min(VertexesOut[4].pos.x, Vertex.pos.x), max(VertexesOut[4].pos.y, Vertex.pos.y), max(VertexesOut[4].pos.z, Vertex.pos.z));
//		VertexesOut[5].pos = XMFLOAT3(max(VertexesOut[5].pos.x, Vertex.pos.x), max(VertexesOut[5].pos.y, Vertex.pos.y), max(VertexesOut[5].pos.z, Vertex.pos.z));
//		VertexesOut[6].pos = XMFLOAT3(max(VertexesOut[6].pos.x, Vertex.pos.x), min(VertexesOut[6].pos.y, Vertex.pos.y), max(VertexesOut[6].pos.z, Vertex.pos.z));
//		VertexesOut[7].pos = XMFLOAT3(min(VertexesOut[7].pos.x, Vertex.pos.x), min(VertexesOut[7].pos.y, Vertex.pos.y), max(VertexesOut[7].pos.z, Vertex.pos.z));
//	}
//	//Extreme points! Maybe we can use it later
//	//for (auto Vertex : Vertexes)
//	//{
//	//	//0 - MAX Y, MAX Z, MIN X
//	//	if (Vertex.pos.y > VertexesOut[0].pos.y && Vertex.pos.z > VertexesOut[0].pos.z && Vertex.pos.x < VertexesOut[0].pos.x)
//	//	{
//	//		VertexesOut[0] = Vertex;
//	//	}
//	//	//1 - MAX Y, MAX Z, MAX X
//	//	if (Vertex.pos.y > VertexesOut[1].pos.y && Vertex.pos.z > VertexesOut[1].pos.z && Vertex.pos.x > VertexesOut[1].pos.x)
//	//	{
//	//		VertexesOut[1] = Vertex;
//	//	}
//	//	//2 - MAX Y, MIN Z, MAX X
//	//	if (Vertex.pos.y > VertexesOut[2].pos.y && Vertex.pos.z < VertexesOut[2].pos.z && Vertex.pos.x > VertexesOut[2].pos.x)
//	//	{
//	//		VertexesOut[2] = Vertex;
//	//	}
//	//	//3 - MAX Y, MIN Z, MIN X
//	//	if (Vertex.pos.y > VertexesOut[3].pos.y && Vertex.pos.z < VertexesOut[3].pos.z && Vertex.pos.x < VertexesOut[3].pos.x)
//	//	{
//	//		VertexesOut[3] = Vertex;
//	//	}
//	//	//4 - MIN Y, MIN Z, MIN X
//	//	if (Vertex.pos.y < VertexesOut[4].pos.y && Vertex.pos.z > VertexesOut[4].pos.z && Vertex.pos.x < VertexesOut[4].pos.x)
//	//	{
//	//		VertexesOut[4] = Vertex;
//	//	}
//	//	//5 - MIN Y, MAX Z, MIN X
//	//	if (Vertex.pos.y < VertexesOut[5].pos.y && Vertex.pos.z > VertexesOut[5].pos.z && Vertex.pos.x > VertexesOut[5].pos.x)
//	//	{
//	//		VertexesOut[5] = Vertex;
//	//	}
//	//	//6 - MIN Y, MIN Z, MAX X
//	//	if (Vertex.pos.y < VertexesOut[6].pos.y && Vertex.pos.z < VertexesOut[6].pos.z && Vertex.pos.x > VertexesOut[6].pos.x)
//	//	{
//	//		VertexesOut[6] = Vertex;
//	//	}
//	//	//7 - MIN Y, MIN Z, MIN X
//	//	if (Vertex.pos.y < VertexesOut[7].pos.y && Vertex.pos.z < VertexesOut[7].pos.z && Vertex.pos.x < VertexesOut[7].pos.x)
//	//	{
//	//		VertexesOut[7] = Vertex;
//	//	}
//	//}
//	
//
//}

void Mesh::Init()
{
	const auto& DX = DirectX12::GetInstance();

	// Initialize vertex buffer
	int VertexBufferSize = GetVertexBufferSize();
	// Create a default heap to hold the vertex buffer
	D3D12_HEAP_PROPERTIES defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(VertexBufferSize);
	DX.m_pDevice->CreateCommittedResource(
		&defaultHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_COMMON, // Start in common state
		nullptr,
		IID_PPV_ARGS(&vertexBuffer));

	// Create an upload heap for the vertex buffer
	D3D12_HEAP_PROPERTIES uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	DX.m_pDevice->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
		nullptr,
		IID_PPV_ARGS(&vBufferUploadHeap));
	vBufferUploadHeap->SetName(L"Vertex Buffer Upload Resource Heap");

	// Initialize index buffer
	int IndexBufferSize = GetIndexBufferSize();

	{
		auto Buffer = CD3DX12_RESOURCE_DESC::Buffer(IndexBufferSize);
		// Create a default heap to hold the index buffer
		DX.m_pDevice->CreateCommittedResource(
			&defaultHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&Buffer,
			D3D12_RESOURCE_STATE_COMMON, // Start in common state
			nullptr,
			IID_PPV_ARGS(&indexBuffer));
		indexBuffer->SetName(L"Index Buffer Resource Heap");
	}
	{
		auto Buffer = CD3DX12_RESOURCE_DESC::Buffer(IndexBufferSize);
		// Create an upload heap for the index buffer
		DX.m_pDevice->CreateCommittedResource(
			&uploadHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&Buffer,
			D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
			nullptr,
			IID_PPV_ARGS(&iBufferUploadHeap));
		iBufferUploadHeap->SetName(L"Index Buffer Upload Resource Heap");
	}
	// Initialize vertex buffer view
	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = sizeof(Vertex);
	vertexBufferView.SizeInBytes = VertexBufferSize;

	// Initialize index buffer view
	indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	indexBufferView.SizeInBytes = IndexBufferSize; // 32-bit unsigned integer (dword)
}

Mesh::~Mesh()
{
	if (vertexBuffer)
	{
		vertexBuffer->Release();
		vertexBuffer = nullptr;
	}

	// Release the index buffer
	if (indexBuffer)
	{
		indexBuffer->Release();
		indexBuffer = nullptr;
	}

	// Release the upload heaps, if they were used for initial data transfer
	if (vBufferUploadHeap)
	{
		vBufferUploadHeap->Release();
		vBufferUploadHeap = nullptr;
	}

	if (iBufferUploadHeap)
	{
		iBufferUploadHeap->Release();
		iBufferUploadHeap = nullptr;
	}
}

void Mesh::LoadFromFile(const std::string& FileName)
{
	
	//Load model
	ModelPredCompiler::Load(FileName, Vertexes, Indexes);
	
	Init();
}

void Mesh::Prepare(ID3D12GraphicsCommandList* _commandList)
{
	//Prepare the barrier to allow writes
	D3D12_RESOURCE_BARRIER Barriers[2];
	Barriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(
		indexBuffer, D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_COPY_DEST);
	Barriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(
		vertexBuffer, D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_COPY_DEST);
	_commandList->ResourceBarrier(2, Barriers);

	// Create subresource data for vertex buffer
	D3D12_SUBRESOURCE_DATA vertexData;
	vertexData.pData = reinterpret_cast<BYTE*>(Vertexes.data()); // Pointer to vertex array
	vertexData.RowPitch = GetVertexBufferSize(); // Size of vertex buffer
	vertexData.SlicePitch = GetVertexBufferSize(); // Same as row pitch

	// Copy vertex buffer data from upload heap to default heap
	UpdateSubresources(_commandList, vertexBuffer, vBufferUploadHeap, 0, 0, 1, &vertexData);

	// Create subresource data for index buffer
	D3D12_SUBRESOURCE_DATA indexData = {};
	indexData.pData = reinterpret_cast<BYTE*>(Indexes.data()); // Pointer to index array
	indexData.RowPitch = GetIndexBufferSize(); // Size of index buffer
	indexData.SlicePitch = GetIndexBufferSize(); // Same as row pitch

	// Copy index buffer data from upload heap to default heap
	UpdateSubresources(_commandList, indexBuffer, iBufferUploadHeap, 0, 0, 1, &indexData);

	// Transition index buffer to vertex and constant buffer state
	Barriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(
			indexBuffer, D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER | D3D12_RESOURCE_STATE_INDEX_BUFFER);

	// Transition vertex buffer to vertex and constant buffer state
	Barriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(
			vertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER | D3D12_RESOURCE_STATE_INDEX_BUFFER);
	_commandList->ResourceBarrier(2, Barriers);

	//We don't need vertexes anymore. TODO:// CHECK IF ITS SAFE TO REMOVE THIS!
	//Vertexes.clear();
	m_bIsDirty = false;
}