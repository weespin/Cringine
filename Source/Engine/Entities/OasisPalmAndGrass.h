#pragma once
#include "Engine/Entities/Entity.h"

DEFINE_ENTITY(OasisPalmAndGrass, StaticMeshEntity, ENCLOSE(
public:
	void Init() override
	{
		m_ModelName = "Oasis_PalmAndGrass.obj";
		StaticMeshEntity::Init();
	}
	void InitComponents() override
	{
		StaticMeshEntity::InitComponents();
		m_StaticMeshComponent->SetTextureName("oasis_PalmTree");
	}
));

