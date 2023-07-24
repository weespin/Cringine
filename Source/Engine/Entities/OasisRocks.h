#pragma once
#include "Engine/Entities/Entity.h"

DEFINE_ENTITY(OasisRocks, StaticMeshEntity,
ENCLOSE(
public:
	void Init() override
	{
		m_ModelName = "Oasis_Rocks.obj";
		StaticMeshEntity::Init();
	}
	void InitComponents() override
	{
		StaticMeshEntity::InitComponents();
		m_StaticMeshComponent->SetTextureName("oasis_Rock");
	}
	)
);
