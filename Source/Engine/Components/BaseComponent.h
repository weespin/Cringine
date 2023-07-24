#pragma once
#include <vector>
#include "ComponentManager.h"
#include "EngineHeaders.h"
#include "PropertyManager.h"

struct Entity;

template <typename T>
struct ComponentRegistrar
{
	ComponentRegistrar(const char* pszTypeName, CreateComponentFunction FnCreatorFunction)
	{
		ComponentManager::RegisterComponent<T>(pszTypeName, FnCreatorFunction);
	}

	static ComponentRegistrar registrar;
};

template <typename T>
ComponentRegistrar<T> ComponentRegistrar<T>::registrar;

#define REGISTER_COMPONENT(Type) \
	static std::shared_ptr<Type> __Create##Type() { \
		return std::make_shared<Type>(); \
	} \
    static ComponentRegistrar<Type> registrar_##Type(#Type,reinterpret_cast<CreateComponentFunction>(&__Create##Type)); 

#define DEFINE_COMPONENT(Type, BaseType, Container) \
	class Type : public BaseType {\
	public: \
		virtual const char* GetTypeName() {return #Type;}; \
	private: \
	Container}; \
	REGISTER_COMPONENT(Type)

class BaseComponent : public IPropertyManager, public  ITypeNamed, public  std::enable_shared_from_this<BaseComponent>
{
public:
	virtual const char* GetTypeName() { return "BaseComponent"; }
	~BaseComponent() override;
	virtual void Init()
	{
		for(auto SubComponent : m_SubComponents)
		{
			SubComponent->Init();
		}
	}
	void Serialize(std::vector<unsigned char>& vector);
	void Deserialize(unsigned char** pData);;
	virtual bool ShouldTick() { return false; }
	//This function is called for data update (i.e. physics, logic)
	virtual void Update(float DeltaTime);
	//This function is called for after data update (i.e. prepare for render, etc.)
	virtual void PostUpdate();
	template<typename T>
	void AddSubComponent(std::shared_ptr<T> subComponent, const std::shared_ptr<Entity>& InParentEntity);
	void RemoveSubComponent(const std::shared_ptr<BaseComponent>& subComponent);

	std::weak_ptr<BaseComponent>& GetParent() { return m_ParentComponent; }
	std::weak_ptr<Entity>& GetOwnedEntity() { return m_ParentEntity; }
	const std::vector<std::shared_ptr<BaseComponent>>& GetSubComponents() const { return m_SubComponents; }
	std::weak_ptr<Entity> m_ParentEntity = {};
	std::weak_ptr<BaseComponent> m_ParentComponent = {};
	//Properties
	CREATE_PROPERTY_DEFAULT(std::string, m_Name)
private:

	std::vector<std::shared_ptr<BaseComponent>> m_SubComponents;
};

REGISTER_COMPONENT(BaseComponent);

template<typename T>
void BaseComponent::AddSubComponent(std::shared_ptr<T> subComponent, const std::shared_ptr<Entity>& InParentEntity)
{
	subComponent->m_ParentEntity = InParentEntity;
	m_SubComponents.push_back(subComponent);
	subComponent->m_ParentComponent = shared_from_this() ;
	subComponent->m_Name = typeid(T).name();
}