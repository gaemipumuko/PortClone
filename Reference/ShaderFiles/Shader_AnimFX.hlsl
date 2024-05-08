#include    "Engine_Shader_Defines.hlsli"

matrix		g_BoneMatrices[256];
Texture2D	g_EffectTextures[10];

VS_OUT_POSNORTEX VS_MAIN(VS_IN_ANIMMESH In)
{
    VS_OUT_POSNORTEX Out = (VS_OUT_POSNORTEX) 0;

    matrix matWV, matWVP, matOldWV, matOldWVP;

	float		fW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

	/* 현재 정점에게 적용되어야할 뼈 네개의 상태행렬 합쳐서 하나의 행렬로 만든다. */
	matrix		BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fW;

	/* 뼈의 행렬을 정점의 로컬위치에 곱해서 정점의 로컬위치가 뼈의 위치로 바뀌게끔 처리한다. */
	vector		vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
	vector		vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vPosition, matWVP);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
	Out.vTexcoord = In.vTexcoord; 
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

    return Out;
}

PS_OUT_DIFFUSE PS_BARBARA_ES1(PS_IN_POSNORTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    vector vDiffuseTexture = g_EffectTextures[1].Sample(AnisotropicSampler, float2((In.vTexcoord.x), (In.vTexcoord.y)));
    if (vDiffuseTexture.a < 0.1f)
        discard;
    
    Out.vDiffuse        = vDiffuseTexture;
    Out.vDiffuse.rgb    = vDiffuseTexture.rgb * float3(1.1255f, 1.3569f, 1.5686f);
    Out.vDiffuse.rgb    = saturate(pow(Out.vDiffuse.rgb, 2.f));

	return Out;
}

PS_OUT_SCENE_DNWF PS_REACTOR(PS_IN_POSNORTEX In)
{
    PS_OUT_SCENE_DNWF Out = (PS_OUT_SCENE_DNWF) 0;
    
    vector vMtrlDiffuse = g_EffectTextures[0].Sample(LinearSampler, In.vTexcoord);
    
    Out.vDiffuse        = vMtrlDiffuse;
    Out.vNormal         = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vWorld          = In.vWorldPos;
    Out.vFactor         = float4(1.f, 0.f, 0.f, 0.f);

    return Out;
}

PS_OUT_DIFFUSE PS_REACTOR_SYMBOL(PS_IN_POSNORTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;

    float2 vUV  = float2(In.vTexcoord);
    vUV.x       = In.vTexcoord.x + g_vUVRate.x * (1.f / g_vUVRate.z);
    vUV.y       = In.vTexcoord.y + g_vUVRate.y * (1.f / g_vUVRate.w);
    
    vector vSymbolDesc  = g_EffectTextures[3].Sample(LinearSampler, vUV);
    if (vSymbolDesc.r == 0.f)
        discard;
    
    vector vColorDesc   = g_EffectTextures[4].Sample(LinearSampler, vUV);

    Out.vDiffuse        = vColorDesc;
    Out.vDiffuse.rgb    = vColorDesc.rgb * 0.75f;
    
    return Out;
}

PS_OUT_DIFFUSE PS_REACTED_SYMBOL(PS_IN_POSNORTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;

    float2 vUV = float2(In.vTexcoord);
    vUV.x = In.vTexcoord.x + g_vUVRate.x * (1.f / g_vUVRate.z);
    vUV.y = In.vTexcoord.y + g_vUVRate.y * (1.f / g_vUVRate.w);
    
    vector vSymbolDesc = g_EffectTextures[3].Sample(LinearSampler, vUV);
    if (vSymbolDesc.r == 0.f)
        discard;
    
    vector vColorDesc = g_EffectTextures[4].Sample(LinearSampler, vUV);

    Out.vDiffuse = vColorDesc;
    Out.vDiffuse.rgb = vColorDesc.rgb;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass BarbaraES1Pass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader	= NULL;
        PixelShader		= compile ps_5_0 PS_BARBARA_ES1();
    }

    pass ReactorPass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_REACTOR();
    }

    pass ReactorSymbolPass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_REACTOR_SYMBOL();
    }
}