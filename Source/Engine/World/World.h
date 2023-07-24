#pragma once
#include "Engine/Entities/Entity.h"
#include "Utils/Logger.h"




class World
{
public:
	void Save(const char* pszName);
	void Deserialize(std::vector<unsigned char>& dataIn);
	void Load(const char* pszName);
	void Tick(float nDeltaTime);
	void PrepareRender();
	EntityManager& GetEntityManager() { return EntityManager; }

private:
	EntityManager EntityManager;
};

