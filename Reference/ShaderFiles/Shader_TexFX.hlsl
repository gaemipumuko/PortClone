#include    "Engine_Shader_Defines.hlsli"

Texture2D   g_EffectTextures[10];

VS_OUT_POSTEX VS_MAIN(VS_IN_POSTEX In)
{
    VS_OUT_POSTEX Out = (VS_OUT_POSTEX) 0;
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

VS_OUT_POSTEX VS_MAGIC_CIRCLE(VS_IN_POSTEX In)
{
    VS_OUT_POSTEX Out = (VS_OUT_POSTEX) 0;
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    
    float2 vFromCenter = In.vTexcoord - float2(0.5f, 0.5f);
    
    float fRotationAngle = g_fTimeAcc;
    if (g_bBlue == true)
        fRotationAngle *= -1.f;
    
    float2 vRotatedTexCoord;
    vRotatedTexCoord.x = vFromCenter.x * cos(fRotationAngle) - vFromCenter.y * sin(fRotationAngle);
    vRotatedTexCoord.y = vFromCenter.x * sin(fRotationAngle) + vFromCenter.y * cos(fRotationAngle);
    
    
    Out.vTexcoord = vRotatedTexCoord + float2(0.5f, 0.5f);
    
    return Out;
}

PS_OUT_DIFFUSE PS_GLOW(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    vector vMtrlDiffuse = g_EffectTextures[2].Sample(LinearSampler, In.vTexcoord);
    
    Out.vDiffuse.rgb    = vMtrlDiffuse.rgb * float3(1.0000f, 0.5917f, 0.0000f) * 2.f;
    Out.vDiffuse.a      = (vMtrlDiffuse.r + vMtrlDiffuse.g + vMtrlDiffuse.b) / 3.f;
    
    Out.vDiffuse        = saturate(Out.vDiffuse);

    if (Out.vDiffuse.a < 0.01f)
        discard;
    
    return Out;
}

PS_OUT_DIFFUSE PS_FLARE(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    Out.vDiffuse = g_EffectTextures[3].Sample(LinearSampler, In.vTexcoord);

    if (Out.vDiffuse.a < 0.01f)
        discard;
    
    return Out;
}

PS_OUT_DIFFUSE PS_SMOKE(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    float2 vUnitUV, vCurrUV;
    
    vUnitUV.x = 1.f / g_vUVRate.x;
    vUnitUV.y = 1.f / g_vUVRate.y;
    
    vCurrUV.x = g_vUVRate.z * vUnitUV.x + In.vTexcoord.x * vUnitUV.x;
    vCurrUV.y = g_vUVRate.w * vUnitUV.y + In.vTexcoord.y * vUnitUV.y;
    
    if (g_iIndex == 0)
        Out.vDiffuse = g_EffectTextures[0].Sample(LinearSampler, vCurrUV);
    else
        Out.vDiffuse = g_EffectTextures[1].Sample(LinearSampler, vCurrUV);

    if (Out.vDiffuse.a < 0.01f)
        discard;
    
    return Out;
}

PS_OUT_DIFFUSE PS_MAGIC_CIRCLE(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    float2 vFromCenter = In.vTexcoord - float2(0.5f, 0.5f);
    
    float fRotationAngle = g_fTimeAcc;
    if (g_bBlue == true)
        fRotationAngle *= -1.f;
    
    float2 vRotatedTexCoord;
    vRotatedTexCoord.x = vFromCenter.x * cos(fRotationAngle) - vFromCenter.y * sin(fRotationAngle);
    vRotatedTexCoord.y = vFromCenter.x * sin(fRotationAngle) + vFromCenter.y * cos(fRotationAngle);
    
    vector vDiffuse = g_Texture.Sample(LinearClampSampler, vRotatedTexCoord + float2(0.5f, 0.5f));
 
    float fAlpha;
    if (g_bBlue == false)
        fAlpha = saturate(vDiffuse.r + vDiffuse.g);
    else
        fAlpha = vDiffuse.b;
    
    Out.vDiffuse = float4(1.f, 0.7f, 0.3f, fAlpha);
    
    if (Out.vDiffuse.a < 0.21f)
        discard;
    
    
    //vector vDiffuse = g_EffectTextures[0].Sample(LinearClampSampler, In.vTexcoord);
 
    //float fAlpha;
    //if (g_bBlue == false)
    //    fAlpha = saturate(vDiffuse.r + vDiffuse.g);
    //else
    //    fAlpha = vDiffuse.b;
    
    //Out.vDiffuse = float4(1.f, 0.7f, 0.3f, fAlpha);
    
    //if (Out.vDiffuse.a < 0.21f)
    //    discard;
 
    
    return Out;
}

technique11 DefaultTechnique
{
    pass GlowPass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_GLOW();
    }

    pass FlarePass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_FLARE();
    }

    pass SmokePass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_SMOKE();
    }

    pass MagicCircle
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader     = compile vs_5_0 VS_MAIN();
        GeometryShader   = NULL;
        PixelShader      = compile ps_5_0 PS_MAGIC_CIRCLE();
    }
}