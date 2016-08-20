textureCUBE asteroid : register(t0);
//texture cube
SamplerState filter : register (s0);
cbuffer LightData : register(b0)
{
	float4 light_pos;
	float4 light_dir;
	float4 light_ambient;
	float4 spot_light_pos;
	float4 spot_light_dir;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 uv : UV0;
	float4 normal : NORMAL;
	float3 l_pos : COLOR1;

};

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	//pass the .xyz to sample for skybox to access the texture cube
	float4 t = asteroid.Sample(filter, input.l_pos);
	return t;
}
