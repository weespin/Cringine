#include "BaseComponent.h"

BaseComponent::~BaseComponent()
{
	m_SubComponents.clear();
}

void BaseComponent::Serialize(std::vector<unsigned char>& dataOut)
{
	//Type
	const char* type = GetTypeName();
	unsigned long TypeSize = strlen(type);
	dataOut.insert(dataOut.end(), INSERT_TO_UC_VECTOR(&TypeSize, sizeof(TypeSize)));
	dataOut.insert(dataOut.end(), INSERT_CONST_TO_UC_VECTOR(type, TypeSize));
	//Properties
	auto Properties = GetAllProperties();
	unsigned long PropertiesSize = Properties.size();
	dataOut.insert(dataOut.end(), INSERT_TO_UC_VECTOR(&PropertiesSize, sizeof(PropertiesSize)));
	for (auto [name, value] : Properties)
	{
		auto type_name = name;
		unsigned long type_name_size = type_name.size();
		dataOut.insert(dataOut.end(), INSERT_TO_UC_VECTOR(&type_name_size, sizeof(type_name_size)));
		dataOut.insert(dataOut.end(), INSERT_CONST_TO_UC_VECTOR(type_name.c_str(), type_name_size));
		SerializeValue(value, dataOut);
	}
	//SubProperties
	auto Components = GetSubComponents();
	unsigned long nComponents = Components.size();
	dataOut.insert(dataOut.end(), INSERT_CONST_TO_UC_VECTOR(&nComponents, sizeof(nComponents)));
	for (auto Component : Components)
	{
		Component->Serialize(dataOut);
	}
}

void BaseComponent::Deserialize(unsigned char** pData)
{
	unsigned long nProperties;
	memcpy(&nProperties, *pData, sizeof(unsigned long));
	*pData += sizeof(unsigned long);

	for (unsigned long nProperty = 0; nProperty < nProperties; ++nProperty )
	{
		DeserializeValue(pData, this);
	}
	Init();
	//SubComponents

	unsigned long nSubComponents;
	memcpy(&nSubComponents, *pData, sizeof(unsigned long));
	*pData += sizeof(unsigned long);
	for(unsigned long nSubComponent = 0; nSubComponent < nSubComponents; ++nSubComponent)
	{
		unsigned long nComponentTypeLen;
		memcpy(&nComponentTypeLen, *pData, sizeof(unsigned long));
		*pData += sizeof(unsigned long);
		std::string ComponentType;
		ComponentType.resize(nComponentTypeLen, '\0');
		memcpy(ComponentType.data(), *pData, nComponentTypeLen);
		*pData += nComponentTypeLen;

		auto NewComponent = ComponentManager::CreateComponent(ComponentType.c_str());
		//TODO LOCK :(
		AddSubComponent(NewComponent, GetOwnedEntity().lock());
		//Component, resolve yourself
		NewComponent->Deserialize(pData);
	}
}

void BaseComponent::Update(float DeltaTime)
{
	for (const auto& subComponent : m_SubComponents)
	{
		subComponent->Update(DeltaTime);
	}
}

void BaseComponent::PostUpdate()
{
	for (const auto& subComponent : m_SubComponents)
	{
		subComponent->PostUpdate();
	}
}



void BaseComponent::RemoveSubComponent(const std::shared_ptr<BaseComponent>& subComponent)
{
	auto it = std::find(m_SubComponents.begin(), m_SubComponents.end(), subComponent);
	if (it != m_SubComponents.end()) {
		m_SubComponents.erase(it);
	}
}