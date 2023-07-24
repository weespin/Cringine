#include "BBOXVisualizerComponent.h"
#include <DirectXMath.h>
#include <intsafe.h>

#include "Application.h"
#include "Renderer/Mesh.h"

void BBOXVisualizerComponent::Update(float DeltaTime)
{
	StaticMeshComponent::Update(DeltaTime);
	//GetTransform().Rotate(0, 0, 1);
}

void BBOXVisualizerComponent::Init()
{
	StaticMeshComponent::Init();
	SetRenderMode("wireframe");
	//LoadBBOX
	static DWORD BoxIndexes[] = {
	0, 1, 2, 2, 3, 0,
	1, 5, 6, 6, 2, 1,
	6, 7, 3, 3, 2, 6,
	4, 5, 1, 1, 0, 4,
	7, 6, 5, 5, 4, 7,
	3, 7, 4, 4, 0, 3,
	0, 4, 5, 5, 1, 0
	};
	const auto& Parent = GetParent().lock();
	const auto& ParentRenderableComponent = std::dynamic_pointer_cast<StaticMeshComponent>(Parent);
	const auto& ParentVertexes = ParentRenderableComponent->GetMesh()->GetVertexes();
	unsigned dataArraySize = sizeof(BoxIndexes) / sizeof(DWORD);
	GetMesh()->GetIndexes().insert(GetMesh()->GetIndexes().end(), &BoxIndexes[0], &BoxIndexes[dataArraySize]);

	XMFLOAT3 bboxmin = ParentVertexes[0].pos;
	XMFLOAT3 bboxmax = ParentVertexes[0].pos;

	// Iterate over the remaining vertices and update bboxmin and bboxmax
	for (const auto& Vertex : ParentVertexes)
	{
		bboxmin.x = min(bboxmin.x, Vertex.pos.x);
		bboxmin.y = min(bboxmin.y, Vertex.pos.y);
		bboxmin.z = min(bboxmin.z, Vertex.pos.z);

		bboxmax.x = max(bboxmax.x, Vertex.pos.x);
		bboxmax.y = max(bboxmax.y, Vertex.pos.y);
		bboxmax.z = max(bboxmax.z, Vertex.pos.z);
	}
	auto& OurVertexes = GetMesh()->GetVertexes();
	OurVertexes.resize(8);
	OurVertexes[0].pos = XMFLOAT3(bboxmin.x - GetBBOXAdjustment(), bboxmax.y + GetBBOXAdjustment(), bboxmin.z - GetBBOXAdjustment());
	OurVertexes[1].pos = XMFLOAT3(bboxmax.x + GetBBOXAdjustment(), bboxmax.y + GetBBOXAdjustment(), bboxmin.z - GetBBOXAdjustment());
	OurVertexes[2].pos = XMFLOAT3(bboxmax.x + GetBBOXAdjustment(), bboxmin.y - GetBBOXAdjustment(), bboxmin.z - GetBBOXAdjustment());
	OurVertexes[3].pos = XMFLOAT3(bboxmin.x - GetBBOXAdjustment(), bboxmin.y - GetBBOXAdjustment(), bboxmin.z - GetBBOXAdjustment());
	OurVertexes[4].pos = XMFLOAT3(bboxmin.x - GetBBOXAdjustment(), bboxmax.y + GetBBOXAdjustment(), bboxmax.z + GetBBOXAdjustment());
	OurVertexes[5].pos = XMFLOAT3(bboxmax.x + GetBBOXAdjustment(), bboxmax.y + GetBBOXAdjustment(), bboxmax.z + GetBBOXAdjustment());
	OurVertexes[6].pos = XMFLOAT3(bboxmax.x + GetBBOXAdjustment(), bboxmin.y - GetBBOXAdjustment(), bboxmax.z + GetBBOXAdjustment());
	OurVertexes[7].pos = DirectX::XMFLOAT3(bboxmin.x - GetBBOXAdjustment(), bboxmin.y - GetBBOXAdjustment(), bboxmax.z + GetBBOXAdjustment());
	//GetTransform().Position.x += 10;
	GetMesh()->Init();
}
