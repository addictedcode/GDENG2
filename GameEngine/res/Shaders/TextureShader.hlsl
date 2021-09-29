cbuffer SceneConstantBuffer: register(b0)
{
	float m_angle;
	matrix viewprojection;
	float4 m_light_direction;
};

cbuffer EntityConstantBuffer: register(b1)
{
	unsigned int entity_id;
	matrix model;
};


struct VS_INPUT
{
	float4 position: POSITION;
	float3 normal: NORMAL;
	float3 texcoord: TEXCOORD;
	float3 color: COLOR;
};

struct PS_INPUT
{
	float4 position: SV_POSITION;
	float3 normal: NORMAL;
	float2 texcoord: TEXCOORD;
	float3 color: COLOR;
};

struct PS_OUTPUT
{
	float4 color : SV_TARGET0;
	unsigned int entity_id : SV_TARGET1;
};

PS_INPUT vsmain(VS_INPUT input)
{
	PS_INPUT output;

	matrix mvp = mul(viewprojection, model);
	float4 position = mul(mvp, input.position);

	output.position = position;
	output.normal = normalize(mul((float3x3)model, input.normal));
	output.texcoord = input.texcoord;
	output.color = input.color;

	return output;
}

Texture2D texture2d : register(t0);
SamplerState samplerstate : register(s0);

PS_OUTPUT psmain(PS_INPUT input) : SV_TARGET
{
	PS_OUTPUT output;

	output.color = texture2d.Sample(samplerstate, input.texcoord);
	output.entity_id = entity_id;
	return output;
}