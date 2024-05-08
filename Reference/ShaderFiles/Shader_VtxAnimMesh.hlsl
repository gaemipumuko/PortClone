#include "Engine_Shader_Defines.hlsli"

matrix		g_BoneMatrices[256];

int         g_iRampIndex = -1;
float       g_fFactor;
float		g_fDissolve;
float       g_fDissolveSize;
float3		g_vDissolveColor;
Texture2D	g_DissolveTexture;
Texture2D	g_PetrificationTexture;
Texture2D   g_FreezingDiffuseTexture;
Texture2D   g_FreezingNoiseTexture;

Texture2D	g_FaceShadowTexture;

float2x2 RotationMatrix(float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    return float2x2(c, -s, s, c);
}

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
    Out.vWorldPos = mul(vector(vPosition), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

    return Out;
}

VS_OUT_POSNORTEX VS_ROTATE(VS_IN_ANIMMESH In)
{
    VS_OUT_POSNORTEX Out = (VS_OUT_POSNORTEX) 0;

    matrix matWV, matWVP, matOldWV, matOldWVP;

    float fW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

	/* 현재 정점에게 적용되어야할 뼈 네개의 상태행렬 합쳐서 하나의 행렬로 만든다. */
    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fW;

	/* 뼈의 행렬을 정점의 로컬위치에 곱해서 정점의 로컬위치가 뼈의 위치로 바뀌게끔 처리한다. */
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

        // 시간에 따라 천천히 회전하는 각도 계산
    float rotationAngle = radians(45.0) * g_fTimeAcc;

    // 회전 행렬을 사용하여 텍스처 좌표를 회전
    float2x2 rotationMatrix = RotationMatrix(rotationAngle);
    float2 rotatedTexCoord = mul(rotationMatrix, In.vTexcoord - 0.5) + 0.5;
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord = rotatedTexCoord;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;

    return Out;
}

