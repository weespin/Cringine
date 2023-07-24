#pragma once
#include "Entity.h"

//The logo belongs to Pingle Game Studio (https://pinglestudio.com/), a company that specializes in game development. 
//The 3D implementation of Pingle Game Studio Logo was made by Edspin (https://twitter.com/Edspin3).

DEFINE_ENTITY(PingleLogo, StaticMeshEntity,
	ENCLOSE(
public:

	void Init() override 
	{
		m_ModelName = "PingleLogo.obj";
		GetTransform().SetRotation(90, 0, 90.f);
		StaticMeshEntity::Init();
		//m_StaticMeshComponent->m_bUseColorOverride = true;
		//m_StaticMeshComponent->m_CustomColor = { 0.f,1.f,0.f };
	}

	void Update(float nDeltaTime) override;
	));
