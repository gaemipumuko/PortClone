#include    "Engine_Shader_Defines.hlsli"

float       g_fFrameRate;
vector      g_vOriginalColor;
vector      g_vAttenuationColor;

Texture2D   g_EffectTextures[10];

VS_OUT_POSTEXPRJ VS_MAIN(VS_IN_POSTEX In)
{
    VS_OUT_POSTEXPRJ Out = (VS_OUT_POSTEXPRJ) 0;

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;   

    return Out;
}

PS_OUT_BWOIT PS_MAIN(PS_IN_POSTEXPRJ In)
{
    PS_OUT_BWOIT Out = (PS_OUT_BWOIT) 0;

    if (In.vTexcoord.x < 0.f || In.vTexcoord.y < 0.f)
        discard;
    
    float4 vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    In.vTexcoord.y += 0.35f;
    
    float4 vDiffuseColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    vDiffuseColor.a = vColor.r;
    
    if (vDiffuseColor.r > 0.9f)
    {
        vDiffuseColor.g += 0.9f;
        vDiffuseColor.b += 0.5f;
    }
    
    Out.vDiffuse = vDiffuseColor;

    /* Weight-Blended OIT */
    float fDepth = In.vProjPos.w;
    float fWeight = vDiffuseColor.a *
        max(min(1.0f, max(max(vDiffuseColor.r, vDiffuseColor.g), vDiffuseColor.b) * vDiffuseColor.a), vDiffuseColor.a) *
        clamp(0.03f / (1e-5f + pow(fDepth / 200.f, 4.0f)), 1e-2f, 3e3f);

    Out.vDiffuse = vector(vDiffuseColor.rgb * vDiffuseColor.a, vDiffuseColor.a) * fWeight;
    Out.vFactor = vector(fDepth, 1.f, 0.f, 0.f);
    Out.vWeight = vector(vDiffuseColor.a, vDiffuseColor.a, vDiffuseColor.a, vDiffuseColor.a);
    
    return Out;
}

PS_OUT_DIFFUSE PS_OVERTRAIL(PS_IN_POSTEXPRJ In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;

    if (In.vTexcoord.x < 0.f || In.vTexcoord.y < 0.f)
        discard;

    Out.vDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vDiffuse = vector(Out.vDiffuse.r, Out.vDiffuse.g, 0.f, Out.vDiffuse.b);
    
    return Out;
}

PS_OUT_BWOIT PS_DILUC_TRAIL(PS_IN_POSTEXPRJ In)
{
    PS_OUT_BWOIT Out = (PS_OUT_BWOIT) 0;

    if (In.vTexcoord.x < 0.f || In.vTexcoord.y < 0.f)
        discard;
    
    float2 vScale = { 1.8f, 1.f };
    float2 vScale2 = { 3.f, 1.f };
    
    float4 vColor = g_Texture.Sample(LinearSampler, In.vTexcoord * vScale + g_vUVRate2);
    float4 vDiffuseColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vShapeColor = g_HighDynamicTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vNoiseColor = g_LightMapTexture.Sample(LinearSampler, In.vTexcoord * vScale2);

     //   In.vTexcoord.y += 0.35f;
    
    if (vColor.r <0.1f)
        discard;
        
    //vDiffuseColor.a = vColor.r;
   
    vDiffuseColor.a = vShapeColor.b + 0.3f;
    if (vDiffuseColor.g > 0.5f)
        vDiffuseColor.g -= vNoiseColor.g/3.f;
    //if(vDiffuseColor.r > 0.9f)
    //{
    //    vDiffuseColor.g += 0.5f;
    //    vDiffuseColor.b += 0.5f;
    //}
    
    //vDiffuseColor.a += 0.5f;
     
    //if (vColor.r < 0.3f)
    //    discard;

    //if (vColor.r > 0.3f)
    //{
    //    vColor.g = vColor.r * 0.5f;
    //    vColor.r = vColor.r + 0.8f;
    //}
    
    //vColor.a = vColor.r * 0.3f;
    
    Out.vDiffuse = vDiffuseColor;

    /* Weight-Blended OIT */
    float fDepth = In.vProjPos.w;
    float fWeight = vDiffuseColor.a *
        max(min(1.0f, max(max(vDiffuseColor.r, vDiffuseColor.g), vDiffuseColor.b) * vDiffuseColor.a), vDiffuseColor.a) *
        clamp(0.03f / (1e-5f + pow(fDepth / 200.f, 4.0f)), 1e-2f, 3e3f);

    Out.vDiffuse = vector(vDiffuseColor.rgb * vDiffuseColor.a, vDiffuseColor.a) * fWeight;
    Out.vFactor = vector(fDepth, 1.f, 0.f, 0.f);
    Out.vWeight = vector(vDiffuseColor.a, vDiffuseColor.a, vDiffuseColor.a, vDiffuseColor.a);

    return Out;
}

