#include    "Engine_Shader_Defines.hlsli"

matrix			g_SceneViewMatrix, g_SceneProjMatrix;
matrix			g_SceneViewMatrixInv, g_SceneProjMatrixInv;

float3          g_vCascadeClip;
float3          g_vCascadeScale;
float3          g_vCascadeOffsetX;
float3          g_vCascadeOffsetY;

float4			g_vLightDir;
float4			g_vLightPos;
float			g_fLightRange;
float4			g_vLightDiffuse;
float4			g_vLightAmbient;

float4			g_vMtrlAmbient = vector(0.2f, 0.2f, 0.2f, 1.f);

Texture2D		g_ShadeTexture;
Texture2D       g_ParticleTexture;
Texture2D		g_RimLightTexture;
Texture2DArray  g_LightDepthTexture;
Texture2D		g_ShadowWorldTexture;
Texture2D		g_SceneFactorTexture;
Texture2D		g_SceneFaceTexture;

Texture2D		g_WeightTexture;
Texture2D		g_FactorTexture;
Texture2D		g_EFFDiffuseTexture;

bool            g_bDebugCascade = false;

/* Shadow */
SamplerComparisonState ShadowSampler
{
    Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
    ComparisonFunc = LESS;
};

float CalcCascadeShadowFactor(int iCascadeIndex, float4 vLightPosition)
{
    // To Proj
    float3 vProjCoords = vLightPosition.xyz / vLightPosition.w;
    vProjCoords.x = vProjCoords.x * 0.5f + 0.5f;
    vProjCoords.y = -vProjCoords.y * 0.5f + 0.5f;

    // 예외 처리
    if (vProjCoords.x < 0 || vProjCoords.x > 1 || vProjCoords.y < 0 || vProjCoords.y > 1)
        return 1.f;
    
    if (vProjCoords.z > 1.f)
        return 0.f;
    
    float fCurrDepth = vProjCoords.z;   // 현재 깊이
    float fBias = 0.0005f;               // 깊이 오차
    float fShadow = 0.f;
    
    float3 vSamplePosition = vProjCoords;
    vSamplePosition.z = iCascadeIndex;
    
	[unroll]
    for (int iShadowX = -1; iShadowX <= 1; ++iShadowX)
    {
        for (int iShadowY = -1; iShadowY <= 1; ++iShadowY)
        {
            fShadow += g_LightDepthTexture.SampleCmpLevelZero(ShadowSampler, vSamplePosition, fCurrDepth - fBias, int2(iShadowX, iShadowY)).r;
        }
    }
    fShadow /= 9.0f;
    
    return fShadow;
}

VS_OUT_POSTEX VS_MAIN(VS_IN_POSTEX In)
{
    VS_OUT_POSTEX Out = (VS_OUT_POSTEX) 0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;

	return Out;
}

struct PS_OUT_LIGHT
{
    vector		vShade      : SV_TARGET0;
    vector		vRim        : SV_TARGET1;
};

/* RenderTarget Pass */
PS_OUT_DIFFUSE PS_SINGLEDEBUG(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;

	Out.vDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);

	return Out;
}

PS_OUT_DIFFUSE PS_MULTIDEBUG(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;

    Out.vDiffuse = g_TextureArray.Sample(LinearSampler, float3(In.vTexcoord, g_fTexArrayIndex));

    return Out;
}

/* Directional Light */
PS_OUT_LIGHT PS_DIRECTIONAL(PS_IN_POSTEX In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	vector vNormalDesc      = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
	vector vWorldDesc       = g_WorldTexture.Sample(PointSampler, In.vTexcoord);        /* World Texture */
    vector vSceneFactorDesc = g_SceneFactorTexture.Sample(PointSampler, In.vTexcoord);  /* SceneFactor Texture */

    float4  vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    float4  vLightDir;
    if (vSceneFactorDesc.b == 0.f && vSceneFactorDesc.a)
    {
        vLightDir           = g_vLightDir * -1.f;
    }
    else /* FaceMesh */
    {        
	    vector vFaceDesc    = g_SceneFaceTexture.Sample(PointSampler, In.vTexcoord);
        
        float fDotL         = dot(normalize(float2(vFaceDesc.xy)), normalize(g_vLightDir.xz));    // FrontL
        float fDotR         = dot(normalize(float2(vFaceDesc.zw)), normalize(g_vLightDir.xz));    // RightL
        
        float fLambertL     = fDotL * 0.5f + 0.5f;
        float fLambertR     = fDotR;
         
        float fFaceShadow   = lerp(vSceneFactorDesc.b, vSceneFactorDesc.a, step(fLambertR, 0.f));
        float3 fFace        = fLambertL > fFaceShadow ? 1.0 : 0.f;
        
        vLightDir           = float4(fFace, 1.f);
    }
    
    float fLightSmooth      = 0.1f;
    float fDot              = dot(vNormal, vLightDir);
    float fSmooth           = smoothstep(0.f, fLightSmooth, fDot);
    
    Out.vShade              = fSmooth * 0.5f;

    return Out;
}

/* Point Light (Not Use) */
PS_OUT_LIGHT PS_POINT(PS_IN_POSTEX In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
	vector	    vWorldDesc = g_WorldTexture.Sample(PointSampler, In.vTexcoord);         /* World Texture */

	float4		vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    
	vector		vLightDir = vWorldDesc - g_vLightPos;
	float		fDistance = length(vLightDir);
    
	float		fAtt = max((g_fLightRange - fDistance) / g_fLightRange, 0.f) * 5.f;
	Out.vShade = fAtt * (g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)));

	return Out;
}

