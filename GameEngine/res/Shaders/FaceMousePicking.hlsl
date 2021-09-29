cbuffer ConstantBuffer: register(b0)
{
	matrix model;
	matrix viewprojection;
};

struct VS_INPUT
{
	float4 position: POSITION;
	unsigned int faceindex: FACEINDEX;
};

struct PS_INPUT
{
	float4 position: SV_POSITION;
	unsigned int faceindex : FACEINDEX;
};

struct PS_OUTPUT
{
	unsigned int faceindex : SV_TARGET0;
};

PS_INPUT vsmain(VS_INPUT input)
{
	PS_INPUT output;

	matrix mvp = mul(viewprojection, model);
	float4 position = mul(mvp, input.position);

	output.position = position;
	output.faceindex = input.faceindex;

	return output;
}

PS_OUTPUT psmain(PS_INPUT input) : SV_TARGET
{
	PS_OUTPUT output;

	output.faceindex = input.faceindex;

	return output;
}