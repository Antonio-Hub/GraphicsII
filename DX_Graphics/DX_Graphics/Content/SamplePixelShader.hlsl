

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
};

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 c = float4(input.color, 1.0f);
	float r = c.x * light_ambient.x;
	float g = c.y * light_ambient.y;
	float b = c.z * light_ambient.z;
	c.x = r;
	c.y = g;
	c.z = b;
	return c;
}
