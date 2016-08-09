
texture2D asteroid : register(t0);
SamplerState filter : register (s0);
cbuffer LightData : register(b1)
{
	float4 light_pos;
	float4 light_dir;
	float4 light_ambient;
}

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
	float4 t = asteroid.Sample(filter, input.uv);
	//color//
	float4 c = float4(normalize(input.normal.xyz), 1.0f);
	
	//point light//
	float3 Light_Direction = normalize(light_pos.xyz - input.l_pos.xyz);
	float Light_Ratio = clamp(dot(Light_Direction, normalize(input.normal.xyz)), 0.0f, 1.0f);

	//direction lighting//
	float LightRatio = clamp(dot(-light_dir, input.normal), 0.0f, 1.0f);
	
	c.xyz = Light_Ratio * float3(0.5f, 0.0f, 0.5f);
	c.xyz += LightRatio * float3(0.5f, 0.5f, 0.5f);

	//float r = c.x * 1.0f * Light_Ratio;/** light_ambient.x * LightRatio;*/
	//float g = c.y * 1.0f * Light_Ratio;/** light_ambient.y * LightRatio;*/
	//float b = c.z * 1.0f * Light_Ratio;/** light_ambient.z * LightRatio;*/
	//c.x = r;
	//c.y = g;
	//c.z = b;
	return t * c;
}
