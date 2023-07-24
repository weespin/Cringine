#pragma once
#include <functional>
#include <map>
#include <mutex>
#include <set>
#include <typeindex>
#include <vector>

class BaseComponent;
class StaticMeshComponent;

struct Entity;
using CreateComponentFunction = std::shared_ptr<BaseComponent> (*)();
class ComponentManager
{
public:
	template <class T>
	void AddComponent(std::shared_ptr<T> InComponent, std::shared_ptr<Entity> ParentEntity, bool bDeferredInit = false);

	template<typename Component>
	void RemoveComponent(Component* InComponent);

	template<typename Component>
	bool HasComponent(Component* InComponent);

	template<typename Component>
	bool GetComponent(Component* InComponent);

	void GetAllSubComponents(const std::shared_ptr<BaseComponent>& InComponent, std::vector<std::shared_ptr<BaseComponent>>& allComponentsOut);
	void RemoveAllComponents();

	static std::shared_ptr<BaseComponent> CreateComponent(const char* pszTypeName);

	static std::shared_ptr<BaseComponent> CreateComponent(const std::type_info& Intype_info);

	template <class T>
	static void RegisterComponent(const char* psz_type_name, CreateComponentFunction fn_creator_function);

	static const std::map<std::type_index, std::pair<std::string, CreateComponentFunction>>& GetRegisteredComponents() { return RegisteredComponents; }
	const std::vector<std::shared_ptr<BaseComponent>> GetAllComponents() const;
private:
	
	std::map<std::type_index, std::shared_ptr<BaseComponent>> Components;
	static std::map<std::type_index,std::pair<std::string, CreateComponentFunction>> RegisteredComponents;
};

template <class T>
void ComponentManager::AddComponent(std::shared_ptr<T> InComponent, std::shared_ptr<Entity> ParentEntity, bool bDeferredInit)
{
	InComponent->m_ParentEntity = ParentEntity;
	Components[typeid(T)] = InComponent;
	if (!bDeferredInit)
	{
		InComponent->Init();
	}
	InComponent->m_Name = typeid(T).name();
}

template <class T>
void ComponentManager::RegisterComponent(const char* psz_type_name, CreateComponentFunction fn_creator_function)
{
	static_assert(std::is_base_of<BaseComponent, T>::value, "T must derive from BaseComponent");
	static bool bIsVectorInitialized = false;
	if(!bIsVectorInitialized)
	{
		//Prevent some compiler fucky wucky behaviour here.
		//std::vector<std::type_index> ComponentManager::RegisteredComponents{} is not initialized in ComponentManager.cpp even if we add "{}" tos its end.
		//And the engine will crash at _Orphan_range_unlocked. This is a wide known issue.
		bIsVectorInitialized = true;
		new (&RegisteredComponents) std::map<std::type_index, std::pair<std::string, CreateComponentFunction>>();
	}
	static std::mutex registeredComponentsMutex;
	std::lock_guard<std::mutex> lock(registeredComponentsMutex);

	std::type_index typeIndex(typeid(T));
	auto it = RegisteredComponents.find(typeIndex);
	if (it != RegisteredComponents.end())
	{
		return;
	}
	RegisteredComponents[typeIndex] = std::make_pair(std::string(psz_type_name), reinterpret_cast<CreateComponentFunction>(fn_creator_function));
}

