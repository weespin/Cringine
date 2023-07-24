#pragma once
#include "TexturedMeshComponent.h"

class StaticMeshComponent : public TexturedMeshComponent
{
public:
	const char* GetTypeName() override { return "StaticMeshComponent"; };

private:
public:
	std::string m_ModelName = {};
	std::string* m_ModelName_ptr = &m_ModelName;
	PropertyBinder<std::string> m_ModelName_bind = PropertyBinder<std::string>(m_ModelName_ptr, "m_ModelName", this);;

	void Init() override
	{
		
		if (m_ModelName.size()) { LoadModel(m_ModelName.c_str()); }
		TexturedMeshComponent::Init();
	}

public:
private:
};

static std::shared_ptr<StaticMeshComponent> __CreateStaticMeshComponent()
{
	return std::make_shared<StaticMeshComponent>();
}

static ComponentRegistrar<StaticMeshComponent> registrar_StaticMeshComponent(
	"StaticMeshComponent", reinterpret_cast<CreateComponentFunction>(&__CreateStaticMeshComponent));;
