#pragma once
#include "Mesh.h"

class ModelPredCompiler
{
public:
	static bool Compile(std::string pszPath, std::vector<Vertex>& VertexesOut, std::vector<DWORD>& IndexesOut);
	static bool Load(std::string pszPath, std::vector<Vertex>& VertexesOut, std::vector<DWORD>& IndexesOut);
	static bool Serialize(FILETIME FileTime,const std::vector<Vertex>& VertexesIn, const std::vector<DWORD>& IndexesIn, const std::string& filename);
};

