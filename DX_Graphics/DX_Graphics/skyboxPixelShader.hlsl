textureCUBE asteroid : register(t0);
SamplerState filter : register (s0);

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
	float4 c = asteroid.Sample(filter, input.l_pos);
	//float d;
	//d = c.x + c.y + c.z;
	//d /= 3;
	//float4 e = float4(d, d, d, 1.0f);
	return c;
}
