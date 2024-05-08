#include    "Engine_Shader_Defines.hlsli"

Texture2D	g_EffectTextures[10];

VS_OUT_MESH VS_MAIN(VS_IN_MESH In)
{
    VS_OUT_MESH Out = (VS_OUT_MESH) 0;
	
	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);
	
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexcoord = In.vTexcoord;
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;
	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix)).xyz;
	Out.vBinormal = normalize(cross(Out.vNormal.xyz, Out.vTangent));

	return Out;
}

PS_OUT_SCENE_DC PS_TORNADO(PS_IN_MESH In)
{
	static float4	vColor = float4(0.4392132f, 0.6797233f, 0.8867924f, 1.f);
	
    PS_OUT_SCENE_DC Out = (PS_OUT_SCENE_DC) 0;

    vector	vNoiseTexture	= g_EffectTextures[2].Sample(LinearSampler, float2((In.vTexcoord.x), (In.vTexcoord.y + g_vUVRate.y) * 3.f));
	float2	vUV				= In.vTexcoord + vNoiseTexture.rg * 0.2f;
	
	vector	vMaskTexture	= g_EffectTextures[1].Sample(LinearSampler, float2(vUV.x, vUV.y));
	vector	vDiffuseTexture	= g_EffectTextures[0].Sample(LinearSampler, float2(vUV.x - g_vUVRate.x * 2.f, vUV.y * 3.f));

    Out.vDiffuse	= lerp(vDiffuseTexture, vColor, vMaskTexture);
    Out.vDiffuse	= lerp(vColor, float4(1.f, 1.f, 1.f, 1.f), Out.vDiffuse);
    Out.vDiffuse	= pow(Out.vDiffuse, 2.f);
	
    Out.vDiffuse.a	= ((Out.vDiffuse.r + Out.vDiffuse.g + Out.vDiffuse.b) / 3.f) * (1.f - In.vTexcoord.y) * 2.f;
    Out.vDiffuse.a	= saturate(pow(Out.vDiffuse.a, 2.f));
    Out.vDiffuse.a	= saturate(Out.vDiffuse.a + 0.3f);
    
    Out.vCShader    = float4(5.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT_SCENE_DC PS_METEORITE(PS_IN_MESH In)
{
    PS_OUT_SCENE_DC Out = (PS_OUT_SCENE_DC) 0;
    
    vector  vNoiseTexture   = g_EffectTextures[0].Sample(LinearSampler, float2((In.vTexcoord.x) + g_vUVRate.z, (In.vTexcoord.y) + g_vUVRate.z));

    Out.vDiffuse    = saturate(g_vColor * vNoiseTexture * 4.f);
    Out.vDiffuse.a  = (vNoiseTexture.r + vNoiseTexture.g + vNoiseTexture.b) / 3.f * 2.f;
    
    Out.vCShader    = float4(5.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT_DIFFUSE PS_SHOCKWAVE(PS_IN_MESH In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;

	vector	vMaskTexture	= g_EffectTextures[0].Sample(LinearSampler, In.vTexcoord);
    vector	vNoiseTexture	= g_EffectTextures[1].Sample(LinearSampler, float2((In.vTexcoord.x), (In.vTexcoord.y * 2.f) + g_vUVRate.x));

    Out.vDiffuse    = vNoiseTexture * vMaskTexture;
    if (((Out.vDiffuse.r + Out.vDiffuse.g + Out.vDiffuse.b) / 3.f) < 0.5f)
        discard;
    
    Out.vDiffuse    = float4(1.f, 1.f, 1.f, 1.f);

    return Out;
}

PS_OUT_DIFFUSE PS_CIRCULARINDICATOR_GAGE(PS_IN_MESH In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;

    float2 vCenter = { 0.5f, 0.5f };
    float2 vDistance = In.vTexcoord - vCenter;

    float fCurrRate = vDistance.y / 0.5f;

    Out.vDiffuse    = g_vColor;
    Out.vDiffuse.a  = min((1.f - abs(g_ColorRate - fCurrRate) * 4.f), 0.6f);
    Out.vDiffuse.a  = max(Out.vDiffuse.a, 0.1f);

    return Out;
}

PS_OUT_DIFFUSE PS_CIRCULARINDICATOR_BORDER(PS_IN_MESH In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;

    Out.vDiffuse        = float4(1.f, 0.f, 0.f, 0.5f);

    return Out;
}

PS_OUT_DIFFUSE PS_ELEMENTAL(PS_IN_MESH In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    vector  vNoiseTexture   = g_EffectTextures[0].Sample(LinearSampler, float2((In.vTexcoord.x) + g_vUVRate.x, (In.vTexcoord.y) + g_vUVRate.x));

    Out.vDiffuse    = saturate(g_vColor * vNoiseTexture * 2.f);

    return Out;
}

PS_OUT_DIFFUSE PS_BARBARA_ES2(PS_IN_MESH In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    vector	vUVTexture	    = g_EffectTextures[2].Sample(LinearSampler, float2(In.vTexcoord.x * 5.f, In.vTexcoord.y * 5.f));
    vector	vNoiseTexture	= g_EffectTextures[3].Sample(LinearSampler, float2(In.vTexcoord.x, In.vTexcoord.y));
	float2	vUV				= In.vTexcoord + vUVTexture.rg * 0.2f;
    
    vector  vDiffuseTexture = g_EffectTextures[1].Sample(LinearSampler, float2(vUV.x + g_vUVRate.x * 2.f, vUV.y + g_vUVRate.x* 2.f));
    if (vDiffuseTexture.a < 0.1f)
        discard;
    
    Out.vDiffuse        = vDiffuseTexture;
    Out.vDiffuse.rgb    = vDiffuseTexture.rgb * float3(1.1255f, 1.3569f, 1.5686f);
    Out.vDiffuse.rgb    = saturate(pow(Out.vDiffuse.rgb, 2.f));

    if (g_vUVRate.z == 4.f)
    {
        if (vNoiseTexture.g < g_vUVRate.y * 2.f)
            discard;
    }
   
    return Out;
}

PS_OUT_DIFFUSE PS_BARBARA_ES3(PS_IN_MESH In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    vector	vUVTexture	    = g_EffectTextures[2].Sample(LinearSampler, float2(In.vTexcoord.x * 5.f, In.vTexcoord.y * 5.f));
    vector	vNoiseTexture	= g_EffectTextures[3].Sample(LinearSampler, float2(In.vTexcoord.x, In.vTexcoord.y));
	float2	vUV				= In.vTexcoord + vUVTexture.rg * 0.2f;
    
    vector  vDiffuseTexture = g_EffectTextures[1].Sample(LinearSampler, float2(vUV.x + g_vUVRate.x * 2.f, vUV.y + g_vUVRate.x * 2.f));
    if (vDiffuseTexture.a < 0.1f)
        discard;
    
    Out.vDiffuse        = vDiffuseTexture;
    Out.vDiffuse.rgb    = vDiffuseTexture.rgb * float3(1.1255f, 1.3569f, 1.5686f);
    Out.vDiffuse.rgb    = saturate(pow(Out.vDiffuse.rgb, 2.f));

    if (g_vUVRate.z  == 6.f)
    {
        if (vNoiseTexture.g < g_vUVRate.y * 2.f)
            discard;
    }
   
    return Out;
}

PS_OUT_DIFFUSE PS_BARBARA_ES4(PS_IN_MESH In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    vector  vDiffuseTexture = g_EffectTextures[4].Sample(LinearSampler, float2(1.f - (In.vTexcoord.x * 3.f) + g_vUVRate.x, (In.vTexcoord.y * 0.5f)));
    if (vDiffuseTexture.a < 0.1f)
        discard;
        
    Out.vDiffuse        = vDiffuseTexture;
    Out.vDiffuse.rgb    = vDiffuseTexture.rgb * float3(1.1255f, 1.3569f, 1.5686f);
    Out.vDiffuse.rgb    = saturate(pow(Out.vDiffuse.rgb, 2.f));
  
    return Out;
}

PS_OUT_DIFFUSE PS_EFFIGYFIRE_CRACK(PS_IN_MESH In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    vector vDiffuseTexture  = g_EffectTextures[0].Sample(LinearSampler, float2(In.vTexcoord.x * 2.f, In.vTexcoord.y));
    if (vDiffuseTexture.a < 0.01f || pow(vDiffuseTexture.g * (1.f + (g_vUVRate.x - 0.5f)), 3.f) < 0.1f)
        discard;
    
    Out.vDiffuse.rgb    = saturate(pow(float3(vDiffuseTexture.rgb) * (1.f + (g_vUVRate.x - 0.25f)), 2.f));
    if (Out.vDiffuse.g < 0.2f)
        discard;

    Out.vDiffuse.a      = 1.f;

    return Out;
}

PS_OUT_DIFFUSE PS_EFFIGYFIRE_IMPACT(PS_IN_MESH In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
 
    if (In.vTexcoord.y > 0.75f - g_iIndex * 0.1f)
        discard;
    
    vector vDiffuseTexture  = g_EffectTextures[0].Sample(LinearSampler, float2(In.vTexcoord.x * 2.f, In.vTexcoord.y));

    Out.vDiffuse        = saturate(vDiffuseTexture * 2.f);
    Out.vDiffuse.g      = max(Out.vDiffuse.g, 0.4f);
    Out.vDiffuse.a      = max(Out.vDiffuse.a, 0.6f);
    
    return Out;
}

PS_OUT_DIFFUSE PS_EFFIGYFIRE_IMPACT_DISSOLVE(PS_IN_MESH In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
 
    if (In.vTexcoord.y > 0.75f - g_iIndex * 0.1f)
        discard;
    
    vector vNoiseTexture    = g_EffectTextures[1].Sample(LinearSampler, float2(In.vTexcoord.x, In.vTexcoord.y));
    vector vDiffuseTexture  = g_EffectTextures[0].Sample(LinearSampler, float2(In.vTexcoord.x * 2.f, In.vTexcoord.y));
    
    Out.vDiffuse        = saturate(vDiffuseTexture * 2.f);
    Out.vDiffuse.g      = max(Out.vDiffuse.g, 0.4f);
    Out.vDiffuse.a      = max(Out.vDiffuse.a, 0.6f);
    
    /* Dissolve */
    if (vNoiseTexture.g < 1.f - (g_ColorRate / 10.f))
        discard;
    
    return Out;
}

PS_OUT_DIFFUSE PS_EFFIGYFIRE_THORN(PS_IN_MESH In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    if (ceil(In.vTexcoord.x * 10.f / 0.25f) % 3 == 0 || 
        ceil(In.vTexcoord.x * 10.f / 0.25f) % 5 == 0 || ceil(In.vTexcoord.x * 10.f / 0.25f) % 7 == 0)
        discard;

    vector  vDiffuseTexture = g_EffectTextures[1].Sample(WrapClampSampler, float2(In.vTexcoord.x * 20.f, (In.vTexcoord.y - 1.f) + g_vUVRate[1 + g_iIndex] * 1.5f));
    vector  vNoiseTexture   = g_EffectTextures[2].Sample(LinearSampler,  float2(In.vTexcoord.x * 20.f, In.vTexcoord.y * 2.f));
    if ((vDiffuseTexture.r + vDiffuseTexture.g + vDiffuseTexture.b) / 3.f < 0.1f)
        discard;
    
    if (vNoiseTexture.r + 0.2f < g_ColorRate * 1.5f)
        discard;

    Out.vDiffuse.rg     = vDiffuseTexture.rg * float2(1.5f, 1.f);
    if (Out.vDiffuse.g > Out.vDiffuse.r)
        Out.vDiffuse.r = Out.vDiffuse.g * 2.f;
    
    Out.vDiffuse.b      = 0.f;
    Out.vDiffuse.rgb    = saturate(Out.vDiffuse.rgb);
    
    Out.vDiffuse.a      = 1.f;

    return Out;
}

PS_OUT_DIFFUSE PS_EFFIGYFIRE_INNER_PROJECTILE(PS_IN_MESH In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    vector  vDiffuseTexture = g_EffectTextures[1].Sample(LinearSampler, float2(In.vTexcoord.x, In.vTexcoord.y));

    Out.vDiffuse.rgb    = saturate(vDiffuseTexture.rgb * float3(1.f, 0.5f, 0.f));
    Out.vDiffuse.a      = 1.f;

    return Out;
}

PS_OUT_DIFFUSE PS_EFFIGYFIRE_OUTER_PROJECTILE(PS_IN_MESH In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    vector	vUVTexture	    = g_EffectTextures[1].Sample(LinearSampler, float2(In.vTexcoord.x, In.vTexcoord.y * 3.f));
	float2	vUV				= In.vTexcoord + vUVTexture.rg * 0.2f;
    
    vector  vDiffuseTexture = g_EffectTextures[0].Sample(LinearSampler, float2(vUV.x + g_vUVRate.x, vUV.y + g_vUVRate.x * 2.f));
    if (vDiffuseTexture.a < 0.1f)
        discard;

    Out.vDiffuse.rgb    = saturate(pow(vDiffuseTexture.rgb, 2.f) * float3(0.8f, 1.f, 1.f) * 4.f);
    Out.vDiffuse.a      = 1.f;

    return Out;
}

PS_OUT_DIFFUSE PS_EFFIGYFIRE_EXTENT(PS_IN_MESH In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;

	vector	vMaskTexture	= g_EffectTextures[0].Sample(LinearSampler, In.vTexcoord);
    vector	vNoiseTexture	= g_EffectTextures[1].Sample(LinearSampler, float2((In.vTexcoord.x), (In.vTexcoord.y * 2.f) + g_vUVRate.x));

    Out.vDiffuse    = vNoiseTexture * vMaskTexture;
    if (((Out.vDiffuse.r + Out.vDiffuse.g + Out.vDiffuse.b) / 3.f) < 0.5f)
        discard;
    
    Out.vDiffuse.rgb    = saturate(vNoiseTexture.rgb * float3(3.f, 0.75f, 0.5f) * 1.5f);
    Out.vDiffuse.a      = 1.f;

    return Out;
}

vector ConvertLiteralSampling(sampler Sampler, float2 vUV)
{
    switch (g_iIndex)
    {
        case 0:
            return g_EffectTextures[2].Sample(Sampler, vUV);
        case 1:
            return g_EffectTextures[3].Sample(Sampler, vUV);
        case 2:
            return g_EffectTextures[4].Sample(Sampler, vUV);
        case 3:
            return g_EffectTextures[5].Sample(Sampler, vUV);
        default:
            return g_EffectTextures[2].Sample(Sampler, vUV);
    }
}

PS_OUT_DIFFUSE PS_EFFIGYFIRE_EXTENTCC(PS_IN_MESH In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    vector  vDiffuseTexture = ConvertLiteralSampling(LinearSampler,In.vTexcoord);
    if (vDiffuseTexture.r < 0.1f)
        discard;
    
    Out.vDiffuse.rgb    = saturate(vDiffuseTexture.r * float3(3.f, 0.75f, 0.5f) * 1.5f);
    Out.vDiffuse.a      = vDiffuseTexture.r;

    return Out;
}

PS_OUT_DIFFUSE PS_EFFIGYFIRE_PILLAR_CRACK(PS_IN_MESH In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    vector  vDiffuseTexture = ConvertLiteralSampling(LinearSampler,In.vTexcoord);
    if (vDiffuseTexture.r < 0.1f)
        discard;
    
    Out.vDiffuse.rgb    = saturate(vDiffuseTexture.r * float3(4.f, 0.75f, 0.5f) * g_vUVRate.w * 0.75f);
    Out.vDiffuse.a      = Out.vDiffuse.r;

    if (Out.vDiffuse.a < 0.1f)
        discard;
	
    return Out;
}

PS_OUT_DIFFUSE PS_EFFIGYFIRE_AURA(PS_IN_MESH In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
 
    vector	vUVTexture	    = g_EffectTextures[1].Sample(LinearSampler, float2(In.vTexcoord.x, In.vTexcoord.y));
	float2	vUV				= In.vTexcoord + g_vUVRate.x + vUVTexture.rg * 0.2f;
    
    vector  vDiffuseTexture = g_EffectTextures[0].Sample(LinearSampler, vUV);
    if ((1.f - In.vTexcoord.y) * vDiffuseTexture.a < 0.1f)
        discard;
    
    Out.vDiffuse        = vDiffuseTexture;

    return Out;
}

PS_OUT_SCENE_DC PS_EFFIGYFIRE_TRSCOW(PS_IN_MESH In)
{
    PS_OUT_SCENE_DC Out = (PS_OUT_SCENE_DC) 0;
    
    vector vMtrlDiffuse = g_EffectTextures[0].Sample(LinearSampler, float2(In.vTexcoord.x - g_vUVRate.z, In.vTexcoord.y - g_vUVRate.z));
    if (vMtrlDiffuse.a < 0.01f)
        discard;
    
    Out.vDiffuse        = vMtrlDiffuse;
    Out.vDiffuse.a      = Out.vDiffuse.a * saturate(pow((1.f - In.vTexcoord.y), 1.5f)) * saturate(g_vUVRate.y * 2.f);
    if (Out.vDiffuse.a < 0.01f)
        discard;
    
    Out.vCShader = float4(10.f, 0.f, 0.f, 0.f);

    return Out;
}

PS_OUT_SCENE_DNWF PS_DUNGEONEXIT_MAGICLINE(PS_IN_MESH In)
{
    PS_OUT_SCENE_DNWF Out = (PS_OUT_SCENE_DNWF) 0;
    
    vector vMtrlDiffuse = g_EffectTextures[0].Sample(LinearSampler, In.vTexcoord);
    
    Out.vDiffuse        = float4(0.09558821f, 0.8877282f, 1.f, 1.f);
    Out.vDiffuse.rgb    = Out.vDiffuse.rgb - vMtrlDiffuse.rgb * 0.2f;
    
    Out.vWorld  = In.vWorldPos;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    
    /* SceneFactor (R:IsAdditionalExposure (not -1 is True), G:Outline (1 is False), B:FaceFow, A:FaceRev) */
    Out.vFactor = float4(1.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT_DIFFUSE PS_DUNGEONEXIT_CYLINDER(PS_IN_MESH In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    vector vNoiseTexture    = g_EffectTextures[3].Sample(LinearSampler, In.vTexcoord);
	float2	vUV				= In.vTexcoord + vNoiseTexture.rg * 0.2f;
	
    vector vMtrlDiffuse     = g_EffectTextures[2].Sample(LinearSampler, float2(vUV.x, vUV.y));

    Out.vDiffuse    = ((vMtrlDiffuse.r + vMtrlDiffuse.g + vMtrlDiffuse.b) / 3.f) * float4(0.09558821f, 0.8877282f, 1.f, 1.f);
    if (Out.vDiffuse.a < 0.01f)
        discard;
    
    return Out;
}

PS_OUT_DIFFUSE PS_DUNGEONEXIT_RING(PS_IN_MESH In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    vector vMtrlDiffuse = g_EffectTextures[4].Sample(LinearSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.01f)
        discard;
    
    Out.vDiffuse    = vMtrlDiffuse;

    return Out;
}

PS_OUT_SCENE_DC PS_REACTOR_ACTIVEFX(PS_IN_MESH In)
{
    PS_OUT_SCENE_DC Out = (PS_OUT_SCENE_DC) 0;
    
    vector vMtrlDiffuse = g_EffectTextures[0].Sample(LinearSampler, float2(In.vTexcoord.x + g_vUVRate.x, In.vTexcoord.y + g_vUVRate.y));
    vector vMaskDiffuse = g_EffectTextures[1].Sample(LinearClampSampler, float2(In.vTexcoord.x, In.vTexcoord.y));

    Out.vDiffuse        = vMtrlDiffuse * g_vColor;
    Out.vDiffuse.rgb    = saturate(Out.vDiffuse.rgb * 2.f);
    Out.vDiffuse.a      = (Out.vDiffuse.a * vMaskDiffuse.r);
    if (Out.vDiffuse.a < 0.1f)
        discard;
    
    Out.vCShader.r      = 10.f;

    return Out;
}

technique11 DefaultTechnique
{
    pass TornadoPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader	= NULL;
        PixelShader		= compile ps_5_0 PS_TORNADO();
    }

    pass MeteoritePass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader	= NULL;
        PixelShader		= compile ps_5_0 PS_METEORITE();
    }

    pass ShockWavePass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader	= NULL;
        PixelShader		= compile ps_5_0 PS_SHOCKWAVE();
    }

    pass CircularIndiGagePass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader	= NULL;
        PixelShader		= compile ps_5_0 PS_CIRCULARINDICATOR_GAGE();
    }

    pass CircularIndiBorderPass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader	= NULL;
        PixelShader		= compile ps_5_0 PS_CIRCULARINDICATOR_BORDER();
    }

    pass ElementalPass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader	= NULL;
        PixelShader		= compile ps_5_0 PS_ELEMENTAL();
    }

    pass BarbaraES2Pass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader	= NULL;
        PixelShader		= compile ps_5_0 PS_BARBARA_ES2();
    }

    pass BarbaraES3Pass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader	= NULL;
        PixelShader		= compile ps_5_0 PS_BARBARA_ES3();
    }

    pass BarbaraES4Pass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader	= NULL;
        PixelShader		= compile ps_5_0 PS_BARBARA_ES4();
    }

    pass EffigyFireCrackPass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader	= NULL;
        PixelShader		= compile ps_5_0 PS_EFFIGYFIRE_CRACK();
    }

    pass EffigyFireImpactPass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader	= NULL;
        PixelShader		= compile ps_5_0 PS_EFFIGYFIRE_IMPACT();
    }

    pass EffigyFireImpactDissolvePass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader	= NULL;
        PixelShader		= compile ps_5_0 PS_EFFIGYFIRE_IMPACT_DISSOLVE();
    }

    pass EffigyFireThornPass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader	= NULL;
        PixelShader		= compile ps_5_0 PS_EFFIGYFIRE_THORN();
    }

    pass EffigyFireInnerProjectilePass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader	= NULL;
        PixelShader		= compile ps_5_0 PS_EFFIGYFIRE_INNER_PROJECTILE();
    }

    pass EffigyFireOuterProjectilePass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader	= NULL;
        PixelShader		= compile ps_5_0 PS_EFFIGYFIRE_OUTER_PROJECTILE();
    }

    pass EffigyFireExtentPass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader	= NULL;
        PixelShader		= compile ps_5_0 PS_EFFIGYFIRE_EXTENT();
    }

    pass EffigyFireExtentCCPass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader	= NULL;
        PixelShader		= compile ps_5_0 PS_EFFIGYFIRE_EXTENTCC();
    }

    pass EffigyFirePillarCrackPass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader	= NULL;
        PixelShader		= compile ps_5_0 PS_EFFIGYFIRE_PILLAR_CRACK();
    }

    pass EffigyFireAuraPass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader	= NULL;
        PixelShader		= compile ps_5_0 PS_EFFIGYFIRE_AURA();
    }

    pass EffigyFireTrsCowPass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_EFFIGYFIRE_TRSCOW();
    }

    pass DungeonExitMagiclinePass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_DUNGEONEXIT_MAGICLINE();
    }

    pass DungeonExitCylinderPass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_DUNGEONEXIT_CYLINDER();
    }

    pass DungeonExitRingPass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_DUNGEONEXIT_RING();
    }

    pass ReactorActiveFXPass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_AdditionalDiffuse, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_REACTOR_ACTIVEFX();
    }
}