PS_OUT_SCENE_DNWLFF PS_MAIN(PS_IN_POSNORTEX In)
{
    PS_OUT_SCENE_DNWLFF Out = (PS_OUT_SCENE_DNWLFF) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(AnisotropicSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.1f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;
    Out.vWorld = In.vWorldPos;
	
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

    if (g_bLightMapTexture == true)
    {
        vector vLightMap = g_LightMapTexture.Sample(AnisotropicSampler, In.vTexcoord);
        Out.vLightMap = vLightMap;
    }
    else
    {
        Out.vLightMap = vector(0.f, 0.f, 0.f, 0.f);
    }
    
    /* SceneFactor (R:IsAdditionalExposure (not -1 is True), G:Outline (1 is False), B:FaceFow, A:FaceRev) */
    if (g_bFaceMesh == true)
    {
        float4 vLook    = g_WorldMatrix[2];
        float4 vRight   = g_WorldMatrix[0];
        
        Out.vFace       = float4(vLook.x, vLook.z, vRight.x, vRight.z);
        
        Out.vFactor.rg  = float2(-1.f, 0.f);
        Out.vFactor.b   = g_FaceShadowTexture.Sample(AnisotropicSampler, In.vTexcoord);
        Out.vFactor.a   = g_FaceShadowTexture.Sample(AnisotropicSampler, float2(-In.vTexcoord.x, In.vTexcoord.y));
    }
    else
    {
        Out.vFace       = float4(0.f, 0.f, 0.f, 0.f);
        Out.vFactor     = float4(-1.f, 0.f, 0.f, 0.f);
    }
    
	return Out;
}

PS_OUT_SCENE_DNWF PS_MTRAIL(PS_IN_POSNORTEX In)
{
    PS_OUT_SCENE_DNWF Out = (PS_OUT_SCENE_DNWF) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(AnisotropicSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.1f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vDiffuse.rgb = saturate(Out.vDiffuse.rgb * float3(0.1255f, 0.3569f, 0.5686f) * (1.f + (g_fFactor / 2.5f)));
    
    Out.vWorld = In.vWorldPos;
    
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    
    /* SceneFactor (R:IsAdditionalExposure, G:IsFaceMesh) */
    Out.vFactor = float4(-1.f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT_SCENE_DNWLF PS_ADDEXPOSURE(PS_IN_POSNORTEX In)
{
    PS_OUT_SCENE_DNWLF Out = (PS_OUT_SCENE_DNWLF) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(AnisotropicSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.1f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;
    Out.vWorld = In.vWorldPos;
	
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

    if (g_bLightMapTexture == true)
    {
        vector vLightMap = g_LightMapTexture.Sample(AnisotropicSampler, In.vTexcoord);
        Out.vLightMap = vLightMap;
    }
    else
    {
        Out.vLightMap = vector(0.f, 0.f, 0.f, 0.f);
    }
    
    /* SceneFactor (R:IsAdditionalExposure, G:IsFaceMesh) */
    Out.vFactor = float4(1.f, 0.f, 0.f, 0.f);

	return Out;
}

PS_OUT_SCENE_DNWLFF PS_RAMP(PS_IN_POSNORTEX In)
{
    PS_OUT_SCENE_DNWLFF Out = (PS_OUT_SCENE_DNWLFF) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(AnisotropicSampler, In.vTexcoord);
    vMtrlDiffuse.a = 1.f;
    
    if (vMtrlDiffuse.a < 0.1f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;
    Out.vWorld = In.vWorldPos;
	
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

    if (g_bLightMapTexture == true)
    {
        vector vLightMap = g_LightMapTexture.Sample(AnisotropicSampler, In.vTexcoord);
        Out.vLightMap = vLightMap;
    }
    else
    {
        Out.vLightMap = vector(0.f, 0.f, 0.f, 0.f);
    }
    
    /* SceneFactor (R:IsAdditionalExposure (not -1 is True), G:Outline (1 is False), B:FaceFow, A:FaceRev) */
    if (g_bFaceMesh == true)
    {
        float4 vLook    = g_WorldMatrix[2];
        float4 vRight   = g_WorldMatrix[0];
        
        Out.vFace       = float4(vLook.x, vLook.z, vRight.x, vRight.z);
        
        Out.vFactor.rg  = float2(-1.f, 0.f);
        Out.vFactor.b   = g_FaceShadowTexture.Sample(AnisotropicSampler, In.vTexcoord);
        Out.vFactor.a   = g_FaceShadowTexture.Sample(AnisotropicSampler, float2(-In.vTexcoord.x, In.vTexcoord.y));
    }
    else
    {
        Out.vFace       = float4(0.f, 0.f, 0.f, 0.f);
        Out.vFactor     = float4(-1.f, 0.f, 0.f, 0.f);
    }
   
	return Out;
}

PS_OUT_SCENE_DNWLFF PS_MAIN_DISSOLVE(PS_IN_POSNORTEX In)
{
    PS_OUT_SCENE_DNWLFF Out = (PS_OUT_SCENE_DNWLFF) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(AnisotropicSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.1f)
        discard;
    
    vector vDissolve = g_DissolveTexture.Sample(AnisotropicSampler, In.vTexcoord);

    Out.vDiffuse = vMtrlDiffuse;
    Out.vWorld = In.vWorldPos;
	
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

    if (g_bLightMapTexture == true)
    {
        vector vLightMap = g_LightMapTexture.Sample(AnisotropicSampler, In.vTexcoord);
        Out.vLightMap = vLightMap;
    }
    else
    {
        Out.vLightMap = vector(0.f, 0.f, 0.f, 0.f);
    }
	
    if (vDissolve.r < g_fDissolve)
        discard;

    float fLength = g_fDissolve + g_fDissolveSize;
    if (vDissolve.r <= fLength)
    {
        float fAtt = sin(radians(90.f * (fLength - vDissolve.r) * (1 / g_fDissolveSize)));
        Out.vDiffuse = vMtrlDiffuse;
        Out.vDiffuse.rgb += g_vDissolveColor * fAtt;
    }

    /* SceneFactor (R:IsAdditionalExposure (not -1 is True), G:Outline (1 is False), B:FaceFow, A:FaceRev) */
    if (g_bFaceMesh == true)
    {
        float4 vLook    = g_WorldMatrix[2];
        float4 vRight   = g_WorldMatrix[0];
        
        Out.vFace       = float4(vLook.x, vLook.z, vRight.x, vRight.z);
        
        Out.vFactor.rg  = float2(-1.f, 0.f);
        Out.vFactor.b   = g_FaceShadowTexture.Sample(AnisotropicSampler, In.vTexcoord);
        Out.vFactor.a   = g_FaceShadowTexture.Sample(AnisotropicSampler, float2(-In.vTexcoord.x, In.vTexcoord.y));
    }
    else
    {
        Out.vFace       = float4(0.f, 0.f, 0.f, 0.f);
        Out.vFactor     = float4(-1.f, 0.f, 0.f, 0.f);
    }
   
    return Out;
}

PS_OUT_SCENE_DNWLFF PS_RAMP_DISSOLVE(PS_IN_POSNORTEX In)
{
    PS_OUT_SCENE_DNWLFF Out = (PS_OUT_SCENE_DNWLFF) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(AnisotropicSampler, In.vTexcoord);
    vMtrlDiffuse.a = 1.f;
    
    if (vMtrlDiffuse.a < 0.1f)
        discard;
    
    vector vDissolve = g_DissolveTexture.Sample(AnisotropicSampler, In.vTexcoord);

    Out.vDiffuse = vMtrlDiffuse;
    Out.vWorld = In.vWorldPos;
	
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	
    if (g_bLightMapTexture == true)
    {
        vector vLightMap = g_LightMapTexture.Sample(AnisotropicSampler, In.vTexcoord);
        Out.vLightMap = vLightMap;
    }
    else
    {
        Out.vLightMap = vector(0.f, 0.f, 0.f, 0.f);
    }
	
    if (vDissolve.r < g_fDissolve)
        discard;

    float fLength = g_fDissolve + g_fDissolveSize;
    if (vDissolve.r <= fLength)
    {
        float fAtt = sin(radians(90.f * (fLength - vDissolve.r) * (1 / g_fDissolveSize)));
        Out.vDiffuse = vMtrlDiffuse;
        Out.vDiffuse.rgb += g_vDissolveColor * fAtt;
    }

    /* SceneFactor (R:IsAdditionalExposure (not -1 is True), G:Outline (1 is False), B:FaceFow, A:FaceRev) */
    if (g_bFaceMesh == true)
    {
        float4 vLook    = g_WorldMatrix[2];
        float4 vRight   = g_WorldMatrix[0];
        
        Out.vFace       = float4(vLook.x, vLook.z, vRight.x, vRight.z);
        
        Out.vFactor.rg  = float2(-1.f, 0.f);
        Out.vFactor.b   = g_FaceShadowTexture.Sample(AnisotropicSampler, In.vTexcoord);
        Out.vFactor.a   = g_FaceShadowTexture.Sample(AnisotropicSampler, float2(-In.vTexcoord.x, In.vTexcoord.y));
    }
    else
    {
        Out.vFace       = float4(0.f, 0.f, 0.f, 0.f);
        Out.vFactor     = float4(-1.f, 0.f, 0.f, 0.f);
    }
    
    return Out;
}

/* Phoenix */
PS_OUT_SCENE_DNWLF PS_PHOENIX(PS_IN_POSNORTEX In)
{
    PS_OUT_SCENE_DNWLF Out = (PS_OUT_SCENE_DNWLF) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(AnisotropicSampler, In.vTexcoord);

    if(vMtrlDiffuse.r < 0.1f)
        discard;

    vMtrlDiffuse.g = vMtrlDiffuse.r * 0.7f;
    vMtrlDiffuse.a = vMtrlDiffuse.r + 0.8f;

    vMtrlDiffuse.r = 1.f;

    Out.vDiffuse = vMtrlDiffuse;
    Out.vWorld = In.vWorldPos;
	
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    if (g_bFaceMesh == true)
        Out.vNormal.w = 1.f;
	
    if (g_bLightMapTexture == true)
    {
        vector vLightMap = g_LightMapTexture.Sample(AnisotropicSampler, In.vTexcoord);
        Out.vLightMap = vLightMap;
    }
    else
    {
        Out.vLightMap = vector(0.f, 0.f, 0.f, 0.f);
    }
    
    Out.vFactor = float4(1.f, 0.f, 0.f, 0.f);
   
	return Out;
}

PS_OUT_SCENE_DNWLF PS_MAIN_PETRIFICATION(PS_IN_POSNORTEX In)
{
    PS_OUT_SCENE_DNWLF Out = (PS_OUT_SCENE_DNWLF) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(AnisotropicSampler, In.vTexcoord);
    vector vPetriDiffuse = g_PetrificationTexture.Sample(PointSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.1f)
        discard;
    
    if (vPetriDiffuse.g == 0.f)
    {
        float fColor = vPetriDiffuse.r * 0.4f;
        Out.vDiffuse = vector(fColor, fColor, fColor, vMtrlDiffuse.a);
    }
    else
        Out.vDiffuse = vector(1.f, (vPetriDiffuse.r + vPetriDiffuse.g) * 0.5f, 0.f, vMtrlDiffuse.a);
    
    Out.vWorld = In.vWorldPos;
	
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    if (g_bFaceMesh == true)
        Out.vNormal.w = 1.f;
	
    if (g_bLightMapTexture == true)
    {
        vector vLightMap = g_LightMapTexture.Sample(AnisotropicSampler, In.vTexcoord);
        Out.vLightMap = vLightMap;
    }
    else
    {
        Out.vLightMap = vector(0.f, 0.f, 0.f, 0.f);
    }
    
    Out.vFactor = float4(1.f, 0.f, 0.f, 0.f);

    return Out;
}

PS_OUT_SCENE_DNWLF PS_MAIN_FREEZING(PS_IN_POSNORTEX In)
{
    PS_OUT_SCENE_DNWLF Out = (PS_OUT_SCENE_DNWLF) 0;
    
    vector vMtrlDiffuse = g_FreezingDiffuseTexture.Sample(AnisotropicSampler, In.vTexcoord);
    vector vEffectDiffuse = g_FreezingNoiseTexture.Sample(PointSampler, In.vTexcoord);

    vector vAddColor = { 0.7f, 0.7f, 0.7f, 1.f };
    
    Out.vDiffuse = vMtrlDiffuse + vAddColor * vEffectDiffuse.r;
    
    Out.vWorld = In.vWorldPos;
	
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    if (g_bFaceMesh == true)
        Out.vNormal.w = 1.f;
	
    if (g_bLightMapTexture == true)
    {
        vector vLightMap = g_LightMapTexture.Sample(AnisotropicSampler, In.vTexcoord);
        Out.vLightMap = vLightMap;
    }
    else
    {
        Out.vLightMap = vector(0.f, 0.f, 0.f, 0.f);
    }
    
    Out.vFactor = float4(1.f, 0.f, 0.f, 0.f);
	
    return Out;
}

PS_OUT_SCENE_DNWLF PS_EFF_METEOR_TRAIL(PS_IN_POSNORTEX In)
{
    PS_OUT_SCENE_DNWLF Out = (PS_OUT_SCENE_DNWLF) 0;
    
    float2 vPos = { 0.f, 0.5f };
    float2 vScale = { 5.f, 1.f };
    
    if (In.vTexcoord.y > g_fTimeAcc*0.2f)
        discard;
    
    //if (In.vTexcoord.y<0.1f)
    //    discard;
    
    vector vMaskTex = g_Texture2.Sample(AnisotropicSampler, vPos + In.vTexcoord * vScale + float2(g_fTimeAcc * 0.6, -g_fTimeAcc / 2.f));
    
    vPos.y =  vMaskTex.r;
    vPos.x  =  vMaskTex.g;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(AnisotropicSampler,In.vTexcoord );
    
    
    //vMaskTex.g = clamp(vMaskTex.g, 0.5f, 0.8f);
    
    vector vRampTex = g_Texture.Sample(AnisotropicSampler, float2(vMaskTex.r, vMaskTex.g));

    float fGrayScale = (vRampTex.r + vRampTex.g + vRampTex.b) / 3.f;
    
    vRampTex.b = 0.f;
    //vRampTex.r = clamp(vRampTex.r, 100.f / 255.f, 240.f/255.f);
    
    if (fGrayScale<0.1f)
        discard;

    //TEST
    vector vColor = { vMaskTex.r, vMaskTex.r, vMaskTex.r, 1.f };
    vColor.a = vMaskTex.g * 0.15f;
    if (vColor.a < 0.1f)
        discard;
    //TEST_END
    
    vRampTex.a = vMtrlDiffuse.b;
    
    if (vRampTex.a < 0.05f)
        discard;
   
    Out.vDiffuse = vColor;
    
    
    Out.vWorld = In.vWorldPos;
	
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    if (g_bFaceMesh == true)
        Out.vNormal.w = 1.f;
	
    if (g_bLightMapTexture == true)
    {
        vector vLightMap = g_LightMapTexture.Sample(AnisotropicSampler, In.vTexcoord);
        Out.vLightMap = vLightMap;
    }
    else
    {
        Out.vLightMap = vector(0.f, 0.f, 0.f, 0.f);
    }
    
    /* SceneFactor (R:IsAdditionalExposure, G:IsFaceMesh) */
    Out.vFactor = float4(1.f, 0.f, 0.f, 0.f);
	
    return Out;
}


PS_OUT_DIFFUSE PS_EFF_WIND_FIELD(PS_IN_POSNORTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    float2 vPos = { 0.f,g_fTimeAcc };
    float2 vScale = { 5.f, 1.f };
    float2 vNoiseScale = { 1.f, 1.f };
    
    vector vNoiseTex    = g_Texture.Sample(AnisotropicSampler, In.vTexcoord);
    vector vMaskTex     = g_Texture2.Sample(AnisotropicSampler, vPos + In.vTexcoord * vScale);
    
    vector vWindColor   = vector(0.7f, 0.7f, 0.7f, 0.5f);
    vWindColor.a        = pow(vMaskTex.b, 4.f);
    if (vWindColor.a < 0.01f)
        discard;
    
    Out.vDiffuse        = vWindColor;
    // Out.vDiffuse.a      = Out.vDiffuse.a * saturate((1.f - In.vTexcoord.y) + 0.2f * In.vTexcoord.x);
     
    return Out;
}

PS_OUT_SCENE_DNWLF PS_EFFIGY_FIRE(PS_IN_POSNORTEX In)
{
    PS_OUT_SCENE_DNWLF Out = (PS_OUT_SCENE_DNWLF) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(AnisotropicSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.1f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;
    //Out.vDiffuse.rgb = vMtrlDiffuse.rgb * float3(1.5686f, 1.1359f, 1.f);
    Out.vDiffuse.rgb = vMtrlDiffuse.rgb * float3(1.5686f, 0.9f, 1.f);
    Out.vDiffuse.rgb = pow(Out.vDiffuse.rgb * 2.f, 1.5f);
    Out.vDiffuse.rgb = saturate(Out.vDiffuse.rgb);
    
    Out.vWorld = In.vWorldPos;
	
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

    if (g_bLightMapTexture == true)
    {
        vector vLightMap = g_LightMapTexture.Sample(AnisotropicSampler, In.vTexcoord);
        Out.vLightMap = vLightMap;
    }
    else
    {
        Out.vLightMap = vector(0.f, 0.f, 0.f, 0.f);
    }
    
    Out.vFactor = float4(-1.f, 0.f, 0.f, 0.f);

    return Out;
}

PS_OUT_SCENE_DNWLF PS_NPC_BLACKHAIR(PS_IN_POSNORTEX In)
{
    PS_OUT_SCENE_DNWLF Out = (PS_OUT_SCENE_DNWLF) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(AnisotropicSampler, In.vTexcoord);
    vMtrlDiffuse.a = 1.f;
    
    if (vMtrlDiffuse.a < 0.1f)
        discard;

    vMtrlDiffuse.rgb -= 0.5f;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vWorld = In.vWorldPos;
	
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

    if (g_bLightMapTexture == true)
    {
        vector vLightMap = g_LightMapTexture.Sample(AnisotropicSampler, In.vTexcoord);
        Out.vLightMap = vLightMap;
    }
    else
    {
        Out.vLightMap = vector(0.f, 0.f, 0.f, 0.f);
    }
    
    Out.vFactor = float4(-1.f, 0.f, 0.f, 0.f);
   
    return Out;
}

PS_OUT_SCENE_DNWLF PS_NPC_BROWNHAIR(PS_IN_POSNORTEX In)
{
    PS_OUT_SCENE_DNWLF Out = (PS_OUT_SCENE_DNWLF) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(AnisotropicSampler, In.vTexcoord);
    vMtrlDiffuse.a = 1.f;
    
    if (vMtrlDiffuse.a < 0.1f)
        discard;

    vMtrlDiffuse.r -= 0.45f;
    vMtrlDiffuse.g -= 0.55f;
    vMtrlDiffuse.b -= 0.9f;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vWorld = In.vWorldPos;
	
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

    if (g_bLightMapTexture == true)
    {
        vector vLightMap = g_LightMapTexture.Sample(AnisotropicSampler, In.vTexcoord);
        Out.vLightMap = vLightMap;
    }
    else
    {
        Out.vLightMap = vector(0.f, 0.f, 0.f, 0.f);
    }
    
    Out.vFactor = float4(-1.f, 0.f, 0.f, 0.f);
   
    return Out;
}

PS_OUT_SCENE_DNWLF PS_NPC_LIGHTBROWNHAIR(PS_IN_POSNORTEX In)
{
    PS_OUT_SCENE_DNWLF Out = (PS_OUT_SCENE_DNWLF) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(AnisotropicSampler, In.vTexcoord);
    vMtrlDiffuse.a = 1.f;
    
    if (vMtrlDiffuse.a < 0.1f)
        discard;

    vMtrlDiffuse.r -= 0.25f;
    vMtrlDiffuse.g -= 0.35f;
    vMtrlDiffuse.b -= 0.9f;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vWorld = In.vWorldPos;
	
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

    if (g_bLightMapTexture == true)
    {
        vector vLightMap = g_LightMapTexture.Sample(AnisotropicSampler, In.vTexcoord);
        Out.vLightMap = vLightMap;
    }
    else
    {
        Out.vLightMap = vector(0.f, 0.f, 0.f, 0.f);
    }
    
    Out.vFactor = float4(-1.f, 0.f, 0.f, 0.f);
   
    return Out;
}

PS_OUT_SCENE_DNWLF PS_NPC_YELLOWBROWNHAIR(PS_IN_POSNORTEX In)
{
    PS_OUT_SCENE_DNWLF Out = (PS_OUT_SCENE_DNWLF) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(AnisotropicSampler, In.vTexcoord);
    vMtrlDiffuse.a = 1.f;
    
    if (vMtrlDiffuse.a < 0.1f)
        discard;

    vMtrlDiffuse.r -= 0.15f;
    vMtrlDiffuse.g -= 0.25f;
    vMtrlDiffuse.b -= 0.9f;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vWorld = In.vWorldPos;
	
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

    if (g_bLightMapTexture == true)
    {
        vector vLightMap = g_LightMapTexture.Sample(AnisotropicSampler, In.vTexcoord);
        Out.vLightMap = vLightMap;
    }
    else
    {
        Out.vLightMap = vector(0.f, 0.f, 0.f, 0.f);
    }
    
    Out.vFactor = float4(-1.f, 0.f, 0.f, 0.f);
   
    return Out;
}

Texture2D g_BoreasBodyLightMap;
Texture2D g_BoreasBodyIce;

PS_OUT_SCENE_DNWLF PS_BOREAS_BODY_1(PS_IN_POSNORTEX In)
{
    PS_OUT_SCENE_DNWLF Out = (PS_OUT_SCENE_DNWLF) 0;
    
    vector vColor = { 65.0f / 255.0f,139.0f / 255.0f, 197.0f / 255.0f, 1.f };
    
    vector vMtrlIce = g_BoreasBodyIce.Sample(AnisotropicSampler, In.vTexcoord);
    vector vMtrlLightMap = g_BoreasBodyLightMap.Sample(AnisotropicSampler, In.vTexcoord);
    
    Out.vDiffuse = vColor + vMtrlIce;
    Out.vWorld = In.vWorldPos;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vLightMap = vMtrlLightMap;
    Out.vFactor = float4(-1.f, 0.f, 0.f, 0.f);

    return Out;
}

Texture2D g_BoreasFluid;

PS_OUT_SCENE_DNWLF PS_BOREAS_BODY_2(PS_IN_POSNORTEX In)
{
    PS_OUT_SCENE_DNWLF Out = (PS_OUT_SCENE_DNWLF) 0;
    
    vector vColor = { 0.0, 0.5568628, 0.93725497f, 0.7058824f };
    vector vMtrlFluid = g_BoreasFluid.Sample(AnisotropicSampler, In.vTexcoord);
    
    Out.vDiffuse = vColor;
    Out.vWorld = In.vWorldPos;
    Out.vNormal = vector(vMtrlFluid.xyz * 0.5f + 0.5f, 0.f);
    Out.vLightMap = vector(0.f, 0.f, 0.f, 0.f);
    Out.vFactor = float4(-1.f, 0.f, 0.f, 0.f);

    return Out;
}

Texture2D g_EvillTexture;
float2 g_vTexutreOffset;

PS_OUT_SCENE_DNWLF PS_DVALIN_BODY(PS_IN_POSNORTEX In)
{   
    PS_OUT_SCENE_DNWLF Out = (PS_OUT_SCENE_DNWLF) 0;

    vector vMtrlDiffuse = g_EvillTexture.Sample(AnisotropicSampler, In.vTexcoord + g_vTexutreOffset);
    vMtrlDiffuse.a = 1.f;
    
    vector vColor = { 17.0f / 255.0f, 28.0f / 255.0f, 99.0f / 255.0f, 1.f };
    vColor.b = vMtrlDiffuse.r;
    
    Out.vDiffuse = vColor;
    Out.vWorld = In.vWorldPos;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

    if (g_bLightMapTexture == true)
    {
        vector vLightMap = g_LightMapTexture.Sample(AnisotropicSampler, In.vTexcoord);
        Out.vLightMap = vLightMap;
    }
    else
    {
        Out.vLightMap = vector(0.f, 0.f, 0.f, 0.f);
    }
    
    Out.vFactor = float4(-1.f, 0.f, 0.f, 0.f);

    return Out;
}

PS_OUT_SCENE_DNWF PS_DUNGEONOBJECT(PS_IN_POSNORTEX In)
{
    PS_OUT_SCENE_DNWF Out = (PS_OUT_SCENE_DNWF) 0;
    
    vector vMtrlDiffuse     = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDiffuse   = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vDiffuse        = vMtrlDiffuse;
    Out.vNormal         = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vWorld          = In.vWorldPos;
    Out.vFactor         = float4(1.f, 0.f, 0.f, 0.f);
    
    return Out;
}

bool g_bVertical;
PS_OUT_DIFFUSE PS_DVALIN_ENTRANCE(PS_IN_POSNORTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    if (g_bVertical == true)
        In.vTexcoord.y += g_fTimeAcc;
    else
        In.vTexcoord.x += g_fTimeAcc;
    
    vector vColor = { 44.f / 255.f, 107.f / 255.f, 186.f / 255.f, 0.3f };
    //vector vColor = { 0.75f, 1.f, 1.f, 0.1f };
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    vColor.a += vMtrlDiffuse.r + vMtrlDiffuse.g;
    vColor.rgb += vMtrlDiffuse.r/3.f + vMtrlDiffuse.g/3.f;
    
    if (vColor.a <0.01f)
        discard;
    
    Out.vDiffuse = vColor;
    
    return Out;
}

PS_OUT_DIFFUSE PS_DVALIN_ENTRANCE_ROT(PS_IN_POSNORTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    vector vColor = { 44.f / 255.f, 107.f / 255.f, 186.f / 255.f, 0.3f };
    //vector vColor = { 0.75f, 1.f, 1.f, 0.1f };
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    vColor.a += vMtrlDiffuse.r + vMtrlDiffuse.g;
    vColor.rgb += vMtrlDiffuse.r / 3.f + vMtrlDiffuse.g / 3.f;
    
    if (vColor.a < 0.01f)
        discard;
    
    Out.vDiffuse = vColor;
    
    return Out;
}

PS_OUT_DIFFUSE PS_EFF_ACTIVATE(PS_IN_POSNORTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    //float2 vPos = { 0.f, g_fTimeAcc };
    //float2 vNoisePos = { 0.f, 0.6f };
    //float2 vScale = { 5.f, 1.f };
    //float2 vNoiseScale = { 1.f, 0.6f };
    
    //vector vWindColor = { 93.f / 255.f, 157.f / 255.f, 255.f / 255.f, 0.3f };
    ////vector vWindColor = { 0.75f, 1.f, 1.f, 0.1f };
    
    

    //vector vNoiseTex = g_Texture.Sample(AnisotropicSampler, vNoisePos + In.vTexcoord * vNoiseScale);
    ////vector vMaskTex = g_Texture2.Sample(AnisotropicSampler, vPos + In.vTexcoord * vScale);
    //vector vDiffTex = g_Texture2.Sample(LinearSampler, vPos + In.vTexcoord * vScale);
    
    //vWindColor.a = pow(vDiffTex.r, 4.f);
   
    //if (vWindColor.a < 0.01f)
    //    discard;
    
    //if (vWindColor.a < vNoiseTex.r)
    //    discard;
    
    // Out.vDiffuse = vWindColor;

    vector vDiffTex = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vColor = { 1.f, 1.f, 1.f, 1.f };
    
    Out.vDiffuse = vColor;
    
    return Out;
}


/* Shadow */
VS_OUT_GEOPOSITION VS_SHADOW(VS_IN_ANIMMESH In)
{
    VS_OUT_GEOPOSITION Out = (VS_OUT_GEOPOSITION) 0;

	float		fW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

	/* 현재 정점에게 적용되어야할 뼈 네개의 상태행렬 합쳐서 하나의 행렬로 만든다. */
	matrix		BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fW;

	/* 뼈의 행렬을 정점의 로컬위치에 곱해서 정점의 로컬위치가 뼈의 위치로 바뀌게끔 처리한다. */
	vector		vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);

	Out.vPosition = mul(vPosition, g_WorldMatrix);

	return Out;
}

struct GS_IN
{
	float4		vPosition		: POSITION;
};

struct GS_OUT
{
	float4		vPosition		: SV_POSITION;
	uint		iRTIndex		: SV_RenderTargetArrayIndex;
};

[maxvertexcount(9)]
void GS_SHADOW(triangle GS_IN In[3], inout TriangleStream<GS_OUT> Triangles)
{
	for (int iFace = 0; iFace < 3; ++iFace)
	{
		GS_OUT Out;
		Out.iRTIndex = iFace;
		for (int iIndex = 0; iIndex < 3; ++iIndex)
		{
			Out.vPosition = mul(In[iIndex].vPosition, g_LightViewProj[iFace]);
			Triangles.Append(Out);
		}
		Triangles.RestartStrip();
	}
}

struct PS_SHADOWIN
{
	float4		vPosition		: SV_POSITION;
};

void PS_SHADOW(PS_SHADOWIN In)
{
}

technique11 DefaultTechnique
{
	pass DefaultPass
	{
        SetRasterizerState(RS_NonCullSolid);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
		PixelShader		= compile ps_5_0 PS_MAIN();
	}

	pass MTrailPass
	{
        SetRasterizerState(RS_NonCullSolid);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
		PixelShader		= compile ps_5_0 PS_MTRAIL();
	}

	pass AddExposure
	{
        SetRasterizerState(RS_NonCullSolid);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
		PixelShader		= compile ps_5_0 PS_ADDEXPOSURE();
	}

	pass Ramp
	{
        SetRasterizerState(RS_NonCullSolid);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
		PixelShader		= compile ps_5_0 PS_RAMP();
	}

    pass Dissolve
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_MAIN_DISSOLVE();
    }

    pass RampDissolve
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_RAMP_DISSOLVE();
    }

	pass PhoenixPass
	{
        SetRasterizerState(RS_NonCullSolid);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
		PixelShader		= compile ps_5_0 PS_PHOENIX();
	}

    pass Petrifacation
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_PETRIFICATION();
    }

    pass Freezing
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FREEZING();
    }

    pass Eff_Meteor_Trail
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFF_METEOR_TRAIL();
    }
    pass Eff_Wind_Field
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFF_WIND_FIELD();
    }

    pass EFFIGY_FIRE_Pass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFFIGY_FIRE();
    }

	pass ShadowPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader	= compile vs_5_0 VS_SHADOW();
		GeometryShader	= compile gs_5_0 GS_SHADOW();
		PixelShader		= compile ps_5_0 PS_SHADOW();
	}

    pass NpcBlackHair
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_NPC_BLACKHAIR();
    }

    pass NpcBrownHair
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_NPC_BROWNHAIR();
    }

    pass NpcLightBrownHair
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_NPC_LIGHTBROWNHAIR();
    }

    pass NpcYellowBrownHair
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_NPC_YELLOWBROWNHAIR();
    }

    pass EFFECT_BOREAS_BODY_1
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BOREAS_BODY_1();
    }

    pass EFFECT_BOREAS_BODY_2
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BOREAS_BODY_2();
    }

    pass EFFECT_DVALIN_BODY
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DVALIN_BODY();
    }

    pass DungeonObjectPass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_DUNGEONOBJECT();
    }

    pass Dvalin_Entrance
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DVALIN_ENTRANCE();
    }
    
    pass Dvalin_Entrance_Rotate
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_ROTATE();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DVALIN_ENTRANCE_ROT();
    }
    pass EFF_Activate
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFF_ACTIVATE();
    }
}
