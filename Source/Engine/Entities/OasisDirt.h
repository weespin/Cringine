#pragma once
#include "Engine/Entities/Entity.h"

DEFINE_ENTITY(OasisDirt, StaticMeshEntity,
	ENCLOSE(
public:
	void Init() override
	{
		m_ModelName = "Oasis_Dirt.obj";
		StaticMeshEntity::Init();
	}
	void InitComponents() override
	{
		StaticMeshEntity::InitComponents();
		m_StaticMeshComponent->SetTextureName("oasis_Dirt");
	}
));