#pragma once
#include <string>

#include "PrimitiveComponent.h"

DEFINE_COMPONENT(TexturedMeshComponent, PrimitiveComponent,

public:
	void SetTextureName(const std::string & TextureName);
	void ResetTexture() { m_TextureName = ""; }
	void Render() {};
	const std::string& GetTextureName() const { return m_TextureName; }
	CREATE_PROPERTY(std::string, m_TextureName,{});

)