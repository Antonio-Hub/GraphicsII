#include "pch.h"
#include "ObjLoader.h"
//#define _CRT_SECURE_NO_WARNINGS
#pragma warning( disable : 4996)
ObjLoader::ObjLoader()
{
}

bool ObjLoader::loadOBJ(
	const char * path,
	vector<XMFLOAT3> & out_vertices,
	vector<XMFLOAT2> & out_uv,
	vector<XMFLOAT3> & out_normals
)
{
	vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	vector<XMFLOAT3>temp_vertices;
	vector<XMFLOAT2>temp_uv;
	vector<XMFLOAT3>temp_normals;
	FILE * file;
	fopen_s(&file, "pyramid.obj", "r");
	if (file == NULL)
	{
		printf("Impossible to open file! \n");
		return false;
	}
	while (true)
	{
		char lineHeader[128]{};
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;
		if (strcmp(lineHeader, "v") == 0)
		{
			XMFLOAT3 vertex;
			//ZeroMemory(&vertex, sizeof(XMFLOAT3));
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			XMFLOAT2 uv;
			//ZeroMemory(&uv, sizeof(XMFLOAT2));
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			temp_uv.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			XMFLOAT3 nor;
			ZeroMemory(&nor, sizeof(XMFLOAT3));
			fscanf_s(file, "%f %f %f\n", &nor.x, &nor.y, &nor.z);
			temp_normals.push_back(nor);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9)
			{
				printf("File can't be read by our simple parser : (Try exporting with other options)");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);

		}
	}
	for (size_t i = 0; i < vertexIndices.size(); i++)
	{
		unsigned int vertexIndex = vertexIndices[i];
		XMFLOAT3 vertex = temp_vertices[vertexIndex - 1];
		out_vertices.push_back(vertex);

		unsigned int uvIndex = uvIndices[i];
		XMFLOAT2 uv = temp_uv[uvIndex - 1];
		out_uv.push_back(uv);

		unsigned int normalIndex = normalIndices[i];
		XMFLOAT3 normal = temp_normals[normalIndex - 1];
		out_normals.push_back(normal);
	}
	return true;
}

ObjLoader::~ObjLoader()
{
}

