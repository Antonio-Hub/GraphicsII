struct VertexShaderInput
{
	float3 pos : POSITION;
	float2 uv : UV0;
	float3 normal : NORMAL;
};
struct GeometryShaderInput
{
	float4 pos : SV_POSITION;
};
GeometryShaderInput main(VertexShaderInput input)
{
	GeometryShaderInput pos;
	pos.pos = float4(input.pos, 1.0f);
	return pos;
}