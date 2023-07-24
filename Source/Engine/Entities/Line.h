#pragma once
#include "Entity.h"

DEFINE_ENTITY(Line,  public Entity,
	ENCLOSE(
public:
	bool CanUpdate() const override { return true; }
	
	void Init() override
	{
		Entity::Init();
		m_Model = new Model();
		m_Model->Indexes = {
		0, 1, 2, // first triangle
		0, 3, 1, // second triangle
		};
		m_Model->Vertexes =
		{ { -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.f, 0.f },
		{  0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.f, 0.f },
		{ -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.f, 0.f },
		{  0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.f, 0.f }
		};

		GetModel()->Init();
	};
));