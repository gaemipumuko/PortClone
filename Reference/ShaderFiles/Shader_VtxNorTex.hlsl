#include    "Engine_Shader_Defines.hlsli"

/* For.Terrain */
Texture2D   g_GrassDiffuse, g_GrassNormal;
Texture2D   g_RoadDiffuse, g_RoadNormal;
Texture2D   g_StoneDiffuse, g_StoneNormal;

/* For.Water */
Texture2D g_NoiseTexture;
float3 g_vScale, g_Noise;
float2 g_vOffset;

VS_OUT_POSNORTEX VS_MAIN(VS_IN_POSNORTEX In)
{
    VS_OUT_POSNORTEX Out = (VS_OUT_POSNORTEX) 0;

    matrix matWV, matWVP;

    matWV   = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP  = mul(matWV, g_ProjMatrix);

    Out.vPosition   = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vNormal     = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexcoord   = In.vTexcoord;
    Out.vWorldPos   = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos    = Out.vPosition;

    return Out;
}

PS_OUT_SCENE_TERRAIN PS_MAIN(PS_IN_POSNORTEX In)
{
    PS_OUT_SCENE_TERRAIN Out = (PS_OUT_SCENE_TERRAIN) 0;

    vector vGrassDiffuse, vRoadDiffuse, vStoneDiffuse;
    vector vGrassNormal, vRoadNormal, vStoneNormal;
    
    vGrassDiffuse = g_GrassDiffuse.Sample(LinearSampler, In.vTexcoord * 1025);
    vGrassNormal = g_GrassNormal.Sample(LinearSampler, In.vTexcoord * 1025);
    
    vRoadDiffuse = g_RoadDiffuse.Sample(LinearSampler, In.vTexcoord * 1025);
    vRoadNormal = g_RoadNormal.Sample(LinearSampler, In.vTexcoord * 1025);
    
    vStoneDiffuse = g_StoneDiffuse.Sample(LinearSampler, In.vTexcoord * 1025);
    vStoneNormal = g_StoneNormal.Sample(LinearSampler, In.vTexcoord * 1025);
    
    vector vMtrlMask = vector(0.f, 0.f, 0.f, 0.f);
    
    vMtrlMask = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);
        
    Out.vDiffuse        = vector((vGrassDiffuse * vMtrlMask.r + vRoadDiffuse * vMtrlMask.g + vStoneDiffuse * vMtrlMask.b).xyz, 1.0f);
    Out.vNormal         = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vWorld          = In.vWorldPos;
    Out.vShadowWorld    = In.vWorldPos;
    
    /* SceneFactor (R:IsAdditionalExposure, G:IsFaceMesh) */
    Out.vFactor = float4(1.f, 0.f, 0.f, 0.f);

    return Out;
}

PS_OUT_SCENE_TERRAIN PS_MAIN_WATER(PS_IN_POSNORTEX In)
{
    PS_OUT_SCENE_TERRAIN Out = (PS_OUT_SCENE_TERRAIN) 0;
    
    vector vMtrlNoiseX = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord * g_vScale.x + g_vOffset * g_Noise.x);
    vector vMtrlNoiseY = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord * g_vScale.y + g_vOffset * g_Noise.y);
    vector vMtrlNoiseZ = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord * g_vScale.z + g_vOffset * g_Noise.z);
    
    vector vColorX = vector(vMtrlNoiseX.xyz * 0.2f, 1.0f);
    vector vColorY = vector(vMtrlNoiseY.xyz * 0.2f, 1.0f);
    vector vColorZ = vector(vMtrlNoiseZ.xyz * 0.2f, 1.0f);
    
    float4 vColor = vector(18.0f / 255.0f, 74.0f / 255.0f, 206.0f / 255.0f, 1.0f) + vColorX + vColorY + vColorZ;
    
    Out.vDiffuse = vColor;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vWorld = In.vWorldPos;
    Out.vShadowWorld = In.vWorldPos;
    Out.vFactor.g = 1.f;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Terrain
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        HullShader      = NULL;
        DomainShader    = NULL;
        PixelShader     = compile ps_5_0 PS_MAIN();
    }

    pass TerrainWireFrame
    {
        SetRasterizerState(RS_WireFrame);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        HullShader      = NULL;
        DomainShader    = NULL;
        PixelShader     = compile ps_5_0 PS_MAIN();
    }

    pass Water
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_WATER();
    }
}