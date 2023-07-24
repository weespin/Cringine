#include "TexturedMeshComponent.h"

#include <string>

void TexturedMeshComponent::SetTextureName(const std::string& TextureName)
{
	m_TextureName = TextureName;
	m_UseTexture = true;
	//GetConstantBuffer().UseTexture(true);
}
