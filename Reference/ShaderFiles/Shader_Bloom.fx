#include "ES_ShaderDefine.fxh"

Texture2D           InputTexture1       : register(t0); // Scene
Texture2D           InputTexture2       : register(t1); // AVG LUM
Texture2D           InputTexture3       : register(t2); // WORLD
Texture2D           InputTexture4       : register(t3); // CSHADER
RWTexture2D<float4> OutputTexture       : register(u1);

#define NUM_WEIGHT      5
#define GROUP_THREADS   128

static const float  fWeights[NUM_WEIGHT] = { 0.0545f, 0.2442f, 0.4026f, 0.2442f, 0.0545f };

groupshared float4  SharedMemory[GROUP_THREADS];

[numthreads(NUM_THREAD, NUM_THREAD, 1)]
void BLM_THRESHOLD(uint3 vDispatchThreadID : SV_DispatchThreadID)
{
    if (float(vDispatchThreadID.x) > g_fWINCX || float(vDispatchThreadID.y) > g_fWINCY)
        return;
    
    float2  vSampleIdx = float2(vDispatchThreadID.xy) / float2(g_fWINCX, g_fWINCY);

    float3  vColor      = InputTexture1.SampleLevel(CSLinearSampler, vSampleIdx, 0).rgb;
    float   fAvgLum     = InputTexture2[uint2(0, 0)].r;
    float   fWorld      = InputTexture3.SampleLevel(CSLinearSampler, vSampleIdx, 0).a;
    float   fSFactor    = InputTexture4.SampleLevel(CSLinearSampler, vSampleIdx, 0).r;
    
    float   fColorLum   = dot(vColor * fSFactor, LUM_FACTOR);
    
    float   fColorScale = saturate(fColorLum - fAvgLum * g_fBloomThreshold);
    
    OutputTexture[vDispatchThreadID.xy] = float4(vColor * fColorScale * saturate(fWorld + 0.2f), 1.f);
}

[numthreads(NUM_THREAD, NUM_THREAD, 1)]
void BLM_DOWNSCALE(uint3 vDispatchThreadID : SV_DispatchThreadID)
{
    float2  vSampleIdx  = float2(vDispatchThreadID.xy) / float2(g_fWINCX, g_fWINCY);
    float3  vColor      = InputTexture1.SampleLevel(CSLinearSampler, vSampleIdx, 0).rgb;

    OutputTexture[vDispatchThreadID.xy] = float4(vColor, 1.f);
}

[numthreads(NUM_THREAD, NUM_THREAD, 1)]
void BLM_UPSCALE(uint3 vDispatchThreadID : SV_DispatchThreadID)
{
    float2  vSampleIdx  = float2(vDispatchThreadID.xy) / float2(g_fWINCX, g_fWINCY);
    float3  vColor      = InputTexture1.SampleLevel(CSLinearSampler, vSampleIdx, 0).rgb;

    OutputTexture[vDispatchThreadID.xy] = float4(vColor, 1.f);
}

[numthreads(GROUP_THREADS, 1, 1)]
void BLM_HBLUR(uint3 vGroupID : SV_GroupID, uint iGroupIndex : SV_GroupIndex)   // XBlur
{
    int2 vCoord = int2(iGroupIndex - (NUM_WEIGHT / 2) + (GROUP_THREADS - (NUM_WEIGHT / 2) * 2) * vGroupID.x, vGroupID.y);
    vCoord      = clamp(vCoord, int2(0, 0), int2(g_fWINCX - 1, g_fWINCX - 1));
    
    SharedMemory[iGroupIndex] = InputTexture1.Load(int3(vCoord.xy, 0)); // Sampling
    
    GroupMemoryBarrierWithGroupSync();
 
    if (iGroupIndex >= (NUM_WEIGHT / 2) && iGroupIndex < (GROUP_THREADS - (NUM_WEIGHT / 2)) &&
         ((vGroupID.x * (GROUP_THREADS - 2 * (NUM_WEIGHT / 2)) + iGroupIndex - (NUM_WEIGHT / 2)) < (uint)g_fWINCX))
    {
        float4 vOut = 0;
        
        [unroll]
        for (int i = -(NUM_WEIGHT / 2); i <= (NUM_WEIGHT / 2); ++i)
            vOut += SharedMemory[iGroupIndex + i] * fWeights[i + (NUM_WEIGHT / 2)];

        OutputTexture[vCoord] = float4(vOut.rgb, 1.0f);
    }
}

