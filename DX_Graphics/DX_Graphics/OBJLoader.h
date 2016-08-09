#pragma once
#include <vector>
#include <DirectXMath.h>
#include "Content\ShaderStructures.h"
using namespace DX_Graphics;

using namespace std;
using namespace DirectX;
class ObjLoader
{
public:
	ObjLoader();
	bool loadOBJ(
		const char * path,
		vector<VertexPositionColor> & out_vertices,
		//vector<XMFLOAT2> & out_uv,
		vector<unsigned int> & out_Indices
		//vector<unsigned short> & _vertexIndices,
		//vector<unsigned int> & _normalIndices
	);
	~ObjLoader();

private:

};
