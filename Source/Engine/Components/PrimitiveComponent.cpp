
#include "PrimitiveComponent.h"

#include "EngineHeaders.h"
#include "Game.h"
#include "Engine/Entities/Entity.h"
#include "Engine/Renderer/DirectX12.h"
#include "Renderer/Mesh.h"


void PrimitiveComponent::LoadModel(const char* pszModelName)
{
	GetMesh()->LoadFromFile(pszModelName);
}

PrimitiveComponent::PrimitiveComponent()
{
	//TODO: Implement static meshes
	static int cBufferCounter = 0;
	m_nConstantBufferMemoryOffset = cBufferCounter++;
	m_Mesh = new Mesh();
}
PrimitiveComponent::~PrimitiveComponent()
{
	delete m_Mesh;
	m_Mesh = nullptr;
}

void PrimitiveComponent::PostUpdate()
{
	WorldComponent::PostUpdate();
	const auto& OwnedEntity = GetOwnedEntity().lock();
	Transform FinalTransform = IsRelativeTransform() ? OwnedEntity->GetTransform() + GetTransform() : GetTransform();
	
	// Calculate the new rotation matrix by multiplying the current rotation matrix by the rotation quaternion
	const XMMATRIX rotMat = XMLoadFloat4x4(&FinalTransform.GetRotMat());

	// Create a translation matrix based on the entity's position vector
	const XMMATRIX translationMat = XMMatrixTranslationFromVector(XMLoadFloat4(&FinalTransform.GetPosition()));
	// Create the world matrix by first applying the rotation, then the translation, and finally the scale
	const XMMATRIX worldMat = rotMat * translationMat * FinalTransform.GetScale();
	// Store the new world matrix in the entity's transform
	XMStoreFloat4x4(&FinalTransform.GetWorldMat(), worldMat);

	// Load the camera's view and projection matrices
	const XMMATRIX& viewMat = XMLoadFloat4x4(&Game::GetInstance().Camera.cameraViewMat);
	const XMMATRIX& projMat = XMLoadFloat4x4(&Game::GetInstance().Camera.cameraProjMat);

	// Calculate the final WVP matrix by multiplying the world matrix by the view and projection matrices
	const XMMATRIX wvpMat = worldMat * viewMat * projMat;
	// Transpose the WVP matrix for compatibility with the GPU
	const XMMATRIX transposed = XMMatrixTranspose(wvpMat);
	// Store the transposed WVP matrix in the entity's constant buffer
	XMStoreFloat4x4(&GetConstantBuffer().m_wvpMat, transposed);
	// Copy the entity's constant buffer to the GPU's constant buffer
	// Calculate the offset in the constant buffer based on the entity's memory offset
	// Note: ConstantBufferPerObjectAlignedSize is the size of each object's constant buffer in bytes, and m_nFrameIndex is the current frame index used by the GPU
	// Update Constant Buffer stuff
	const XMMATRIX transposedworld = XMMatrixTranspose(worldMat);
	// Store the transposed WVP matrix in the entity's constant buffer
	XMStoreFloat4x4(&GetConstantBuffer().WorldMat, transposedworld);
	auto& SLight = Game::GetInstance().light;
	m_ConstantBuffer.diffuse = SLight.diffuse;
	m_ConstantBuffer.ambient = SLight.ambient;
	m_ConstantBuffer.dir = SLight.dir;
	m_ConstantBuffer.m_UseColorOverride = m_bUseColorOverride ? 1.f : 0.f;
	m_ConstantBuffer.m_UseTexture = m_UseTexture ? 1.f : 0.f;
	m_ConstantBuffer.m_color.x = m_CustomColor.R;
	m_ConstantBuffer.m_color.y = m_CustomColor.G;
	m_ConstantBuffer.m_color.z = m_CustomColor.B;
	m_ConstantBuffer.m_color.w = m_CustomColor.A;
	memcpy(DirectX12::GetInstance().m_pCBVGPUAddress[DirectX12::GetInstance().m_nFrameIndex] + (ConstantBufferPerObjectAlignedSize * GetConstantBufferOffset()), &GetConstantBuffer().m_wvpMat, sizeof(GetConstantBuffer()));
}


