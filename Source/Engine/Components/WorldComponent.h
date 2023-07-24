#pragma once
#include "BaseComponent.h"
#include "Engine/Headers/EngineHeaders.h"

//Only transform
DEFINE_COMPONENT(WorldComponent, BaseComponent,

public:
	Transform& GetTransform() { return m_Transform; }
	bool IsRelativeTransform() { return m_bRelativeTransform; }
	void SetRelativeTransform(bool bRelative) { m_bRelativeTransform = bRelative; }
private:
	CREATE_PROPERTY(bool, m_bRelativeTransform, true);
	CREATE_PROPERTY_DEFAULT(Transform, m_Transform);
);