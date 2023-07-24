// This is the input struct for the vertex shader, containing the position and color of a vertex
struct VS_INPUT
{
	float4 pos : POSITION; // vertex position in 3D space
	float4 color : COLOR; // vertex color
	float2 uv : TEXCOORD; // texture coordinates
	float4 normal : NORMAL;
};

// This is the output struct for the vertex shader, containing the position and color of a vertex after transformation
struct VS_OUTPUT
{
	float4 pos : SV_POSITION; // vertex position in 2D space after transformation
	float4 color : COLOR; // vertex color
	float2 uv : TEXCOORD; // texture coordinates
	float4 normal : NORMAL;
};

// This is the constant buffer containing the World-View-Projection matrix

cbuffer Cbuffer : register(b0)
{
	float4x4 wvpMat;
	float4 color;
	float UseColorOverride;
	float UseTexture;
	float2 pad;
	float4x4 World;
	float3 dir;
	float pad1;
	float4 ambient;
	float4 diffuse;
};
// This is the main function of the vertex shader
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	// Transform the vertex position using the World-View-Projection matrix
	output.pos = mul(input.pos, wvpMat);
	output.normal = mul(input.normal, World);
	// Pass the vertex color through to the output struct
    if (UseColorOverride)
    {
        output.color = color;
    }
	else
	{
        output.color = input.color;
    }
	output.uv = input.uv;

	return output; // Return the transformed vertex position, color, and texture coordinates
}