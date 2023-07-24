#pragma once
#include <map>
#include <string>
#include <vector>

#include <typeindex>

#include "Utils/Logger.h"
class Camera;
struct Entity;
using CreateEntityFunction = std::shared_ptr<Entity> (*)();
//This class is not a real manager. It can be reused for something else later but i'll use it to register entity types for a factory.
class EntityManager
{
public:
	//This shouldn't be called from deserialize function! Use PushEntity for that!
	void AddEntity(std::shared_ptr<Entity> pEntity, const char* type);
	void PushEntity(std::shared_ptr<Entity>& pEntity);
	const std::vector<std::shared_ptr<Entity>>& GetEntites() const { return m_Entities; }
	void ResetEntities();
	void RemoveEntity(const std::shared_ptr<Entity>& pEntity);
	//Factories without switch cases :^)
	static std::shared_ptr<Entity> CreateEntitiy(const char* pszName)
	{
		for (const auto& [type, pair] : RegisteredEntities)
		{
			if (pair.first == pszName)
			{
				return (*pair.second)();
			}
		}
		return nullptr;
	}
	
	static std::shared_ptr<Entity> CreateEntitiy(const std::type_info& Intype_info)
	{
		auto it = RegisteredEntities.find(Intype_info);
		if (it == RegisteredEntities.end())
		{
			return nullptr;
		}
		return (*RegisteredEntities[Intype_info].second)();
	}
	template <class T>
	static void RegisterEntity(const char* psz_type_name, CreateEntityFunction fn_creator_function)
	{
		static_assert(std::is_base_of<Entity, T>::value, "T must derive from Entity");
		static bool bIsVectorInitialized = false;
		if (!bIsVectorInitialized)
		{
			//Prevent some MSVC fucky wucky here.
			//std::vector<std::type_index> EntityManager::RegisteredEntities{} is not initialized in ComponentManager.cpp even if we add "{}" it its end.
			//And the engine will crash at _Orphan_range_unlocked. This is a wide known issue.
			bIsVectorInitialized = true;
			new (&RegisteredEntities) std::map<std::type_index, std::pair<std::string, CreateEntityFunction>>();
		}
		static std::mutex registeredComponentsMutex;
		std::lock_guard<std::mutex> lock(registeredComponentsMutex);

		std::type_index typeIndex(typeid(T));
		auto it = RegisteredEntities.find(typeIndex);
		if (it != RegisteredEntities.end())
		{
			return;
		}
		RegisteredEntities[typeIndex] = std::make_pair(std::string(psz_type_name), reinterpret_cast<CreateEntityFunction>(fn_creator_function));
	}
	template <class T>
	static std::string GetEntityTypeName()
	{
		//static_assert(std::is_base_of<Entity, T>::value, "T must derive from Entity");
		std::type_index typeIndex(typeid(T));
		return RegisteredEntities[typeIndex].first;
	}
	static std::string GetEntityTypeName(std::type_index index)
	{
		//static_assert(std::is_base_of<Entity, T>::value, "T must derive from Entity");
		return RegisteredEntities[index].first;
	}
	static std::map<std::type_index, std::pair<std::string, CreateEntityFunction>> GetRegisteredEntities() { return RegisteredEntities; }
	static std::vector<std::string> GetRegisteredEntitiesTypes()
	{
		std::vector<std::string> ret;
		for (const auto& [type, pair] : RegisteredEntities)
		{
			ret.emplace_back(pair.first);
		}
		return ret;
	}
private:
	static std::map<std::type_index, std::pair<std::string, CreateEntityFunction>> RegisteredEntities;
	std::vector<std::shared_ptr<Entity>> m_Entities;
};