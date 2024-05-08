#include    "Engine_Shader_Defines.hlsli"

float       g_fFrameTime;

Texture2D   g_DistortionTexture[4]; // 일반, 노이즈, 알파

struct VS_IN
{
    float3  vPosition   : POSITION;
    float2  vTexcoord   : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition  : SV_POSITION;
    float2 vTex       : TEXCOORD0;
    float2 vTexcoord1 : TEXCOORD1;
    float2 vTexcoord2 : TEXCOORD2;
    float2 vTexcoord3 : TEXCOORD3;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vPosition = mul(Out.vPosition, g_ViewMatrix);
    Out.vPosition = mul(Out.vPosition, g_ProjMatrix);
    
    Out.vTex = In.vTexcoord;
    
    Out.vTexcoord1 = In.vTexcoord * 1.f;
    Out.vTexcoord1.y = Out.vTexcoord1.y + (g_fFrameTime * 1.3f);
   
    Out.vTexcoord2 = In.vTexcoord * 2.f;
    Out.vTexcoord2.y = Out.vTexcoord2.y + (g_fFrameTime * 2.1f);
    
    Out.vTexcoord3 = In.vTexcoord * 3.f;
    Out.vTexcoord3.y = Out.vTexcoord3.y + (g_fFrameTime * 2.3f);

    return Out;
}

struct PS_IN
{
    float4 vPosition  : SV_POSITION;
    float2 vTex       : TEXCOORD0;
    float2 vTexcoord1 : TEXCOORD1;
    float2 vTexcoord2 : TEXCOORD2;
    float2 vTexcoord3 : TEXCOORD3;
};

struct PS_OUT
{
    vector  vColor      : SV_TARGET0;    
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    

    float4 vNoise1;
    float4 vNoise2;
    float4 vNoise3;
    float4 vFinalNoise;
    float fPerturb;
    float2 vNoiseCoords;
    float4 vAlphaColor;
    
    vNoise1 = g_DistortionTexture[1].Sample(LinearSampler, In.vTexcoord1);
    vNoise2 = g_DistortionTexture[1].Sample(LinearSampler, In.vTexcoord2);
    vNoise3 = g_DistortionTexture[1].Sample(LinearSampler, In.vTexcoord3);
    
    vNoise1 = (vNoise1 - 0.5f) * 2.f;
    vNoise2 = (vNoise2 - 0.5f) * 2.f;
    vNoise3 = (vNoise3 - 0.5f) * 2.f;
    
    vNoise1.xy = vNoise1.xy * float2(0.1f, 0.2f);
    vNoise2.xy = vNoise2.xy * float2(0.1f, 0.3f);
    vNoise3.xy = vNoise3.xy * float2(0.1f, 0.1f);
    
    vFinalNoise = vNoise1 + vNoise2 + vNoise3;
    
    fPerturb = ((1.f - In.vTex.y) * 0.8f) + 0.5f;
    
    vNoiseCoords.xy = (vFinalNoise.xy * fPerturb) + In.vTex.xy;
    
    Out.vColor = g_DistortionTexture[0].Sample(LinearClampSampler, vNoiseCoords.xy);
    
    vAlphaColor = g_DistortionTexture[2].Sample(LinearClampSampler, vNoiseCoords.xy);
    
    Out.vColor.a = vAlphaColor;
    
    if (Out.vColor.a < 0.1f)
        discard;
    
    Out.vColor.r = 1.f;
    Out.vColor.g -= 1.f - vAlphaColor;
    
	
    return Out;
}

technique11 DefaultTechnique
{
    pass NormalFlame
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_MAIN();
    }
}