[numthreads(1, GROUP_THREADS, 1)]
void BLM_VBLUR(uint3 vGroupID : SV_GroupID, uint iGroupIndex : SV_GroupIndex)   // YBlur
{
    int2 vCoord = int2(vGroupID.x, iGroupIndex - (NUM_WEIGHT / 2) + (GROUP_THREADS - (NUM_WEIGHT / 2) * 2) * vGroupID.y);
    vCoord      = clamp(vCoord, int2(0, 0), int2(g_fWINCX - 1, g_fWINCX - 1)); 
    
    SharedMemory[iGroupIndex] = InputTexture1.Load(int3(vCoord.xy, 0)); // Sampling
    
    GroupMemoryBarrierWithGroupSync();
 
    if (iGroupIndex >= (NUM_WEIGHT / 2) && iGroupIndex < (GROUP_THREADS - (NUM_WEIGHT / 2)) &&
         ((iGroupIndex - (NUM_WEIGHT / 2) + (GROUP_THREADS - (NUM_WEIGHT / 2) * 2) * vGroupID.y) < (uint)g_fWINCY))
    {
        float4 vOut = 0;
        
        [unroll]
        for (int i = -(NUM_WEIGHT / 2); i <= (NUM_WEIGHT / 2); ++i)
            vOut += SharedMemory[iGroupIndex + i] * fWeights[i + (NUM_WEIGHT / 2)];

        OutputTexture[vCoord] = float4(vOut.rgb, 1.0f);
    }
}

VS_OUT_POSTEX VS_BLOOM(VS_IN_POSITION In)
{
    VS_OUT_POSTEX Out = (VS_OUT_POSTEX)0;

    float4x4    OrthoWVPMatrix;

    OrthoWVPMatrix[0] = g_vOrthoWVPRight;
    OrthoWVPMatrix[1] = g_vOrthoWVPUp;
    OrthoWVPMatrix[2] = g_vOrthoWVPLook;
    OrthoWVPMatrix[3] = g_vOrthoWVPPosition;

    Out.vPosition = mul(vector(In.vPosition, 1.f), OrthoWVPMatrix);
    Out.vTexcoord.x = (Out.vPosition.x + 1.f) / 2.f;
    Out.vTexcoord.y = abs(Out.vPosition.y - 1.f) / 2.f;

    return Out;
}

float4 PS_THRESHOLD(PS_IN_POSTEX In) : SV_Target
{
    float3  vColor      = InputTexture1.Sample(CSLinearSampler, In.vTexcoord).rgb;
    float   fAvgLum     = InputTexture2.Sample(CSPointSampler, int2(0, 0)).r;
    float   fWorld      = InputTexture3.Sample(CSLinearSampler, In.vTexcoord).a;
    float   fSFactor    = InputTexture4.Sample(CSLinearSampler, In.vTexcoord).r;
    
    float   fColorLum   = dot(vColor, LUM_FACTOR);

    float   fColorScale = saturate(fColorLum - fAvgLum * g_fBloomThreshold);

    return float4(vColor * fColorScale * saturate(fWorld + 0.2f) * fSFactor, 1.f);
}

float4 PS_SCALING(PS_IN_POSTEX In) : SV_Target
{
    return InputTexture1.Sample(CSLinearSampler, In.vTexcoord);
}

float4 PS_BLOOM(PS_IN_POSTEX In) : SV_Target
{
    float4 vColor       = InputTexture1.Sample(CSPointSampler, In.vTexcoord);
    if (vColor.r == 1.f && vColor.g == 0.f && vColor.b == 1.f)
        discard;
    
    float4 vBloomDesc   = InputTexture2.Sample(CSLinearSampler, In.vTexcoord);
    
    /* Bloom */
    vColor.rgb          = saturate(vColor.rgb + g_fBloomScale * vBloomDesc.rgb);
    
    return float4(vColor.rgb, 1.f);
}