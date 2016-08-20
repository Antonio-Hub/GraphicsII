cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix view;
	matrix projection;
};
cbuffer Models : register(b1)
{
	matrix position;
};

struct GSOutput
{
	float4 pos : SV_POSITION;
	float2 uv : UV0;
	float4 normal : NORMAL;
	float3 l_pos : COLOR1;
};
struct GeometryShaderInput
{
	float4 pos : SV_POSITION;
};
[maxvertexcount(4)]
void main(
	point float4 input[1] : SV_POSITION,
	inout TriangleStream< GSOutput > output
)
{

	GSOutput element0;
	element0.pos = input[0];
	element0.pos.x -= 5;
	element0.pos.z += 5;
	element0.pos = mul(element0.pos, position);
	element0.l_pos = element0.pos.xyz;
	element0.pos = mul(element0.pos, view);
	element0.pos = mul(element0.pos, projection);
	element0.normal = float4(0.0f, 1.0f, 0.0f, 1.0f);
	element0.uv = float2(0.0f, 0.0f);
	output.Append(element0);

	GSOutput element1;
	element1.pos = input[0];
	element1.pos.x += 5;
	element1.pos.z += 5;
	element1.pos = mul(element1.pos, position);
	element1.l_pos = element1.pos.xyz;
	element1.pos = mul(element1.pos, view);
	element1.pos = mul(element1.pos, projection);
	element1.normal = float4(0.0f, 1.0f, 0.0f, 1.0f);
	element1.uv = float2(0.0f, 1.0f);
	output.Append(element1);

	GSOutput element2;
	element2.pos = input[0];
	element2.pos.x -= 5;
	element2.pos.z -= 5;
	element2.pos = mul(element2.pos, position);
	element2.l_pos = element2.pos.xyz;
	element2.pos = mul(element2.pos, view);
	element2.pos = mul(element2.pos, projection);
	element2.normal = float4(0.0f, 1.0f, 0.0f, 1.0f);
	element2.uv = float2(1.0f, 0.0f);
	output.Append(element2);

	GSOutput element3;
	element3.pos = input[0];
	element3.pos.x += 5;
	element3.pos.z -= 5;
	element3.pos = mul(element3.pos, position);
	element3.l_pos = element3.pos.xyz;
	element3.pos = mul(element3.pos, view);
	element3.pos = mul(element3.pos, projection);
	element3.normal = float4(0.0f, 1.0f, 0.0f, 1.0f);
	element3.uv = float2(1.0f, 1.0f);
	output.Append(element3);
}