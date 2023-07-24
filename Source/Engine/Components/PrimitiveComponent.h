#pragma once
#include <string>

#include "WorldComponent.h"
#include "Engine/Headers/EngineHeaders.h"
#include "Utils/ColorHelper.h"
class Mesh;

//Has a render function and can be drawn on screen
DEFINE_COMPONENT(PrimitiveComponent, WorldComponent,

public:
	PrimitiveComponent();
	~PrimitiveComponent() override;

	//virtual void Render() = 0;
	//bool IsVisible() { return m_bIsVisible; }
	//void SetVisibility(bool bNewVisibility) { m_bIsVisible = bNewVisibility; }
	//Every primitive can be drawn so it need a constant buffer
	int GetConstantBufferOffset() const { return m_nConstantBufferMemoryOffset; }
	ConstantBuffer& GetConstantBuffer() { return m_ConstantBuffer; }
	std::string GetRenderMode() const { return RenderMode; }
	void SetRenderMode(const std::string& newRenderMode) { RenderMode = newRenderMode; }
	void LoadModel(const char* pszModelName);
	Mesh* GetMesh() const { return m_Mesh; }
	void SetTextureName(const std::string& TextureName);
	void SetMesh(Mesh* InMesh) { m_Mesh = InMesh; }
	void PostUpdate() override;
	CREATE_PROPERTY(bool, m_bUseColorOverride, false);
	CREATE_PROPERTY(ColorHelper::RGBA, m_CustomColor, {});
	CREATE_PROPERTY(bool, m_UseTexture, false);

private:
	ConstantBuffer m_ConstantBuffer{};
	int m_nConstantBufferMemoryOffset = 0;
	//bool m_bIsVisible = true;
	CREATE_PROPERTY(std::string, RenderMode , "default");
	
	Mesh* m_Mesh = nullptr;
)