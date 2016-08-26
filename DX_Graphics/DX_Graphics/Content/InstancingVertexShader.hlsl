
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix view;
	matrix projection;
};
cbuffer InstanceModels : register(b1)
{
	matrix position[5];
};

struct VertexShaderInput
{
	float3 pos : POSITION;
	float2 uv : UV0;
	float3 normal : NORMAL;
	uint instanceID : SV_InstanceID;
};
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 uv : UV0;
	float4 normal : NORMAL;
	float3 l_pos : COLOR1;

};

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	//add w to position and normal
	float4 pos = float4(input.pos, 1.0f);
	float4 nor = float4(normalize(input.normal), 0.0f);

	// Transform the vertex position into projected space.
	pos = mul(pos, position[input.instanceID]);
	output.l_pos = pos.xyz;
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	//vertex normal
	nor = mul(nor, position[input.instanceID]);
	output.normal = nor;

	// Pass the color through without modification.
	output.uv = input.uv;


	return output;
}