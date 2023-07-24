struct VS_OUTPUT
{
	float4 pos: SV_POSITION;
	nointerpolation float4 color: COLOR;
	float2 tex : TEXCOORD; // texture coordinates
    float4 normal : NORMAL;
};

Texture2D objTexture : register(t0); // texture resource
SamplerState objSampler : register(s0); // sampler state

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

float4 main(VS_OUTPUT input) : SV_TARGET
{
    // Sample the texture using the input texture coordinates
    float4 texColor = objTexture.Sample(objSampler, input.tex);

    float4 color = input.color;
    if (UseTexture == 1.f)
    {
        // Combine the vertex color with the texture color
        color *= texColor;
    }

    //// Calculate the diffuse lighting contribution //TODO: Add Intesify!
    //float3 lightDir = normalize(dir);
    //float3 normal = normalize(input.normal);
    //float diffuseIntensity = saturate(dot(lightDir, normal));
    //finalColor *= (ambient + diffuseIntensity * diffuse);
    //Uhh i'll be better create a deffered shading
    float3 finalColor;

    finalColor = diffuse * ambient;
    finalColor += saturate(dot(dir, input.normal) * diffuse * color);

    return float4(finalColor,1.f);
}