#include "World.h"

#include "Game.h"
#include "PropertyManager.h"
void World::Save(const char* pszName)
{
	//Here we go
	//First of all we need to serialize an Entity then Components then Subcomponents
	
	auto& EntityManager = GetEntityManager();
	auto Entities = EntityManager.GetEntites();
	std::vector<unsigned char> Data;
	unsigned long nEntities = Entities.size();
	Data.insert(Data.end(), INSERT_TO_UC_VECTOR(&nEntities, sizeof(nEntities)));
	for (auto CurrentEntity : Entities )
	{
		CurrentEntity->Serialize(Data);
		//Serialize TypeInfo.
		//std::string TypeName = EntityManager.GetEntityTypeName<CurrentEntity>();
		
	}
	CreateDirectoryA("Worlds", nullptr);
	HANDLE hFile = CreateFileA((std::string("Worlds\\") + std::string(pszName) + ".world").c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		// Error opening the file
		//TODO: Handle it
	}
	WriteFile(hFile, Data.data(), Data.size(), nullptr, nullptr);
	CloseHandle(hFile);

}

//TODO: Implement normal container-walker class :^)
void World::Deserialize(std::vector<unsigned char>& dataIn)
{
	
	//Name
	unsigned char* pData = dataIn.data();
	unsigned long nEntities;
	memcpy(&nEntities, pData, sizeof(unsigned long));
	pData += sizeof(unsigned long);
	for (unsigned long i = 0; i < nEntities; ++i)
	{
		unsigned long nEntityNameSize;
		memcpy(&nEntityNameSize, pData, sizeof(unsigned long));
		pData += sizeof(unsigned long);
		std::string TypeName;
		TypeName.resize(nEntityNameSize + 1, '\0');
		memcpy(TypeName.data(), pData, nEntityNameSize);
		pData += nEntityNameSize;
		//Got the name, its time to create an entity!
		auto pCreatedEntity = EntityManager::CreateEntitiy(TypeName.c_str());
		pCreatedEntity->Deserialize(&pData);
		EntityManager.PushEntity(pCreatedEntity);
	}
}

void World::Load(const char* pszName)
{
	CreateDirectoryA("Worlds", nullptr);
	HANDLE hFile = CreateFileA((std::string("Worlds\\") + std::string(pszName) + ".world").c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	std::vector<unsigned char> buffer;
	if (hFile == INVALID_HANDLE_VALUE)
	{
		// Error opening the file
		//TODO: Handle it
	}
	
	DWORD fileSize = GetFileSize(hFile, nullptr);

	buffer.resize(fileSize);
	DWORD bytesRead = 0;
	ReadFile(hFile, buffer.data(), fileSize, &bytesRead, nullptr);
	
	CloseHandle(hFile);
	EntityManager.ResetEntities();
	Deserialize(buffer);
}

void World::Tick(float nDeltaTime)
{
	if(Game::GetInstance().bPaused)
	{
		return;
	}
	for (auto Ent : EntityManager.GetEntites())
	{
		EntityManager::GetEntityTypeName(typeid(Ent));
		if (Ent->CanUpdate())
		{
			Ent->Update(nDeltaTime);
		}
	}
}

void World::PrepareRender()
{
	for (auto Ent : EntityManager.GetEntites())
	{
		Ent->PostUpdate();
	}
}
