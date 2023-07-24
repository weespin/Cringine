#include "ModelPredCompiler.h"

#include <fstream>
#include <sstream>

const std::string ModelDir = "Models\\";

bool ModelPredCompiler::Compile(std::string pszPath, std::vector<Vertex>& VertexesOut, std::vector<DWORD>& IndexesOut)
{
	HANDLE hFile = CreateFileA((ModelDir + pszPath).c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	FILETIME OriginalFileTime;
	GetFileTime(hFile, &OriginalFileTime, nullptr, nullptr);
	DWORD fileSize = GetFileSize(hFile, nullptr);
	std::vector<char> buffer(fileSize);
	DWORD bytesRead = 0;
	if (!ReadFile(hFile, buffer.data(), fileSize, &bytesRead, nullptr))
	{
		CloseHandle(hFile);
		return false;
	}

	CloseHandle(hFile);

	std::istringstream iss(std::string(buffer.begin(), buffer.end()));
	std::string line;
	//positions.reserve(buffer.size());
	//VertexesOut.reserve(buffer.size() / 32);
	std::vector<XMFLOAT3> positions; // A separate list to store the positions
	std::vector<XMFLOAT2> uvs; // A separate list to store the UV coordinates
	std::vector<XMFLOAT3> normals;   // A separate list to store the normals
	logger.Log(INFO, "Trying to compile %s.\n WARNING: COMPILATION TAKES ALOT OF TIME!", pszPath.c_str());

	while (std::getline(iss, line))
	{
		std::istringstream lineStream(line);
		std::string token;
		lineStream >> token;

		if (token == "v")
		{
			XMFLOAT3 pos;
			lineStream >> pos.x >> pos.y >> pos.z;
			positions.emplace_back(pos);
		}
		else if (token == "vt") // Process texture coordinates
		{
			XMFLOAT2 uv = {};
			lineStream >> uv.x >> uv.y;
			uv.y = 1.0f - uv.y; // Flip UV coordinate by Y-axis.
			uvs.push_back(uv);
		}
		else if (token == "vn") // Process normals
		{
			XMFLOAT3 normal;
			lineStream >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal); // Use push_back to add the normal to the vector
		}
		else if (token == "f")
		{
			int Quad[4] = { -1 };
			int numIndexes = 0;
			int uvIndex[4] = { -1 }; // Store UV indices for each vertex

			while (lineStream >> token)
			{
				size_t pos = token.find('/');
				if (pos != std::string::npos)
				{
					uvIndex[numIndexes] = std::stoi(token.substr(pos + 1)) - 1;
					token = token.substr(0, pos);
				}
				Quad[numIndexes++] = std::stoi(token) - 1;
			}

			static const int triangleIndices[] = { 0, 1, 2 };
			static const int quadIndices[] = { 3, 0, 1, 1, 2, 3 };

			const int* indices = (numIndexes == 3) ? triangleIndices : quadIndices;
			const int numTriangles = (numIndexes == 3) ? 1 : 2;

			for (int t = 0; t < numTriangles * 3; ++t)
			{
				int i = indices[t];
				auto normal = (i < normals.size()) ? normals[i] : XMFLOAT3(0,0,0);
				VertexesOut.push_back(Vertex(positions[Quad[i]].x, positions[Quad[i]].y, positions[Quad[i]].z,
					1.f, 1.f,1.f,1.f,
					uvs[uvIndex[i]].x, uvs[uvIndex[i]].y, normal.x, normal.y, normal.z));
				IndexesOut.push_back(static_cast<DWORD>(VertexesOut.size() - 1));
			}
		}
	}

	//Save it
	Serialize(OriginalFileTime, VertexesOut, IndexesOut, ModelDir + "\\Compiled\\" + pszPath);

	return true;
}

