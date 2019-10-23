//
// Tessellation Quad, 1 control point
//
#include "common.fx"


cbuffer TessellationBuffer : register(b6)
{
	float gTessellationAmount;
	float2 gSize;
	float gLevel;
	float4 gEdgeLevel;
};

struct HullInputType
{
	float3 pos : POSITION;
};

struct ConstantOutputType
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;

	float3 pos : POSITION;
};

struct HullOutputType
{
	float3 pos : POSITION;
};

struct PixelInputType
{
	float4 pos : SV_POSITION;
	float4 posW : TEXCOORD0;
};



////////////////////////////////////////////////////////////////////////////////
// Patch Constant Function
////////////////////////////////////////////////////////////////////////////////

HullInputType main(
	float4 Pos : POSITION
)
{
	HullInputType output;
	output.pos = Pos.xyz;
	return output;
}


////////////////////////////////////////////////////////////////////////////////
// Patch Constant Function
////////////////////////////////////////////////////////////////////////////////
ConstantOutputType ColorPatchConstantFunction(InputPatch<HullInputType, 1> inputPatch
	, uint patchId : SV_PrimitiveID)
{
	ConstantOutputType output;

	const float tessFactor = gTessellationAmount;
	output.edges[0] = 1;
	output.edges[1] = 1;
	output.edges[2] = 1;
	output.edges[3] = 1;
	output.inside[0] = 1;
	output.inside[1] = 1;
	output.pos = inputPatch[0].pos;

	return output;
}


////////////////////////////////////////////////////////////////////////////////
// Hull Shader
////////////////////////////////////////////////////////////////////////////////
[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(1)]
[patchconstantfunc("ColorPatchConstantFunction")]

HullOutputType ColorHullShader(InputPatch<HullInputType, 1> patch
	, uint pointId : SV_OutputControlPointID
	, uint patchId : SV_PrimitiveID)
{
	HullOutputType output = (HullOutputType)0;
	output.pos = patch[pointId].pos;
	return output;
}



////////////////////////////////////////////////////////////////////////////////
// Domain Shader
////////////////////////////////////////////////////////////////////////////////
[domain("quad")]

PixelInputType ColorDomainShader(ConstantOutputType input
	, float2 uv : SV_DomainLocation
	, const OutputPatch<HullOutputType, 1> patch)
{
	float3 vertexPosition;
	PixelInputType output = (PixelInputType)0;
	vertexPosition = input.pos;

	// x-z 축으로 quad를 만든다.
	output.pos = mul(float4(vertexPosition, 1.0f), gWorld);
	output.pos.xz = output.pos.xz + uv * gSize;
	output.posW = output.pos; // world coordinate 

	output.pos = mul(output.pos, gView);
	output.pos = mul(output.pos, gProjection);

	// Send the input color into the pixel shader.
	//output.color = float4(0,0,0,1);

	return output;
}


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 ColorPixelShader(PixelInputType input) : SV_TARGET
{
	//float4 texColor = txDiffuse.Sample(samLinear, input.tex);
	//return float4(1, 1, 1, 1); //input.color;
	//return texColor;
	//return input.posW.y / 500.f;

	return float4(1, 1, 1, 0.5);
	return float4(
		float3(gMtrl_Diffuse.xyz)*0.3f * saturate(input.posW.y * 300)
		+ gMtrl_Diffuse.xyz * (input.posW.y/300)
		, gMtrl_Diffuse.w);
}


technique11 Unlit
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, main()));
		SetHullShader(CompileShader(hs_5_0, ColorHullShader()));
		SetDomainShader(CompileShader(ds_5_0, ColorDomainShader()));
		SetPixelShader(CompileShader(ps_5_0, ColorPixelShader()));
	}
}

