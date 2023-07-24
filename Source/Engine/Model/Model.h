#pragma once

//class Model
//{
//
////public:
////	Model();
////
////	void SetTextureName(const std::string& TextureName);
////	void ResetTexture() { m_TextureName = ""; }
////	const std::string& GetTextureName() const { return m_TextureName; }
////	bool ShouldDrawBBOX() { return m_bDrawBBOX; };
////
////	void SetCustomColor(XMFLOAT4 CustomColor);
////	void UseColorOverride(bool InUseColorOverride);
////	void UseTexture(bool InUseTexture);
////
////	Mesh* GetMesh() const { return m_Mesh; }
////	void SetMesh(Mesh* InMesh) { m_Mesh = InMesh; }
////	
////	int GetConstantBufferOffset() const { return m_nConstantBufferMemoryOffset; }
////	ConstantBuffer& GetConstantBuffer() { return m_ConstantBuffer; }
////
////	std::string GetRenderMode() const { return RenderMode; }
////	void SetRenderMode(const std::string& newRenderMode) { RenderMode = newRenderMode; }
////	
////private:
////	ConstantBuffer m_ConstantBuffer{};
////	int m_nConstantBufferMemoryOffset;
////	std::string m_TextureName;
////	Mesh* m_Mesh{};
////	std::string RenderMode = "default";
////	bool m_bDrawBBOX = true;
//};