PS_OUT_DEFERRED PS_SCENE(PS_IN_POSTEX In)
{
    PS_OUT_DEFERRED Out = (PS_OUT_DEFERRED) 0;

    vector  vDiffuse = g_DiffuseTexture.Sample(AnisotropicSampler, In.vTexcoord);
    if (vDiffuse.a == 0.f)
        discard;

    vDiffuse.rgb = EOTF_Linearization(vDiffuse.rgb);
    
    vector	vRim = g_RimLightTexture.Sample(PointSampler, In.vTexcoord);                /* RimLight Texture */
	vector  vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);                /* Lambert Texture */
	vector	vWorldDesc = g_WorldTexture.Sample(PointSampler, In.vTexcoord);             /* World Texture */
    vector  vLightMapDesc = g_LightMapTexture.Sample(PointSampler, In.vTexcoord);       /* LightMap Texture */
	vector	vShadowWorldDesc = g_ShadowWorldTexture.Sample(PointSampler, In.vTexcoord); /* ShadowWorld Texture */
    vector  vSceneFactorDesc = g_SceneFactorTexture.Sample(PointSampler, In.vTexcoord); /* SceneFactor Texture */
    
    /* Factor */
    float4  vWorldTranslation = mul(vWorldDesc, g_SceneViewMatrix);
    float4  vShadowTranslation = mul(vShadowWorldDesc, g_SceneViewMatrix);
    
    float   fWorldDepth = vWorldTranslation.z;
    float   fShadowDepth = vShadowTranslation.z;
    
    /* Additional Exposure */
    if (vSceneFactorDesc.r != -1.f)
        vDiffuse.rgb *= pow(2.f, 2.f);

    /* Toon Shader (명암) */
    float3  vBaseColor = float3(1.f, 1.f, 1.f);
    float3  vShadowColor = float3(0.878f, 0.733f, 0.698f) / 3.f;
    
    float3  vLerpColor = lerp(vShadowColor, vBaseColor, vShade.rgb);
    Out.vDiffuse.rgb = vLerpColor * vDiffuse.rgb;
    Out.vDiffuse.a = 1.f;
    
    /* Shadow */
    float   vShadowFactor;
    float4  vCascadeLightPosition[3];

    if (fWorldDepth == fShadowDepth)
    {
        /* Mesh Shadow Shader */
        [unroll]
        for (int iIndex = 0; iIndex < 3; ++iIndex)
        {
            vCascadeLightPosition[iIndex] = mul(vShadowWorldDesc, g_LightViewProj[iIndex]);
        }
	    [unroll]
        for (int iDepthClip = 0; iDepthClip < 3; ++iDepthClip)
        {
            if (fShadowDepth < g_vCascadeClip[iDepthClip])
            {
                vShadowFactor = CalcCascadeShadowFactor(iDepthClip, vCascadeLightPosition[iDepthClip]);
                break;
            }
        }
    
        Out.vDiffuse *= min((vShadowFactor + 0.25f), 1.f);
    }
    
    if (vSceneFactorDesc.g == 0.f)
        Out.vDepth.r = fWorldDepth;

    // Debug Render
    if (g_bDebugCascade == true)
    {
        if (fWorldDepth < g_vCascadeClip[0])
        {
            Out.vDiffuse.rgb *= float3(1.f, 0.25f, 0.25f);
        }
        else if (fWorldDepth < g_vCascadeClip[1])
        {
            Out.vDiffuse.rgb *= float3(0.25f, 1.f, 0.25f);
        }
        else if (fWorldDepth < g_vCascadeClip[2])
        {
            Out.vDiffuse.rgb *= float3(0.25f, 0.25f, 1.f);
        }
    }
    
    return Out;
}

PS_OUT_DIFFUSE PS_EFFECT(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out  = (PS_OUT_DIFFUSE) 0;
    
	vector	vWorldDesc  = g_WorldTexture.Sample(PointSampler, In.vTexcoord);         /* World Texture */

    float   fWeight     = g_WeightTexture.Sample(PointSampler, In.vTexcoord).r;
    vector  vFactor     = g_FactorTexture.Sample(PointSampler, In.vTexcoord);
    vector  vDiffuse    = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
    
    /* Compare Depth */ 
    float4  vTranslation = mul(vWorldDesc, g_SceneViewMatrix);
    vTranslation = mul(vTranslation, g_SceneProjMatrix);
    float   fDepth = vTranslation.w;
    
    if (!(vWorldDesc.x == 0 && vWorldDesc.y == 0 && vWorldDesc.z == 0 && vWorldDesc.w == 0))
        if (((fWeight == 1.f) && (vWorldDesc.w == 0.f)) || ((fDepth < vFactor.x / vFactor.y)))
            discard;
    
    Out.vDiffuse = vector(vDiffuse.rgb / clamp(vDiffuse.a, 1e-4, 5e4), fWeight);

    return Out;
}

technique11 DefaultTechnique
{
    pass DebugSinglePass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_SINGLEDEBUG();
    }

    pass DebugMultiPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_MULTIDEBUG();
    }

    pass LightDirectionalPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_One, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_DIRECTIONAL();
    }

    pass LightPointPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_One, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_POINT();
    }

	pass DeferredScenePass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader	= NULL;
		PixelShader		= compile ps_5_0 PS_SCENE();
	}

	pass DeferredEffectPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_EffectDeferred, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader	= NULL;
		PixelShader		= compile ps_5_0 PS_EFFECT();
	}
}