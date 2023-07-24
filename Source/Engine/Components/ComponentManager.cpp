#include "ComponentManager.h"

#include "Game.h"
#include "Engine/Renderer/DirectX12.h"


std::map<std::type_index, std::pair<std::string, CreateComponentFunction>> ComponentManager::RegisteredComponents{};
//std::mutex ComponentManager::registeredComponentsMutex;


void ComponentManager::GetAllSubComponents(const std::shared_ptr<BaseComponent>& InComponent, std::vector<std::shared_ptr<BaseComponent>>& allComponentsOut)
{
	allComponentsOut.push_back(InComponent);

	for (const auto& subComponent : InComponent->GetSubComponents())
	{
		GetAllSubComponents(subComponent, allComponentsOut);
	}
}

void ComponentManager::RemoveAllComponents()
{
	Components.clear();
}

std::shared_ptr<BaseComponent> ComponentManager::CreateComponent(const char* pszTypeName)
{
	for (const auto& [type, pair] : RegisteredComponents)
	{
		if (pair.first == pszTypeName)
		{
			return (*pair.second)();
		}
	}
	return nullptr;
}

std::shared_ptr<BaseComponent> ComponentManager::CreateComponent(const std::type_info& Intype_info)
{
	auto it = RegisteredComponents.find(Intype_info);
	if (it == RegisteredComponents.end())
	{
		return nullptr;
	}
	return (*RegisteredComponents[Intype_info].second)();
}


const std::vector<std::shared_ptr<BaseComponent>> ComponentManager::GetAllComponents() const
{
	std::vector<std::shared_ptr<BaseComponent>> ret;
	for(const auto& [type,component] : Components)
	{
		ret.insert(ret.end(), component);
	}
	return ret;
}