bool ModelPredCompiler::Load(std::string pszPath, std::vector<Vertex>& VertexesOut, std::vector<DWORD>& IndexesOut)
{
	logger.Log(INFO, "Trying to load %s from compiled obj", pszPath.c_str());
	bool bDirty = false;
	;
	CreateDirectoryA(std::string(ModelDir + "\\Compiled").c_str(),nullptr);
	HANDLE hFile = CreateFileA((ModelDir + "//Compiled//" + pszPath).c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	std::vector<char> buffer;
	if (hFile == INVALID_HANDLE_VALUE)
	{
		bDirty = true;
	}
	else
	{
		DWORD fileSize = GetFileSize(hFile, nullptr);
		
		buffer.resize(fileSize);
		DWORD bytesRead = 0;
		if (!ReadFile(hFile, buffer.data(), fileSize, &bytesRead, nullptr))
		{
			CloseHandle(hFile);
			bDirty = true;
		}
	}
	const char* FilePointer = buffer.data();

	if (bDirty == false)
	{

		FILETIME CompiledFileTime = *(FILETIME*)FilePointer;
		FilePointer += sizeof(FILETIME);
		//Check Original obj filetime
		HANDLE hOriginalFile = CreateFileA((ModelDir + pszPath).c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hOriginalFile == INVALID_HANDLE_VALUE)
		{
			logger.Log(ERR, "Failed to load compiled file %s. Original file is missing!", pszPath.c_str());
			return false;
		}
		FILETIME OriginalFileTime = {};
		GetFileTime(hOriginalFile, &OriginalFileTime, nullptr, nullptr);
		CloseHandle(hOriginalFile);

		if (CompiledFileTime.dwHighDateTime != OriginalFileTime.dwHighDateTime && CompiledFileTime.dwLowDateTime != OriginalFileTime.dwLowDateTime)
		{
			bDirty = true;
		}
	}

	if (bDirty == false)
	{
		// Read vertex count
		unsigned long nVertexCount = *reinterpret_cast<const unsigned long*>(FilePointer);
		FilePointer += sizeof(unsigned long);
		VertexesOut.reserve(nVertexCount);

		// Read vertex data
		VertexesOut.insert(VertexesOut.end(), reinterpret_cast<const Vertex*>(FilePointer), reinterpret_cast<const Vertex*>(FilePointer + nVertexCount * sizeof(Vertex)));
		FilePointer += nVertexCount * sizeof(Vertex);

		// Read index count
		unsigned long nIndexCount = *reinterpret_cast<const unsigned long*>(FilePointer);
		FilePointer += sizeof(unsigned long);
		IndexesOut.reserve(nIndexCount);

		// Read index data
		IndexesOut.insert(IndexesOut.end(), reinterpret_cast<const DWORD*>(FilePointer), reinterpret_cast<const DWORD*>(FilePointer + nIndexCount * sizeof(DWORD)));

		logger.Log(INFO, "Great! Loaded a compiled model!");
		return true;
	}
	return Compile(pszPath, VertexesOut, IndexesOut);
}

bool ModelPredCompiler::Serialize(FILETIME FileTime, const std::vector<Vertex>& VertexesIn,
	const std::vector<DWORD>& IndexesIn, const std::string& filename)
{
	HANDLE hFile = CreateFileA(filename.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		// Error opening the file
		return false;
	}

	DWORD bytesWritten;
	WriteFile(hFile, reinterpret_cast<const char*>(&FileTime), sizeof(FILETIME), &bytesWritten, nullptr);

	// Write vertex count
	unsigned long nVertexCount = static_cast<unsigned long>(VertexesIn.size());

	WriteFile(hFile, reinterpret_cast<const char*>(&nVertexCount), sizeof(unsigned long), &bytesWritten, nullptr);

	// Write vertex data
	WriteFile(hFile, reinterpret_cast<const char*>(VertexesIn.data()), nVertexCount * sizeof(Vertex), &bytesWritten, nullptr);

	// Write index count
	unsigned long nIndexCount = static_cast<unsigned long>(IndexesIn.size());
	WriteFile(hFile, reinterpret_cast<const char*>(&nIndexCount), sizeof(unsigned long), &bytesWritten, nullptr);

	// Write index data
	WriteFile(hFile, reinterpret_cast<const char*>(IndexesIn.data()), nIndexCount * sizeof(DWORD), &bytesWritten, nullptr);

	// Close the file
	CloseHandle(hFile);

	return true;
}
