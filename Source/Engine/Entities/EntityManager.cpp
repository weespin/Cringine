#include <map>
#include <string>
#include <typeindex>
#include "Entities/Entity.h"
#include "EntityManager.h"
std::map<std::type_index, std::pair<std::string, CreateEntityFunction>> EntityManager::RegisteredEntities{};

void EntityManager::AddEntity(std::shared_ptr<Entity> pEntity, const char* type)
{
	logger.Log(DEBUG, "Adding entity #%i (%s)", m_Entities.size(), type);
	pEntity->SetName(type);
	m_Entities.push_back(pEntity); //std::move?
	pEntity->Init();
	pEntity->CreateComponents();
	pEntity->InitComponents();
}

void EntityManager::PushEntity(std::shared_ptr<Entity>& pEntity)
{
	m_Entities.push_back(std::move(pEntity));
}

void EntityManager::ResetEntities()
{
	//Super stupid workaround. TODO Create a god damn cameracomponent!
	m_Entities.erase(
		std::remove_if(m_Entities.begin(), m_Entities.end(),
			[](const std::shared_ptr<Entity>& entity) {
				return !strstr(entity->GetTypeName(), "Camera");
			}),
		m_Entities.end()
	);
	//for (auto Entity : m_Entities)
	//{
	//	//TODO: Camera should be included in EntityList!
	//	if(strstr(Entity->GetTypeName(),"Camera"))
	//	{
	//		continue;
	//	}
	//	delete Entity;
	//}
	//m_Entities.clear();
}

void EntityManager::RemoveEntity(const std::shared_ptr<Entity>& pEntity)
{
	m_Entities.erase(
		std::remove_if(m_Entities.begin(), m_Entities.end(),
			[&](const std::shared_ptr<Entity>& entity) {
				return entity.get() == pEntity.get();
			}),
		m_Entities.end()
	);
}

//void EntityManager::RemoveEntity(Entity* pEntity)
//{
//	Entities.erase(std::remove(Entities.begin(), Entities.end(), pEntity), Entities.end());
//}
