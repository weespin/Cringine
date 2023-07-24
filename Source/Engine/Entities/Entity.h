#pragma once
#include <DirectXMath.h>

#include "../Model/Model.h"
#include "Engine/Components/ComponentManager.h"

#include "Engine/Components/BBOXVisualizerComponent.h"
#include "Engine/Headers/EngineHeaders.h"
#include "EntityManager.h"
using namespace DirectX; // we will be using the directxmath library
template <typename T>
struct EntityRegistrar
{
	EntityRegistrar(const char* pszTypeName, CreateEntityFunction FnCreatorFunction)
	{
		EntityManager::RegisterEntity<T>(pszTypeName, FnCreatorFunction);
	}

	static EntityRegistrar registrar;
};

template <typename T>
EntityRegistrar<T> EntityRegistrar<T>::registrar;

#define REGISTER_ENTITY(Type) \
	static std::shared_ptr<Type> __Create##Type() { \
		return std::make_shared<Type>(); \
	} \
    static EntityRegistrar<Type> registrar_##Type(#Type,reinterpret_cast<CreateEntityFunction>(&__Create##Type));

#define DEFINE_ENTITY(Type, BaseType, Container) \
	class Type : public BaseType {\
	public: \
		virtual const char* GetTypeName() {return #Type;}; \
	private: \
	Container}; \
	REGISTER_ENTITY(Type)

//#define DEFINE_COMPONENT(Type, BaseType, Container) \
//	class Type : public BaseType \
//	Container; \
//	REGISTER_COMPONENT(Type)
//

//Please don't use any constructors.
//We can pass almost any data as PROPERTY and when loading an entity we use Init to load all deserialized data!
struct Entity : IPropertyManager, std::enable_shared_from_this<Entity>
{
	//The main reason to use init because 
	virtual void Init();
	virtual void CreateComponents() {};
	virtual void InitComponents()
	{
		for (auto& Component : m_ComponentManager.GetAllComponents())
		{
			Component->Init();
		}
	};
	virtual const char* GetTypeName() { return "Entity"; }
	virtual ~Entity()
	{
		m_ComponentManager.RemoveAllComponents();
	}
	void Serialize(std::vector<unsigned char>& dataOut)
	{
		//Type
		const char* type = GetTypeName();
		unsigned long TypeSize = strlen(type);
		dataOut.insert(dataOut.end(), INSERT_TO_UC_VECTOR(&TypeSize, sizeof(TypeSize)));
		dataOut.insert(dataOut.end(), INSERT_CONST_TO_UC_VECTOR(type,TypeSize));
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
		//Serialize Components
		auto Components = m_ComponentManager.GetAllComponents();
		unsigned long nComponents = Components.size();
		dataOut.insert(dataOut.end(), INSERT_CONST_TO_UC_VECTOR(&nComponents, sizeof(nComponents)));
		for(auto Component : Components)
		{
			Component->Serialize(dataOut);
		}
	}
	bool IsVisible() { return m_bIsVisible; }
	void SetVisibility(bool bIsVisible) { m_bIsVisible = bIsVisible; }
	//Set override this and set it to false only if you don't want to use the model as static, otherwise we will cache the model and improve the memory performance! 
	virtual bool UseStaticModel() { return true; }
	

	// Returns whether the entity should be updated or not
	bool CanUpdate() const { return m_bCanUpdate; }

	// Updates the entity
	virtual void Update(float DeltaTime);
	void SetName(const char* Name) { m_Name = Name; }
	std::string GetName() { return m_Name; }


	virtual void PostUpdate()
	{
		for (auto element : GetComponentManager().GetAllComponents())
		{
			element->PostUpdate();
		}
	}

	void Deserialize(unsigned char** p_data);;
	virtual float CustomIntervalRate() { return 0; };

public:
	CREATE_PROPERTY(Transform, m_Transform,{});
	Transform& GetTransform() { return m_Transform; };
	ComponentManager& GetComponentManager()
	{
		return m_ComponentManager;
	}
	ComponentManager m_ComponentManager;
	CREATE_PROPERTY(bool, m_bIsVisible,true);
	CREATE_PROPERTY(std::string,m_Name,"");
	CREATE_PROPERTY(bool, m_bCanUpdate, true);
};
REGISTER_ENTITY(Entity);
//Use this class if you want to render an simple static object without collision/physics
struct StaticMeshEntity : public Entity
{
	void Init() override
	{
		__super::Init();
		
	}
	void CreateComponents() override
	{
		Entity::CreateComponents();
		m_StaticMeshComponent = std::make_shared<StaticMeshComponent>();
		GetComponentManager().AddComponent<StaticMeshComponent>(m_StaticMeshComponent, this->shared_from_this(), true);
		//m_BBOXVisualizerComponent = std::make_shared<BBOXVisualizerComponent>();
		//m_StaticMeshComponent->AddSubComponent(m_BBOXVisualizerComponent, this->shared_from_this());
	}
	void InitComponents() override
	{
		m_StaticMeshComponent->m_ModelName = m_ModelName;
		Entity::InitComponents();
		//m_StaticMeshComponent->LoadModel(m_ModelName.c_str());
		m_StaticMeshComponent->Init();
	}
	std::shared_ptr<StaticMeshComponent> m_StaticMeshComponent = nullptr;
	std::shared_ptr<BBOXVisualizerComponent> m_BBOXVisualizerComponent = nullptr;
	CREATE_PROPERTY(std::string, m_ModelName, "");
};
REGISTER_ENTITY(StaticMeshEntity);
