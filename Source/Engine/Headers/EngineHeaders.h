#pragma once
#include <DirectXMath.h>

#include "PropertyManager.h"
using namespace DirectX;
#define ENCLOSE(...) __VA_ARGS__
struct Transform : public IPropertyManager
{
	//Todo: Convert to CREATE_PROPERTY back!
	XMFLOAT4 Position = {}; XMFLOAT4* Position_ptr = &Position; PropertyBinder<XMFLOAT4> Position_bind = PropertyBinder<XMFLOAT4>(Position_ptr, "Position", this);;

	XMFLOAT4& GetPosition() { return Position; } void SetPosition(const XMFLOAT4& value) { Position = value; };

	XMMATRIX Scale = DirectX::XMMatrixScaling(1.f, 1.f, 1.f); XMMATRIX* Scale_ptr = &Scale; PropertyBinder<XMMATRIX> Scale_bind = PropertyBinder<XMMATRIX>(Scale_ptr, "Scale", this);;

	XMMATRIX& GetScale() { return Scale; } void SetScale(const XMMATRIX& value) { Scale = value; };

	XMFLOAT4X4 RotMat = {}; XMFLOAT4X4* RotMat_ptr = &RotMat; PropertyBinder<XMFLOAT4X4> RotMat_bind = PropertyBinder<XMFLOAT4X4>(RotMat_ptr, "RotMat", this);;

	XMFLOAT4X4& GetRotMat() { return RotMat; } void SetRotMat(const XMFLOAT4X4& value) { RotMat = value; };

	XMFLOAT4X4 WorldMat = {}; XMFLOAT4X4* WorldMat_ptr = &WorldMat; PropertyBinder<XMFLOAT4X4> WorldMat_bind = PropertyBinder<XMFLOAT4X4>(WorldMat_ptr, "WorldMat", this);
	XMFLOAT4X4& GetWorldMat() { return WorldMat; } void SetWorldMat(const XMFLOAT4X4& value) { WorldMat = value; };



	Transform()
	{
		auto Rotation = DirectX::XMMatrixRotationRollPitchYaw(0.f, 0.f, 0.f);
		DirectX::XMStoreFloat4x4(&RotMat, Rotation);
	}

	void SetRotation(float pitch, float yaw, float roll)
	{
	   
	    const XMMATRIX Rotation = XMMatrixRotationRollPitchYaw(XMConvertToRadians(pitch), XMConvertToRadians(yaw), XMConvertToRadians(roll));
	    XMStoreFloat4x4(&RotMat, Rotation);
	}
	
	void Rotate(float roll, float pitch, float yaw)
	{
		XMMATRIX newRotation = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(pitch), DirectX::XMConvertToRadians(yaw), DirectX::XMConvertToRadians(roll));
		XMMATRIX accumulatedRotation = DirectX::XMMatrixMultiply(newRotation, DirectX::XMLoadFloat4x4(&RotMat));
		XMStoreFloat4x4(&RotMat, accumulatedRotation);
	}
	Transform operator+(const Transform& other) const
	{
		Transform result;
		
		// Add the position vectors
		result.Position.x = Position.x + other.Position.x;
		result.Position.y = Position.y + other.Position.y;
		result.Position.z = Position.z + other.Position.z;
		result.Position.w = Position.w + other.Position.w;
		
		// Multiply the scale matrices
		result.Scale = DirectX::XMMatrixMultiply(Scale, other.Scale);
		
		// Add the rotation matrices
		DirectX::XMMATRIX accumulatedRotation = DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&RotMat), DirectX::XMLoadFloat4x4(&other.RotMat));
		DirectX::XMStoreFloat4x4(&result.RotMat, accumulatedRotation);

		return result;
	}
};

// this is the structure of our constant buffer.
struct ConstantBuffer {
	XMFLOAT4X4 m_wvpMat;
	XMFLOAT4 m_color = XMFLOAT4(1.f, 1.f, 1.f, 1);
	float m_UseColorOverride = 0;
	float m_UseTexture = 0;
	float pad[2];
	XMFLOAT4X4 WorldMat;
	XMFLOAT3 dir;
	float pad1;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
};
constexpr size_t ConstantBufferPerObjectAlignedSize = (sizeof(ConstantBuffer) + 255) & ~255;

class ITypeNamed
{
public:
	virtual ~ITypeNamed() = default;

private:
	virtual const char* GetTypeName() = 0;
};