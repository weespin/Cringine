#pragma once
#include <map>

#include "Mesh.h"

class StaticMeshManager
{
public:
    static StaticMeshManager& GetInstance() {
        static StaticMeshManager instance;
        return instance;
    }
	Mesh* LoadMesh(const std::string& meshName);
private:
	std::map<std::string, Mesh*> m_CachedMeshes;
};

