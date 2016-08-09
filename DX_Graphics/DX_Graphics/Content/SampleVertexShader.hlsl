// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix l_model;
	matrix w_model;
	matrix view;
	matrix projection;
};
cbuffer LightData : register(b1)
{
	float4 light_pos;
	float4 light_dir;
	float4 light_ambient;
}
cbuffer ModelViewProjectionConstantBuffer : register(b2)
{
	matrix model1;
	matrix view1;
	matrix projection1;
};
// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float3 pos : POSITION;
	float2 uv : UV0;
	float3 normal : NORMAL;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 uv : UV0;
	float4 normal : NORMAL;
	float3 l_pos : COLOR1;

};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	//add w to position and normal
	float4 pos = float4(input.pos, 1.0f);
	float4 nor = float4(normalize(input.normal), 0.0f);

	// Transform the vertex position into projected space.
	pos = mul(pos, w_model);
	output.l_pos = pos.xyz;
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	//vertex normal
	nor = mul(nor, w_model);
	output.normal = nor;

	// Pass the color through without modification.
	output.uv = input.uv;



	return output;
}
