#pragma once
#include "Engine/Entities/Entity.h"

DEFINE_ENTITY(OasisSand, StaticMeshEntity,
	public:
		void Init() override
		{
			m_ModelName = "Oasis_Sand.obj";
			StaticMeshEntity::Init();
		}
		void InitComponents() override
		{
			StaticMeshEntity::InitComponents();
			m_StaticMeshComponent->SetTextureName("oasis_Sand");
		}
);