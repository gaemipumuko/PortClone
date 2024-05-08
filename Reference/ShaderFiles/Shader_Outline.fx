#include "ES_ShaderDefine.fxh"

Texture2D           InputTexture1       : register(t0);
Texture2D           InputTexture2       : register(t1);
Texture2D           InputTexture3       : register(t2);
RWTexture2D<float4> OutputTexture       : register(u1);

#define WINCX 1600
#define WINCY 900

VS_OUT_POSTEX VS_OUTLINE(VS_IN_POSITION In)
{
    VS_OUT_POSTEX Out = (VS_OUT_POSTEX) 0;
    
    float4x4 OrthoWVPMatrix;
    
    OrthoWVPMatrix[0] = g_vOrthoWVPRight;
    OrthoWVPMatrix[1] = g_vOrthoWVPUp;
    OrthoWVPMatrix[2] = g_vOrthoWVPLook;
    OrthoWVPMatrix[3] = g_vOrthoWVPPosition;
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), OrthoWVPMatrix);
    Out.vTexcoord.x = (Out.vPosition.x + 1.f) / 2.f;
    Out.vTexcoord.y = abs(Out.vPosition.y - 1.f) / 2.f;
   
    return Out;
}

float4 PS_OUTLINE(PS_IN_POSTEX In) : SV_Target
{
    static float fDepthThreshold01 = 1.5f;
    static float fDepthNormalThreshold = 0.5f;
    static float fDepthNormalThresholdScale = 14.f; // 7.f
    
    static float fOutScale   = 0.5f;    /* 외곽선 검출 범위 */
    static float fOutStrong  = 0.5f;    /* 외곽선 세기 */
    
    vector  vDiffuse    = InputTexture1.Sample(CSAnisotropicSampler, In.vTexcoord);
    if (vDiffuse.w == 0.f)
        discard;
    
    /* Depth Factor */
    float   fDepthTL    = InputTexture2.Sample(CSPointSampler, saturate(In.vTexcoord + float2(-fOutScale / WINCX,   -fOutScale / WINCY))).r;
    float   fDepthTR    = InputTexture2.Sample(CSPointSampler, saturate(In.vTexcoord + float2(fOutScale / WINCX,    -fOutScale / WINCY))).r;
    float   fDepthBL    = InputTexture2.Sample(CSPointSampler, saturate(In.vTexcoord + float2(-fOutScale / WINCX,   fOutScale / WINCY))).r;
    float   fDepthBR    = InputTexture2.Sample(CSPointSampler, saturate(In.vTexcoord + float2(fOutScale / WINCX,    fOutScale / WINCY))).r;
  
    /* Normal Factor */
    float3  vNormalTL   = InputTexture3.Sample(CSPointSampler, saturate(In.vTexcoord + float2(-fOutScale / WINCX,   -fOutScale / WINCY))).rgb;
    float3  vNormalTR   = InputTexture3.Sample(CSPointSampler, saturate(In.vTexcoord + float2(fOutScale / WINCX,    -fOutScale / WINCY))).rgb;
    float3  vNormalBL   = InputTexture3.Sample(CSPointSampler, saturate(In.vTexcoord + float2(-fOutScale / WINCX,   fOutScale / WINCY))).rgb;
    float3  vNormalBR   = InputTexture3.Sample(CSPointSampler, saturate(In.vTexcoord + float2(fOutScale / WINCX,    fOutScale / WINCY))).rgb;

    /* Threshold */
    float3  vViewNormal = float3(vNormalBL * 2.f - 1.f);
    float   fNdotV      = 1.f - dot(vViewNormal, -SceneViewMatrix[2].rgb);

    /* Normal Outline */
    float3  vNormalDiffTRBL = vNormalTR - vNormalBL;
    float3  vNormalDiffTLBR = vNormalTL - vNormalBR;
    
    float   fNormalOutline        = sqrt(dot(vNormalDiffTRBL, vNormalDiffTRBL) + dot(vNormalDiffTLBR, vNormalDiffTLBR));
    float   fNormalThreshold01    = saturate((fNdotV - fDepthThreshold01) / (1.f - fDepthThreshold01));
    float   fNormalThreshold      = fNormalThreshold01 * fDepthNormalThresholdScale + 1.f;
    fNormalOutline = fNormalOutline > fNormalThreshold ? 1.f : 0.f;
    
    /* Depth Outline */ 
    float fDepthOutline = 0.f;
    
    float fDepthDiffTRBL = fDepthTR - fDepthBL;
    float fDepthDiffTLBR = fDepthTL - fDepthBR;
    
    fDepthOutline = sqrt(pow(fDepthDiffTRBL, 2.f) + pow(fDepthDiffTLBR, 2.f)) * 100.f;
    float fDepthThreshold = fDepthThreshold01 * fDepthBL * fNormalThreshold;
    fDepthOutline = fDepthOutline > fDepthThreshold ? 1.f : 0.f;
    
    // return max(fNormalOutline, fDepthOutline);
    return vDiffuse * (1.f - fOutStrong * max(fNormalOutline, fDepthOutline));
}