#pragma once
#include "StaticMeshComponent.h"

//This component should have an parent of type "StaticMeshComponent"
DEFINE_COMPONENT(BBOXVisualizerComponent, StaticMeshComponent,

public:
	float GetBBOXAdjustment() { return m_BBOXAdjustment; }
	void Update(float DeltaTime) override;
	void Init()  override;
	//void Render() override {};

	CREATE_PROPERTY(float, m_BBOXAdjustment, 0.3f);
);