PS_OUT_BWOIT PS_ANDRIUS_TRAIL(PS_IN_POSTEXPRJ In)
{
    PS_OUT_BWOIT Out = (PS_OUT_BWOIT) 0;

    if (In.vTexcoord.x < 0.f || In.vTexcoord.y < 0.f)
        discard;

    float4 vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    //vColor = vector(vColor.r, vColor.r * 0.5f, 0.f, 1.f - vColor.b);
    //vColor.a = vColor.a * (1.f - 1.f * In.vTexcoord.x);
    
    if (vColor.a < 0.1f)
        discard;

    /* Weight-Blended OIT */
    float fDepth = In.vProjPos.w;
    float fWeight = vColor.a *
        max(min(1.0f, max(max(vColor.r, vColor.g), vColor.b) * vColor.a), vColor.a) *
        clamp(0.03f / (1e-5f + pow(fDepth / 200.f, 4.0f)), 1e-2f, 3e3f);

    Out.vDiffuse = vector(vColor.rgb * vColor.a, vColor.a) * fWeight;
    Out.vFactor = vector(fDepth, 1.f, 0.f, 0.f);
    Out.vWeight = vector(vColor.a, vColor.a, vColor.a, vColor.a);

    return Out;
}

PS_OUT_BWOIT PS_ELEMENT_TRAIL(PS_IN_POSTEXPRJ In)
{
    PS_OUT_BWOIT Out = (PS_OUT_BWOIT) 0;

    if (In.vTexcoord.x < 0.f || In.vTexcoord.y < 0.f)
        discard;

    float4  vDiffuseDesc = g_Texture.Sample(LinearSampler, In.vTexcoord);
    float   fDiffuse = (vDiffuseDesc.r + vDiffuseDesc.g + vDiffuseDesc.b) / 3.f;
 
    if (fDiffuse < 0.1f)
        discard;
    
    float4  vColor  = float4(fDiffuse, fDiffuse, fDiffuse, fDiffuse);
    vColor.rgb      = saturate(vColor.rgb * g_vColor.rgb * float3(2.f, 2.f, 2.f));

    /* Weight-Blended OIT */
    float fDepth = In.vProjPos.w;
    float fWeight = vColor.a *
        max(min(1.0f, max(max(vColor.r, vColor.g), vColor.b) * vColor.a), vColor.a) *
        clamp(0.03f / (1e-5f + pow(fDepth / 200.f, 4.0f)), 1e-2f, 3e3f);

    Out.vDiffuse    = vector(vColor.rgb * vColor.a, vColor.a) * fWeight;
    Out.vFactor     = vector(fDepth, 1.f, 0.f, 0.f);
    Out.vWeight     = vector(vColor.a, vColor.a, vColor.a, vColor.a);

    return Out;
}

PS_OUT_DIFFUSE PS_EFFIGYFIRE_TRAIL(PS_IN_POSTEXPRJ In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;

    if (In.vTexcoord.x < 0.f || In.vTexcoord.y < 0.f)
        discard;
    
    float4 vTrailDesc       = g_Texture.Sample(LinearSampler, float2(In.vTexcoord.x, In.vTexcoord.y));
    float4 vDiffuseDesc     = g_Texture2.Sample(LinearSampler, float2(In.vTexcoord.x * 10.f - g_fFrameRate * 4.f, In.vTexcoord.y * 2.f));
    
    Out.vDiffuse            = vTrailDesc * vDiffuseDesc;
    if (Out.vDiffuse.a < 0.01f)
        discard;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_NoneWrite, 0);
        SetBlendState(BS_WeightedBlended, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_MAIN();
    }

    pass OverTrailPass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_NoneWrite, 0);
        SetBlendState(BS_One, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_OVERTRAIL();
    }

    pass ANDRIUS_TRAIL
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_NoneWrite, 0);
        SetBlendState(BS_One, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader = compile ps_5_0 PS_ANDRIUS_TRAIL();
    }

    pass DilucTrailPass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_NoneWrite, 0);
        SetBlendState(BS_WeightedBlended, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DILUC_TRAIL();
    }

    pass ElementTrailPass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_NoneWrite, 0);
        SetBlendState(BS_WeightedBlended, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_ELEMENT_TRAIL();
    }

    pass EffigyFireTrailPass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_EFFIGYFIRE_TRAIL();
    }
}