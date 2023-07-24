#pragma once
#include "Entity.h"

//This entity is a meme to test UV mapping + textures. Pingle, please don't laugh too much :D
DEFINE_ENTITY(WilliamSphere, StaticMeshEntity,

public:
	void Init() override
	{
		m_ModelName = "sphere.obj";
		StaticMeshEntity::Init();
	}
	void Update(float nDeltaTime) override;
	void InitComponents() override
	{
		StaticMeshEntity::InitComponents();
		m_StaticMeshComponent->SetTextureName("sphere");
	}
	);