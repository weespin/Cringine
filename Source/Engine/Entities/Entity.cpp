#include "Entity.h"

void Entity::Init()
{
	// load position vector into a XMVECTOR
	const XMVECTOR posVec = XMLoadFloat4(&m_Transform.GetPosition());

	// create a translation matrix from position vector
	const auto tmpMat = XMMatrixTranslationFromVector(posVec);

	// initialize rotation matrix to identity matrix
	XMStoreFloat4x4(&m_Transform.GetRotMat(), XMMatrixIdentity());

	// store world matrix as a combination of translation and rotation
	XMStoreFloat4x4(&m_Transform.GetWorldMat(), tmpMat);
}

void Entity::Update(float DeltaTime)
{
	for (auto element : GetComponentManager().GetAllComponents())
	{
		element->Update(DeltaTime);
	}
}

void Entity::Deserialize(unsigned char** pData)
{
	//Populate Properties, woohoo :^(
	unsigned long nProperties;
	memcpy(&nProperties, *pData, sizeof(unsigned long));
	*pData += sizeof(unsigned long);
	for (unsigned long nProperty = 0; nProperty < nProperties; ++nProperty)
	{
		DeserializeValue(pData, this);
	}
	
	//Deserialize Components, gonna kms.
	//I hope this 2 functions will ever compile
	unsigned long nComponents;
	memcpy(&nComponents, *pData, sizeof(unsigned long));
	*pData += sizeof(unsigned long);
	for (unsigned long nComponent = 0; nComponent < nComponents; ++nComponent)
	{
		unsigned long nComponentTypeLen;
		memcpy(&nComponentTypeLen, *pData, sizeof(unsigned long));
		*pData += sizeof(unsigned long);
		std::string ComponentType;
		ComponentType.resize(nComponentTypeLen, '\0');
		memcpy(ComponentType.data(), *pData, nComponentTypeLen);
		*pData += nComponentTypeLen;

		auto NewComponent = ComponentManager::CreateComponent(ComponentType.c_str());
		m_ComponentManager.AddComponent(NewComponent, shared_from_this(),true);
		//Component, resolve yourself
		NewComponent->Deserialize(pData);
	}
	Init();
}


