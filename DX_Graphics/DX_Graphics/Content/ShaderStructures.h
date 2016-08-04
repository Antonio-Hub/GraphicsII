#pragma once

namespace DX_Graphics
{
	// Constant buffer used to send MVP matrices to the vertex shader.
	struct ModelViewProjectionConstantBuffer
	{
		DirectX::XMFLOAT4X4 model;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	};
	struct LightData
	{
		DirectX::XMFLOAT4 light_pos;
		DirectX::XMFLOAT4 light_dir;
		DirectX::XMFLOAT4 light_ambient;
	};
	// Used to send per-vertex data to the vertex shader.
	struct VertexPositionColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 color;
		DirectX::XMFLOAT3 normal;
	};
}