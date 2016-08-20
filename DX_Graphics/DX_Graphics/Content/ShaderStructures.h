#pragma once

namespace DX_Graphics
{
	// Constant buffer used to send MVP matrices to the vertex shader.
	struct ModelViewProjectionConstantBuffer
	{
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	};
	struct LightData
	{
		DirectX::XMFLOAT4 light_pos;
		DirectX::XMFLOAT4 light_dir;
		DirectX::XMFLOAT4 light_ambient;
		DirectX::XMFLOAT4 spot_light_pos;
		DirectX::XMFLOAT4 spot_light_dir;
	};
	// Used to send per-vertex data to the vertex shader.
	struct VertexPositionColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 uv;
		DirectX::XMFLOAT3 normal;
	};
	struct Models
	{
		DirectX::XMFLOAT4X4 position;
	};
	struct InstanceModels
	{
		DirectX::XMFLOAT4X4 position[5];
	};
}