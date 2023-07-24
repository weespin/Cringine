#pragma once
#include "Engine/Entities/Entity.h"

DEFINE_ENTITY(OasisWater, StaticMeshEntity,
ENCLOSE(
public:
	OasisWater() {};
	void Init() override
	{
		m_ModelName = "Oasis_Water.obj";
		StaticMeshEntity::Init();
	}
	void InitComponents() override
	{
		StaticMeshEntity::InitComponents();
		m_StaticMeshComponent->SetTextureName("oasis_Water");
	}
	)
);