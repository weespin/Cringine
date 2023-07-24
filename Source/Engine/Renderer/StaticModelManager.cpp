#include "StaticModelManager.h"

Mesh* StaticMeshManager::LoadMesh(const std::string& meshName)
{
	if(m_CachedMeshes.find(meshName) == m_CachedMeshes.end())
	{
		Mesh* pMesh = new Mesh();
		pMesh->LoadFromFile(meshName);
		m_CachedMeshes[meshName] = pMesh;
		logger.Log(INFO, "Using a cached model!");
		return pMesh;
	}
	return m_CachedMeshes[meshName];
}
