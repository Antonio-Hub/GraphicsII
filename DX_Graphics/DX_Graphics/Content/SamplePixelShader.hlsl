texture2D asteroid : register(t0);
texture2D asteroid1 : register(t1);
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
	float4 t = asteroid1.Sample(filter, input.uv);
	//color//
	float4 c = float4(normalize(input.normal.xyz), 1.0f);
	
	//point light//
	float3 Light_Direction = normalize(light_pos.xyz - input.l_pos.xyz);
	float Light_Ratio = clamp(dot(Light_Direction, normalize(input.normal.xyz)), 0.0f, 1.0f);
	float p_lightAttenuation = 1.0f - clamp(length(light_pos.xyz - input.l_pos.xyz) / 20.0f, 0.0f, 1.0f);


	//direction lighting//
	float LightRatio = clamp(dot(-light_dir, input.normal), 0.0f, 1.0f);
	
	//spotlight//
	float3 s_lightDirection = normalize(spot_light_pos.xyz - input.l_pos.xyz);
	float s_surfaceRatio = clamp(dot(-s_lightDirection, spot_light_dir), 0.0f, 1.0f);
	//this line can be replaced with s_lightRadiusAttenuation
	//float s_spotFactor = (s_surfaceRatio > 0.8f) ? 1 : 0;
	float s_lightRatio = clamp(dot(s_lightDirection, input.normal.xyz), 0.0f, 1.0f);
	float s_lightDistanceAttenuation = 1.0f - clamp(length(spot_light_pos.xyz - input.l_pos.xyz) / 20.0f, 0.0f, 1.0f);
	float s_lightRadiusAttenuation = 1.0f - clamp((0.9f - s_surfaceRatio) / (0.9f - 0.8f), 0.0f, 1.0f);

	//point light
	c.xyz = p_lightAttenuation * Light_Ratio * float3(0.7f, 0.7f, 0.2f);
	//direction light
	c.xyz += LightRatio * float3(0.5f, 0.5f, 0.5f);
	//spot light
	c.xyz += s_lightRadiusAttenuation * s_lightDistanceAttenuation * /*s_spotFactor **/ s_lightRatio * float3(0.9f, 0.9f, 0.9f);
	c = saturate(c);
	return t * c;
}
