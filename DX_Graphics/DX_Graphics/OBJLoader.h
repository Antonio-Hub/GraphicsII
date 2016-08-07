#pragma once
#include <vector>
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;
class ObjLoader
{
public:
	ObjLoader();
	bool loadOBJ(
		const char * path,
		vector<XMFLOAT3> & out_vertices,
		vector<XMFLOAT2> & out_uv,
		vector<XMFLOAT3> & out_normals
	);
	~ObjLoader();

private:

};
