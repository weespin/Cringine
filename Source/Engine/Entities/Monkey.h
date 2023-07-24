#pragma once
#include "Entity.h"

DEFINE_ENTITY(Monkey, StaticMeshEntity,
	ENCLOSE(
public:
	void Init() override
	{
		m_ModelName = "Monkey.obj";
		StaticMeshEntity::Init();
		//m_StaticMeshComponent->SetRenderMode("wireframe");
		//m_StaticMeshComponent->m_bUseColorOverride = true;
		//m_StaticMeshComponent->m_CustomColor = { 0.f,0.f,1.f };
	}
	void Update(float nDeltaTime) override
	{
		__super::Update(nDeltaTime);

		GetTransform().Rotate(0.f, 1.f, 0.f);
	}
));
