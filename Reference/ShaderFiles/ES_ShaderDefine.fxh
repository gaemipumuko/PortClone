#define NUM_THREAD  32
#define HALF_THREAD 16
#define LUM_FACTOR float3(0.299f, 0.587f, 0.114f)

SamplerState        CSPointSampler          : register(s0);
SamplerState        CSLinearSampler         : register(s1);
SamplerState        CSBlurSampler           : register(s2);
SamplerState        CSAnisotropicSampler    : register(s3);

cbuffer cbWindow     : register(b0)
{
    float   g_fWINCX = 1600.f;
    float   g_fWINCY = 900.f;
};

cbuffer cbBloom      : register(b1)
{
    float   g_fBloomScale = 1.f;
    float   g_fBloomThreshold = 1.f;
};

cbuffer cbColorGrade : register(b2)
{
    float3  g_vColorFilter;
    float   g_fSaturation;
    float   g_fExposure;
    float   g_fContrast;
};

cbuffer cbOrthoWVP    : register(b3)
{
    float4  g_vOrthoWVPRight = float4(1.f, 0.f, 0.f, 0.f);
    float4  g_vOrthoWVPUp = float4(0.f, 1.f, 0.f, 0.f);
    float4  g_vOrthoWVPLook = float4(0.f, 0.f, 1.f, 0.f);
    float4  g_vOrthoWVPPosition = float4(0.f, 0.f, 0.f, 1.f);
};

cbuffer cbOutline : register(b4)
{
    float4x4    SceneViewMatrix;
    float4x4    SceneProjMatrix;
    
    float       fSceneFar = 200.f;
};

#include "Engine_Shader_Struct.hlsli"
#include "Engine_Shader_Function.hlsli"