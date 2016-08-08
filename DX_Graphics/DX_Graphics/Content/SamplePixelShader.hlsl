

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
	float3 color : COLOR0;
	float3 norm : NORMAL;
};

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 c = float4(input.color, 1.0f);
	float LightRatio = clamp(dot(-light_dir, input.norm), 0.0f, 1.0f);
	float r = c.x /** light_ambient.x*/ * LightRatio;
	float g = c.y  /** light_ambient.y*/ * LightRatio;
	float b = c.z  /** light_ambient.z*/ * LightRatio;
	c.x = r;
	c.y = g;
	c.z = b;
	return c;
}
