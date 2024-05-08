#include "ES_ShaderDefine.fxh"

Texture2D           InputTexture        : register(t0);
Texture2D           InputTexture2       : register(t1); // WORLD
RWTexture2D<float>  OutputTexture       : register(u1);

static const uint   TotalNumThreads = NUM_THREAD * NUM_THREAD;
groupshared float4  SharedMemory[TotalNumThreads];

[numthreads(NUM_THREAD, NUM_THREAD, 1)]
void LUM_MAPPING(uint3 vDispatchThreadID : SV_DispatchThreadID)
{
    float  fSampleXRate = g_fWINCX / 1024.f;
    float  fSampleYRate = g_fWINCY / 1024.f;
    
    float4  vSample = 0.f;
    float3  vColor = InputTexture[uint2(vDispatchThreadID.x * fSampleXRate, vDispatchThreadID.y * fSampleYRate)].rgb;
    float   fWorld = InputTexture2[uint2(vDispatchThreadID.x * fSampleXRate, vDispatchThreadID.y * fSampleYRate)].a;
    
    float fLuminance = max(dot(vColor, LUM_FACTOR), 0.0001f);
    
    OutputTexture[vDispatchThreadID.xy] = fLuminance * saturate(fWorld + 0.2f);
}

[numthreads(NUM_THREAD, NUM_THREAD, 1)]
void LUM_CALCAVG(uint3 vGroupID : SV_GroupID, uint3 vGroupThreadID : SV_GroupThreadID)
{
    const uint iThreadIndex = vGroupThreadID.y * NUM_THREAD + vGroupThreadID.x;
    
    const uint2 vSampleIdx = (vGroupID.xy * NUM_THREAD + vGroupThreadID.xy) * 2;
    float4 vSample = 0.f;
    
    vSample.x = InputTexture[vSampleIdx + uint2(0, 0)].r;
    vSample.y = InputTexture[vSampleIdx + uint2(1, 0)].r;
    vSample.z = InputTexture[vSampleIdx + uint2(0, 1)].r;
    vSample.w = InputTexture[vSampleIdx + uint2(1, 1)].r;
    
    SharedMemory[iThreadIndex] = vSample;
    GroupMemoryBarrierWithGroupSync();
    
	[unroll(TotalNumThreads)]
    for (uint s = TotalNumThreads / 2; s > 0; s >>= 1)
    {
        if (iThreadIndex < s)
            SharedMemory[iThreadIndex] += SharedMemory[iThreadIndex + s];
    
        GroupMemoryBarrierWithGroupSync();
    }
    
    if (iThreadIndex == 0)
        OutputTexture[vGroupID.xy] = dot(SharedMemory[0], 0.25f) / TotalNumThreads;
}