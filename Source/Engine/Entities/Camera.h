#pragma once
#include "Entity.h"
#include "Engine/Input/InputEventable.h"

DEFINE_ENTITY(Camera, ENCLOSE(Entity, InputEventable),

public:
	Camera();
	void Update(float nDeltaTime) override;
	void SetFOV(float x);
	void Initialize();

	//todo add getters
	XMFLOAT4X4 cameraProjMat = {}; // this will store our projection matrix
	XMFLOAT4X4 cameraViewMat = {}; // this will store our view matrix
	XMVECTOR cameraTarget = {}; // a vector describing the point in space our camera is looking at
	XMFLOAT4 cameraUp = {}; // the worlds up vector
	float camPitch = 0.f;
	float camYaw = 0.f;
	float camRoll = 0.f;
	
	XMVECTOR DefaultForward = XMVectorSet(0.0f ,0.0f, 1.0f, 0.0f);
	XMVECTOR DefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR camUp = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	float m_FOV = 90.f;
private:
	void OnMouseMove(int dx,int dy);
	void OnKeyHold(char Code);
)
