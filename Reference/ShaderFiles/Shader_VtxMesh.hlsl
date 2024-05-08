#include "Engine_Shader_Defines.hlsli"

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

PS_OUT_SCENE_DNWF PS_MAIN(PS_IN_MESH In)
{
    PS_OUT_SCENE_DNWF Out = (PS_OUT_SCENE_DNWF) 0;

	vector		vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

	if (vMtrlDiffuse.a < 0.1f)
		discard;

	Out.vDiffuse = vMtrlDiffuse;
    Out.vWorld = In.vWorldPos;

	if (g_bNormalTexture == true)
	{
		vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);

		/* DXT5 to DXT1 */
		vNormalDesc.b = 1.f;
		vNormalDesc.r = vNormalDesc.a;

		float3		vNormal = vNormalDesc.xyz * 2.f - 1.f;

		float3x3	WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal.xyz);
		vNormal = mul(normalize(vNormal), WorldMatrix);

		Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
	}
	else
	{
		Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	}
    
    /* SceneFactor (R:IsAdditionalExposure (not -1 is True), G:Outline (1 is False), B:FaceFow, A:FaceRev) */
    Out.vFactor = float4(1.f, 0.f, 0.f, 0.f);
	
    return Out;
}

PS_OUT_SCENE_TERRAIN PS_MAIN_TERRAIN(PS_IN_MESH In)
{
    PS_OUT_SCENE_TERRAIN Out = (PS_OUT_SCENE_TERRAIN) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.1f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;

    Out.vWorld = In.vWorldPos;
    Out.vShadowWorld = In.vWorldPos;

    if (g_bNormalTexture == true)
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);

		/* DXT5 to DXT1 */
        vNormalDesc.b = 1.f;
        vNormalDesc.r = vNormalDesc.a;

        float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

        float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal.xyz);
        vNormal = mul(normalize(vNormal), WorldMatrix);

        Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    }
    else
    {
        Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    }
    
    /* SceneFactor (R:IsAdditionalExposure (not -1 is True), G:Outline (1 is False), B:FaceFow, A:FaceRev) */
    Out.vFactor = float4(1.f, 0.f, 0.f, 0.f);
	
    return Out;
}

float g_fPillarLineLight;
float g_fFadeIn;

PS_OUT_SCENE_DNWF PS_MAIN_PILLAR(PS_IN_MESH In)
{
    PS_OUT_SCENE_DNWF Out = (PS_OUT_SCENE_DNWF) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.1f)
        discard;
    
    if (g_fFadeIn == 0.f)
    {
        if (In.vTexcoord.x > 0.968f)
        {
            if (In.vTexcoord.y >= g_fPillarLineLight - 0.1f && In.vTexcoord.y <= g_fPillarLineLight + 0.1f)
            {
                float fAtt = 1.f - abs(g_fPillarLineLight - In.vTexcoord.y) * 8.f;
                Out.vDiffuse.rgba = vector(1.f * fAtt, 0.5f * fAtt, 0.2f * fAtt, 1.f);
            }
            else
                Out.vDiffuse = vMtrlDiffuse;
        }
        else
            Out.vDiffuse = vMtrlDiffuse;
    }
    else
    {
        vMtrlDiffuse.rgb *= 0.2f;
        Out.vDiffuse.rgba = float4(vMtrlDiffuse.rgb + float3(1.f, 0.5f, 0.f) * g_fFadeIn, 1.f);

    }
	
    Out.vWorld = In.vWorldPos;

    if (g_bNormalTexture == true)
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);

		/* DXT5 to DXT1 */
        vNormalDesc.b = 1.f;
        vNormalDesc.r = vNormalDesc.a;

        float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

        float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal.xyz);
        vNormal = mul(normalize(vNormal), WorldMatrix);

        Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    }
    else
    {
        Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    }
    
    /* SceneFactor (R:IsAdditionalExposure (not -1 is True), G:Outline (1 is False), B:FaceFow, A:FaceRev) */
    Out.vFactor = float4(1.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT_SCENE_DNWF PS_MAIN_WIND_RANGE(PS_IN_MESH In)
{
    PS_OUT_SCENE_DNWF Out = (PS_OUT_SCENE_DNWF) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord * g_vUVScaleRate + g_vUVSpinRate);

    if (vMtrlDiffuse.a < 0.1f)
        discard;

    if (vMtrlDiffuse.b < 0.1f)
        discard;

    //vector vWindColor = { 1.f, 1.f, 1.f, 0.f };
    //vector vWindColor = { 1.f, 0.65882352941f, 0.43921568627f, 0.f };
    //vector vWindColor = { 0.10980392156, 0.73333333333, 0.74117647058, 0.f }; // 청록색 28.f, 187.f, 189.f
    vector vWindColor = g_vColor; // 윈터 귀걸이색 rgb(151, 210, 240)
    
    vWindColor.a = vMtrlDiffuse.r;
    
    Out.vDiffuse = vWindColor;
    
    Out.vWorld = In.vWorldPos;  

    if (g_bNormalTexture == true)
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);

		/* DXT5 to DXT1 */
        vNormalDesc.b = 1.f;
        vNormalDesc.r = vNormalDesc.a;

        float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

        float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal.xyz);
        vNormal = mul(normalize(vNormal), WorldMatrix);

        Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    }
    else
    {
        Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    }
    
    /* SceneFactor (R:IsAdditionalExposure (not -1 is True), G:Outline (1 is False), B:FaceFow, A:FaceRev) */
    Out.vFactor = float4(1.f, 0.f, 0.f, 0.f);
	
    return Out;
}

PS_OUT_SCENE_DNWF PS_MAIN_WIND_BLADE(PS_IN_MESH In)
{
    PS_OUT_SCENE_DNWF Out = (PS_OUT_SCENE_DNWF) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord + g_vUVSpinRate);
    
    vector vWindColor = { 0.10980392156, 0.73333333333, 0.74117647058, 0.f }; // 청록색 28.f, 187.f, 189.f
    
    vMtrlDiffuse += vWindColor;
    
    if(vMtrlDiffuse.r < 0.5f)
        vMtrlDiffuse.a = 0.5f;
        
    Out.vDiffuse = vMtrlDiffuse;
    Out.vWorld = In.vWorldPos;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    
    /* SceneFactor (R:IsAdditionalExposure (not -1 is True), G:Outline (1 is False), B:FaceFow, A:FaceRev) */
    Out.vFactor = float4(1.f, 0.f, 0.f, 0.f);
	
    return Out;
}

PS_OUT_SCENE_DNWF PS_MAIN_SCRATCH(PS_IN_MESH In)
{
    PS_OUT_SCENE_DNWF Out = (PS_OUT_SCENE_DNWF) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord + g_vUVSpinRate);
    
    vector vScratchColor = { 0.10980392156, 0.73333333333, 0.74117647058, 0.f }; // 청록색 28.f, 187.f, 189.f
    
    vMtrlDiffuse += vScratchColor;
    
    if(vMtrlDiffuse.b <= 1.f)
        discard;
   
        
    Out.vDiffuse = vMtrlDiffuse;
    Out.vWorld = In.vWorldPos;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    
    /* SceneFactor (R:IsAdditionalExposure (not -1 is True), G:Outline (1 is False), B:FaceFow, A:FaceRev) */
    Out.vFactor = float4(1.f, 0.f, 0.f, 0.f);
	
    return Out;
}

PS_OUT_SCENE_DNWF PS_MAIN_RUSH(PS_IN_MESH In)
{
    PS_OUT_SCENE_DNWF Out = (PS_OUT_SCENE_DNWF) 0;

    //vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord + g_vUVSpinRate);
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    vector vEffectDiffuse = { 0.f, 0.231f, 0.612f, 0.f }; // rgb(0, 59, 156) 돌진 남색
   
    if(vMtrlDiffuse.g <= 0.2f)
        discard;
    
    vMtrlDiffuse = vEffectDiffuse;
    vMtrlDiffuse.a = 0.8f;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vWorld = In.vWorldPos;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    
    /* SceneFactor (R:IsAdditionalExposure (not -1 is True), G:Outline (1 is False), B:FaceFow, A:FaceRev) */
    Out.vFactor = float4(1.f, 0.f, 0.f, 0.f);
	  
    return Out;
}

PS_OUT_SCENE_DNWF PS_MAIN_RUSH2(PS_IN_MESH In)
{
    PS_OUT_SCENE_DNWF Out = (PS_OUT_SCENE_DNWF) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord + g_vUVSpinRate);
       
    vMtrlDiffuse.a = 0.8f;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vWorld = In.vWorldPos;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    
    /* SceneFactor (R:IsAdditionalExposure (not -1 is True), G:Outline (1 is False), B:FaceFow, A:FaceRev) */
    Out.vFactor = float4(1.f, 0.f, 0.f, 0.f);
	
    return Out;
}

PS_OUT_SCENE_DNWF PS_MAIN_EFF_SP_EXPLODE(PS_IN_MESH In)
{
    PS_OUT_SCENE_DNWF Out = (PS_OUT_SCENE_DNWF) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord );
    

    //float3 rgb = vMtrlDiffuse.rgb;

    //float minComponent = min(min(rgb.r, rgb.g), rgb.b);
    //float maxComponent = max(max(rgb.r, rgb.g), rgb.b);
    //float delta = maxComponent - minComponent;

    //float hue = 0.0;
    //float saturation = (maxComponent > 0.0) ? (delta / maxComponent) : 0.0;
    //float value = maxComponent;

    //if (saturation > 0.0)
    //{
    //    if (rgb.r >= maxComponent)
    //        hue = (rgb.g - rgb.b) / delta;
    //    else if (rgb.g >= maxComponent)
    //        hue = 2.0 + (rgb.b - rgb.r) / delta;
    //    else
    //        hue = 4.0 + (rgb.r - rgb.g) / delta;

    //    hue *= 60.0;

    //    if (hue < 0.0)
    //        hue += 360.0;
    //}

    //float3 hsv = float3(hue, saturation, value);
    
    //hsv.y += g_fTimeAcc;
    
    //if (hsv.y<0.f)
    //    hsv.y = 0.f;
    
    //float C = hsv.y * hsv.z;
    //float X = C * (1.0 - abs(fmod(hsv.x / 60.0, 2.0) - 1.0));
    //float m = hsv.z - C;

    //float3 hsvTorgb = 0.0;

    //if (hsv.x >= 0.0 && hsv.x < 60.0)
    //    hsvTorgb = float3(C, X, 0.0);
    //else if (hsv.x >= 60.0 && hsv.x < 120.0)
    //    hsvTorgb = float3(X, C, 0.0);
    //else if (hsv.x >= 120.0 && hsv.x < 180.0)
    //    hsvTorgb = float3(0.0, C, X);
    //else if (hsv.x >= 180.0 && hsv.x < 240.0)
    //    hsvTorgb = float3(0.0, X, C);
    //else if (hsv.x >= 240.0 && hsv.x < 300.0)
    //    hsvTorgb = float3(X, 0.0, C);
    //else if (hsv.x >= 300.0 && hsv.x < 360.0)
    //    hsvTorgb = float3(C, 0.0, X);

    //rgb = (hsvTorgb + m);

    //vMtrlDiffuse.rgb = rgb;
    
    //Zhongli
    vMtrlDiffuse.r += g_fTimeAcc * 9;
    vMtrlDiffuse.g += g_fTimeAcc * 9;
    vMtrlDiffuse.b += g_fTimeAcc * 9;
    vMtrlDiffuse.a = 0.9f;
    
    ////MonsterShield
    //vector vColor = {12.f/255.f,1.f,1.f,.5f};
    //vMtrlDiffuse = vColor;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vWorld = In.vWorldPos;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    
    /* SceneFactor (R:IsAdditionalExposure (not -1 is True), G:Outline (1 is False), B:FaceFow, A:FaceRev) */
    Out.vFactor = float4(1.f, 0.f, 0.f, 0.f);
	
    return Out;
}

PS_OUT_SCENE_DNWF PS_MAIN_EFF_Slash(PS_IN_MESH In)
{
    PS_OUT_SCENE_DNWF Out = (PS_OUT_SCENE_DNWF) 0;

    float2 vScale = { 110.f/268.f, 1.2f };
    float2 vPos = { 0.f, -0.5f };
    
    if (In.vTexcoord.x > 0.4f)
        discard;
    
    //if (In.vTexcoord.x >0.5f)
    //{
    //    In.vTexcoord.x += 0.5f;
    //}
    
    //1검기 {0.f,0.f} {103.f/512.f,1.f}
    //2검기 {125.f,64.f }{80.f,960.f/1024.f}
    //3검기 {258.f,134.f}{50.f,890.f}
    //4검기 {387.f,178.f}{23.f,817.f}
    //5검기 {506.f,223.f}{6.f,770.f}
    
    //if (In.vTexcoord.x > 103.f / 512.f)
    //    discard;
    
    //if (In.vTexcoord.x < 0.98828125)
    //      discard;
    //if (In.vTexcoord.y < 223.f / 1024.f)
    //    discard;
    
    //if (In.vTexcoord.x >506.f/512.f && In.vTexcoord.y < 223.f/1024.f && In.vTexcoord.y > 990.f/1024.f)
    //    discard;
        vPos.y += g_fTimeAcc * 5.f;
        //if (In.vTexcoord.x < (g_fTimeAcc - 0.9f))
        //    discard;
        
    //if (In.vTexcoord.y < 1.f - g_fTimeAcc)
    //    discard;
    
    //if ((1.f - g_fTimeAcc) < 0.f && In.vTexcoord.y < (-g_fTimeAcc))
    //    discard;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, vPos + In.vTexcoord * vScale);
    
    vector vSideColor = { 1.f, 1.f, 0.95f, 1.f };
    vector vColor = { 1.f, 1.f, 0.8558f, 0.f };

    
    
    float fGrayScale = vMtrlDiffuse.r;
    if (fGrayScale < 0.05f)
        discard;
    
    vMtrlDiffuse.rgb = vColor.rgb;
    vMtrlDiffuse.a = fGrayScale - (g_fTimeAcc*3.f);
    
    //if (In.vTexcoord.x > 0.39f)
    //    vMtrlDiffuse = vSideColor;
    vMtrlDiffuse.a += (In.vTexcoord.x - 0.2f + (0.5-In.vTexcoord.y));
    vMtrlDiffuse.r += (In.vTexcoord.x - 0.2f);
    vMtrlDiffuse.g += (In.vTexcoord.x - 0.2f);
    vMtrlDiffuse.b += (In.vTexcoord.x - 0.2f);
    
    if (vMtrlDiffuse.a <0.05f)
        discard;
    
        Out.vDiffuse = vMtrlDiffuse;
    Out.vWorld = In.vWorldPos;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    
    /* SceneFactor (R:IsAdditionalExposure (not -1 is True), G:Outline (1 is False), B:FaceFow, A:FaceRev) */
    Out.vFactor = float4(1.f, 1.f, 0.f, 0.f);
	
    return Out;
}

PS_OUT_SCENE_DNWF PS_BURST_BACKGROUND(PS_IN_MESH In)
{
    PS_OUT_SCENE_DNWF Out = (PS_OUT_SCENE_DNWF) 0;

    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord + g_vUVSpinRate);
    
    if (vMtrlDiffuse.a < 0.1f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vWorld = In.vWorldPos;

    if (g_bNormalTexture == true)
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);

		/* DXT5 to DXT1 */
        vNormalDesc.b = 1.f;
        vNormalDesc.r = vNormalDesc.a;

        float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

        float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal.xyz);
        vNormal = mul(normalize(vNormal), WorldMatrix);

        Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    }
    else
    {
        Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    }
    
    /* SceneFactor (R:IsAdditionalExposure (not -1 is True), G:Outline (1 is False), B:FaceFow, A:FaceRev) */
    Out.vFactor = float4(1.f, 0.f, 0.f, 0.f);
	
    return Out;
}


PS_OUT_DIFFUSE PS_MAIN_EFF_ELEM_SHIELD(PS_IN_MESH In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;

    float2 vScale = { 3.f, 3.f };
    
    //vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vCrackDiff = g_MaskTexture.Sample(LinearSampler, In.vTexcoord * vScale);
   
    
    //Water
    //vector vColor = {12.f/255.f,1.f,1.f,.4f};
    //vColor.a += vMtrlDiffuse.g;
    //vMtrlDiffuse = vColor;
    
    //if (g_fTimeAcc > 0.f)
    //{
    //    vCrackDiff.g -= (1 - g_fTimeAcc / 3.f);
    //    if (vCrackDiff.g > 0.f)
    //        vMtrlDiffuse += (vCrackDiff.g + 0.5f);
    //}
    
    Out.vDiffuse = g_vColor;
    float3 vLook = normalize(g_WorldMatrix._41_42_43_44 - g_vCamPosition);
    float3 vUp = float4(0.f, 1.f, 0.f, 0.f);
    float3 vRight = normalize(cross(vUp, vLook));
    
    Out.vDiffuse.a = (1.f - max(dot(vLook * -1.f, In.vNormal.xyz), 0.f)) * abs(dot(vRight, In.vNormal.xyz));
    if(Out.vDiffuse.a <= 0.01f)
        Out.vDiffuse.a = 0.01f;

    if (g_fTimeAcc > 0.f)
    {
        vCrackDiff.g -= (1 - g_fTimeAcc / 3.f);
        if (vCrackDiff.g > 0.f)
        {
            Out.vDiffuse += (vCrackDiff.g + 0.5f);
            
        }
    }
    
    return Out;
}

PS_OUT_SCENE_DNWF PS_ZHONGLI_SHIELD(PS_IN_MESH In)
{
    PS_OUT_SCENE_DNWF Out = (PS_OUT_SCENE_DNWF) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

        
    vMtrlDiffuse.r -= 0.3f;
    vMtrlDiffuse.a -= 0.6f;
    
    if (vMtrlDiffuse.a < 0.1f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vWorld = In.vWorldPos;

    if (g_bNormalTexture == true)
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);

		/* DXT5 to DXT1 */
        vNormalDesc.b = 1.f;
        vNormalDesc.r = vNormalDesc.a;

        float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

        float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal.xyz);
        vNormal = mul(normalize(vNormal), WorldMatrix);

        Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    }
    else
    {
        Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    }
    
    /* SceneFactor (R:IsAdditionalExposure (not -1 is True), G:Outline (1 is False), B:FaceFow, A:FaceRev) */
    Out.vFactor = float4(1.f, 0.f, 0.f, 0.f);
	
    return Out;
}

PS_OUT_SCENE_DNWF PS_BURST_RAY(PS_IN_MESH In)
{
    PS_OUT_SCENE_DNWF Out = (PS_OUT_SCENE_DNWF) 0;

    float2 vScale = { 1.f, 1.f };
    float2 vPos = { 0.f, 0.9f };
    
    vPos.y  += g_vUVSpinRate.x * 10.f;
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, vPos + In.vTexcoord * vScale);
    
    vScale.y = 0.4f;
    vPos.y = 0.2f;
    
    float sampleY = vPos.y + In.vTexcoord.y * vScale.y;
    // 샘플링 좌표가 [0, 1] 범위를 벗어나면 조정
    sampleY = saturate(sampleY);
    // 상하 반전
    sampleY = 1.0f - sampleY;

    // 상하 반전된 샘플링 좌표로 텍스처 샘플링
    vector vMaskDiffuse = g_Texture.Sample(LinearSampler, float2(In.vTexcoord.x, sampleY));
    

    //vScale.y = 1.f;
    //vPos.y = 0.2f;
   

    float fGrayScale = (vMtrlDiffuse.r+ vMtrlDiffuse.g + vMtrlDiffuse.b);
    //if (fGrayScale < 0.1f)
    //        discard;
            
     vMtrlDiffuse= vMaskDiffuse;
    
    if (vMtrlDiffuse.r > 0.9f)
    {
        vector vColor = { 0.4f, 0.4f, 0.4f, 0.f };
        vMtrlDiffuse += vColor;
    }
    
        vMtrlDiffuse.a = vMtrlDiffuse.g*2.f;
   

        Out.vDiffuse = vMtrlDiffuse;
        Out.vWorld = In.vWorldPos;

        if (g_bNormalTexture == true)
        {
            vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);

		/* DXT5 to DXT1 */
            vNormalDesc.b = 1.f;
            vNormalDesc.r = vNormalDesc.a;
        
            float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

            float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal.xyz);
            vNormal = mul(normalize(vNormal), WorldMatrix);

            Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
        }
        else
        {
            Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
        }
    
    /* SceneFactor (R:IsAdditionalExposure (not -1 is True), G:Outline (1 is False), B:FaceFow, A:FaceRev) */
        Out.vFactor = float4(1.f, 0.f, 0.f, 0.f);
	
        return Out;
    }

PS_OUT_SCENE_DC PS_MAIN_EFF_SlashDiluc(PS_IN_MESH In)
{
    PS_OUT_SCENE_DC Out = (PS_OUT_SCENE_DC) 0;

    float2 vScale = { 118.f / 268.f, 2.f };
    float2 vPos = { 0.f, -1.f };
    
    //float2 vScale = { 0.5f, 1.f };
    //float2 vPos = { 70.f / 256.f, -0.6f };
    
    
    //if (In.vTexcoord.x >0.5f)
    //{
    //    In.vTexcoord.x += 0.5f;
    //}
    
    //1검기 {0.f,0.f} {103.f/512.f,1.f}
    //2검기 {125.f,64.f }{80.f,960.f/1024.f}
    //3검기 {258.f,134.f}{50.f,890.f}
    //4검기 {387.f,178.f}{23.f,817.f}
    //5검기 {506.f,223.f}{6.f,770.f}
    
    //if (In.vTexcoord.x > 103.f / 512.f)
    //    discard;
    
    //if (In.vTexcoord.x < 0.98828125)
    //      discard;
    //if (In.vTexcoord.y < 223.f / 1024.f)
    //    discard;
    
    //if (In.vTexcoord.x >506.f/512.f && In.vTexcoord.y < 223.f/1024.f && In.vTexcoord.y > 990.f/1024.f)
    //    discard;
    
    if ((vPos.y + g_fTimeAcc * 5.f)  < 0.f)
    {
        vPos.y += g_fTimeAcc * 5.f;
    }
    else
    {
        vPos.y = 0.f;
    }
    
        //if (In.vTexcoord.x < (g_fTimeAcc - 0.9f))
        //    discard;
        
    //if (In.vTexcoord.y < 1.f - g_fTimeAcc)
    //    discard;
    
    //if ((1.f - g_fTimeAcc) < 0.f && In.vTexcoord.y < (-g_fTimeAcc))
    //    discard;
    
    vector vDistTex = g_Texture.Sample(LinearSampler, In.vTexcoord);
  
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, vPos + In.vTexcoord * vScale);
    
    vector vSideColor = { 0.3f, 0.3f, 0.45f, 1.f };
    vector vColor = g_vColor;
    
    
    if (vMtrlDiffuse.r < 0.05f)
        discard;
    
    
    
    vMtrlDiffuse.rgb = vColor.rgb;
    //vMtrlDiffuse.a = fGrayScale +0.2f -(g_fTimeAcc * 3.f);
    vMtrlDiffuse.a = vMtrlDiffuse.r + vDistTex.r - (g_fTimeAcc * 5.f);
    
    //if (In.vTexcoord.x > 0.50f)
    //    vMtrlDiffuse += vSideColor;
    
    if (In.vTexcoord.x > 0.46f)
        vMtrlDiffuse += vSideColor;
    
    vMtrlDiffuse.a += (In.vTexcoord.x + (0.5 - In.vTexcoord.y));
    vMtrlDiffuse.r += (In.vTexcoord.x );
    vMtrlDiffuse.g += (In.vTexcoord.x );
    vMtrlDiffuse.b += (In.vTexcoord.x );
    
    
    float fGrayScale = (vMtrlDiffuse.r + vMtrlDiffuse.g + vMtrlDiffuse.b)/3.f;
    if (fGrayScale < 0.1f)
        discard;
    if (vMtrlDiffuse.a < 0.01f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vCShader = float4(5.f, 0.f, 0.f, 0.f);

    return Out;
}

PS_OUT_SCENE_DNWF PS_MAIN_EFF_Andrius_Claw(PS_IN_MESH In)
{
    PS_OUT_SCENE_DNWF Out = (PS_OUT_SCENE_DNWF) 0;

    float2 vScale = { 2.8f, 1.f };
    float2 vPos = { 0.f, -1.f };
    
    //float2 vScale = { 0.5f, 1.f };
    //float2 vPos = { 70.f / 256.f, -0.6f };

        
        if ((vPos.y + g_fTimeAcc*3.f) < 0.f)
        {
            vPos.y += g_fTimeAcc*3.f;
        }
        else
        {
            vPos.y = 0.f;
        }

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, vPos + In.vTexcoord * vScale);
   
    vMtrlDiffuse.a = vMtrlDiffuse.g - (g_fTimeAcc);
    
    if (vMtrlDiffuse.a <0.01f)
        discard;
    
    //vMtrlDiffuse.g -= vMtrlDiffuse.r;
    //vMtrlDiffuse.b -= vMtrlDiffuse.r;
    //vMtrlDiffuse.r = 0.f;
    
        Out.vDiffuse = vMtrlDiffuse;
    Out.vWorld = In.vWorldPos;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    
    /* SceneFactor (R:IsAdditionalExposure (not -1 is True), G:Outline (1 is False), B:FaceFow, A:FaceRev) */
    Out.vFactor = float4(1.f, 0.f, 0.f, 0.f);
	
    return Out;
}

PS_OUT_BWOIT PS_MAIN_Diluc_Flame_Slash(PS_IN_MESH In)
{
    PS_OUT_BWOIT Out = (PS_OUT_BWOIT) 0;

    float2 vScale = { 0.9f, 2.f };
    float2 vPos = { 0.f, -0.5f };
    
    //float2 vScale = { 0.5f, 1.f };
    //float2 vPos = { 70.f / 256.f, -0.6f };
    
    
    //if (In.vTexcoord.x >0.5f)
    //{
    //    In.vTexcoord.x += 0.5f;
    //}
    
    //1검기 {0.f,0.f} {103.f/512.f,1.f}
    //2검기 {125.f,64.f }{80.f,960.f/1024.f}
    //3검기 {258.f,134.f}{50.f,890.f}
    //4검기 {387.f,178.f}{23.f,817.f}
    //5검기 {506.f,223.f}{6.f,770.f}
    
    //if (In.vTexcoord.x > 103.f / 512.f)
    //    discard;
    
    //if (In.vTexcoord.x < 0.98828125)
    //      discard;
    //if (In.vTexcoord.y < 223.f / 1024.f)
    //    discard;
    
    //if (In.vTexcoord.x >506.f/512.f && In.vTexcoord.y < 223.f/1024.f && In.vTexcoord.y > 990.f/1024.f)
    //    discard;
    
    if ((vPos.y + g_fTimeAcc*2.f) < 0.f)
    {
        vPos.y += g_fTimeAcc*2.f;
    }
    else
    {
        vPos.y = 0.f;
    }
    
        //if (In.vTexcoord.x < (g_fTimeAcc - 0.9f))
        //    discard;
        
    //if (In.vTexcoord.y < 1.f - g_fTimeAcc)
    //    discard;
    
    //if ((1.f - g_fTimeAcc) < 0.f && In.vTexcoord.y < (-g_fTimeAcc))
    //    discard;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, vPos + In.vTexcoord * vScale);   
    
    In.vTexcoord.x = 0.95f - In.vTexcoord.x * 2.f;
    vector vTrailDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);
   
    
    float fMtrlR = vMtrlDiffuse.r;
    if (fMtrlR < 0.01f)
        discard;
    
    vMtrlDiffuse.rgb = vTrailDiffuse.rgb;
    //vMtrlDiffuse.a = fGrayScale +0.2f -(g_fTimeAcc * 3.f);
    vMtrlDiffuse.a = fMtrlR + 0.4f - (g_fTimeAcc * 3.f);
    
    //if (In.vTexcoord.x > 0.50f)
    //    vMtrlDiffuse += vSideColor;
    
    //if (In.vTexcoord.x > 0.44f)
    //    vMtrlDiffuse += vSideColor;
    
    //vMtrlDiffuse.a += (In.vTexcoord.x - 0.2f + (0.5 - In.vTexcoord.y));
    //vMtrlDiffuse.r += (In.vTexcoord.x - 0.2f);
    //vMtrlDiffuse.g += (In.vTexcoord.x - 0.2f);
    //vMtrlDiffuse.b += (In.vTexcoord.x - 0.2f);
        
    float fGrayScale = (vMtrlDiffuse.r + vMtrlDiffuse.g + vMtrlDiffuse.b) / 3.f;
    if (fGrayScale < 0.2f)
        discard;
    if (vMtrlDiffuse.a <0.01f)
        discard;

    vector vColor = vMtrlDiffuse;
    
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

PS_OUT_BWOIT PS_MAIN_Diluc_Outer_Flame_Slash(PS_IN_MESH In)
{
    PS_OUT_BWOIT Out = (PS_OUT_BWOIT) 0;

    float2 vScale = { 118.f / 268.f, 2.f };
    float2 vPos = { 0.f, -1.f };
    
    //float2 vScale = { 0.5f, 1.f };
    //float2 vPos = { 70.f / 256.f, -0.6f };
    
    float fDelayTime = g_fTimeAcc - 0.1f;
    if (fDelayTime <0.f)
        discard;
    
    //if (In.vTexcoord.x >0.5f)
    //{
    //    In.vTexcoord.x += 0.5f;
    //}
    
    //1검기 {0.f,0.f} {103.f/512.f,1.f}
    //2검기 {125.f,64.f }{80.f,960.f/1024.f}
    //3검기 {258.f,134.f}{50.f,890.f}
    //4검기 {387.f,178.f}{23.f,817.f}
    //5검기 {506.f,223.f}{6.f,770.f}
    
    //if (In.vTexcoord.x > 103.f / 512.f)
    //    discard;
    
    //if (In.vTexcoord.x < 0.98828125)
    //      discard;
    //if (In.vTexcoord.y < 223.f / 1024.f)
    //    discard;
    
    //if (In.vTexcoord.x >506.f/512.f && In.vTexcoord.y < 223.f/1024.f && In.vTexcoord.y > 990.f/1024.f)
    //    discard;
    
    if  ((vPos.y + fDelayTime * 5.f) < 0.f)
        {
        vPos.y += fDelayTime * 5.f;
         }
        else
        {
            vPos.y = 0.f;
        }
    
        //if (In.vTexcoord.x < (g_fTimeAcc - 0.9f))
        //    discard;
        
    //if (In.vTexcoord.y < 1.f - g_fTimeAcc)
    //    discard;
    
    //if ((1.f - g_fTimeAcc) < 0.f && In.vTexcoord.y < (-g_fTimeAcc))
    //    discard;
   
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, vPos + In.vTexcoord * vScale);
    
    
    In.vTexcoord.x = clamp(In.vTexcoord.x - fDelayTime*5.f, 0.2f, 0.9f);
    
    vector vTrailDiffuse = g_Texture.Sample(LinearClampSampler, In.vTexcoord);
    
    float fMtrlR = vMtrlDiffuse.r;
    if (fMtrlR < 0.01f)
        discard;
    
    
    vMtrlDiffuse.rgb = vTrailDiffuse.rgb;
    //vMtrlDiffuse.a = fGrayScale +0.2f -(g_fTimeAcc * 3.f);
    vMtrlDiffuse.a = fMtrlR + 0.4f - (fDelayTime * 2.f);
    
    //if (In.vTexcoord.x > 0.50f)
    //    vMtrlDiffuse += vSideColor;
    
    //vMtrlDiffuse.a += (In.vTexcoord.x - 0.2f + (0.5 - In.vTexcoord.y));
    //vMtrlDiffuse.r += (In.vTexcoord.x - 0.2f);
    //vMtrlDiffuse.g += (In.vTexcoord.x - 0.2f);
    //vMtrlDiffuse.b += (In.vTexcoord.x - 0.2f);
    
    float fGrayScale = (vMtrlDiffuse.r + vMtrlDiffuse.g + vMtrlDiffuse.b) / 3.f;
    //if (fGrayScale < 0.2f)
    //    discard;
    if (vMtrlDiffuse.a < 0.01f)
        discard;
    
    vector vColor = vMtrlDiffuse;
    
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

//PS_OUT_SCENE_DNWF PS_MAIN_Diluc_Flame_Slash(PS_IN_MESH In)
//{
//    PS_OUT_SCENE_DNWF Out = (PS_OUT_SCENE_DNWF) 0;

//    float2 vScale = { 0.9f, 2.f };
//    float2 vPos = { 0.f, -0.5f };
    
//    //float2 vScale = { 0.5f, 1.f };
//    //float2 vPos = { 70.f / 256.f, -0.6f };
    
    
//    //if (In.vTexcoord.x >0.5f)
//    //{
//    //    In.vTexcoord.x += 0.5f;
//    //}
    
//    //1검기 {0.f,0.f} {103.f/512.f,1.f}
//    //2검기 {125.f,64.f }{80.f,960.f/1024.f}
//    //3검기 {258.f,134.f}{50.f,890.f}
//    //4검기 {387.f,178.f}{23.f,817.f}
//    //5검기 {506.f,223.f}{6.f,770.f}
    
//    //if (In.vTexcoord.x > 103.f / 512.f)
//    //    discard;
    
//    //if (In.vTexcoord.x < 0.98828125)
//    //      discard;
//    //if (In.vTexcoord.y < 223.f / 1024.f)
//    //    discard;
    
//    //if (In.vTexcoord.x >506.f/512.f && In.vTexcoord.y < 223.f/1024.f && In.vTexcoord.y > 990.f/1024.f)
//    //    discard;
    
//    if ((vPos.y + g_fTimeAcc * 2.f) < 0.f)
//    {
//        vPos.y += g_fTimeAcc * 2.f;
//    }
//    else
//    {
//        vPos.y = 0.f;
//    }
    
//        //if (In.vTexcoord.x < (g_fTimeAcc - 0.9f))
//        //    discard;
        
//    //if (In.vTexcoord.y < 1.f - g_fTimeAcc)
//    //    discard;
    
//    //if ((1.f - g_fTimeAcc) < 0.f && In.vTexcoord.y < (-g_fTimeAcc))
//    //    discard;
    
//    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, vPos + In.vTexcoord * vScale);
    
//    In.vTexcoord.x = 0.95f - In.vTexcoord.x * 2.f;
//    vector vTrailDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);
   
    
//    float fMtrlR = vMtrlDiffuse.r;
//    if (fMtrlR < 0.01f)
//        discard;
    
//    vMtrlDiffuse.rgb = vTrailDiffuse.rgb;
//    //vMtrlDiffuse.a = fGrayScale +0.2f -(g_fTimeAcc * 3.f);
//    vMtrlDiffuse.a = fMtrlR + 0.4f - (g_fTimeAcc * 3.f);
    
//    //if (In.vTexcoord.x > 0.50f)
//    //    vMtrlDiffuse += vSideColor;
    
//    //if (In.vTexcoord.x > 0.44f)
//    //    vMtrlDiffuse += vSideColor;
    
//    //vMtrlDiffuse.a += (In.vTexcoord.x - 0.2f + (0.5 - In.vTexcoord.y));
//    //vMtrlDiffuse.r += (In.vTexcoord.x - 0.2f);
//    //vMtrlDiffuse.g += (In.vTexcoord.x - 0.2f);
//    //vMtrlDiffuse.b += (In.vTexcoord.x - 0.2f);
        
//    float fGrayScale = (vMtrlDiffuse.r + vMtrlDiffuse.g + vMtrlDiffuse.b) / 3.f;
//    if (fGrayScale < 0.2f)
//        discard;
//    if (vMtrlDiffuse.a < 0.01f)
//        discard;
    
//    Out.vDiffuse = vMtrlDiffuse;
//    Out.vWorld = In.vWorldPos;
//    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    
//    /* SceneFactor (R:IsAdditionalExposure, G:IsFaceMesh) */
//    Out.vFactor = float4(1.f, 0.f, 1.f, 0.f);
	
//    return Out;
//}

//PS_OUT_SCENE_DNWF PS_MAIN_Diluc_Outer_Flame_Slash(PS_IN_MESH In)
//{
//    PS_OUT_SCENE_DNWF Out = (PS_OUT_SCENE_DNWF) 0;

//    float2 vScale = { 118.f / 268.f, 2.f };
//    float2 vPos = { 0.f, -1.f };
    
//    //float2 vScale = { 0.5f, 1.f };
//    //float2 vPos = { 70.f / 256.f, -0.6f };
    
//    float fDelayTime = g_fTimeAcc - 0.1f;
//    if (fDelayTime < 0.f)
//        discard;
    
//    //if (In.vTexcoord.x >0.5f)
//    //{
//    //    In.vTexcoord.x += 0.5f;
//    //}
    
//    //1검기 {0.f,0.f} {103.f/512.f,1.f}
//    //2검기 {125.f,64.f }{80.f,960.f/1024.f}
//    //3검기 {258.f,134.f}{50.f,890.f}
//    //4검기 {387.f,178.f}{23.f,817.f}
//    //5검기 {506.f,223.f}{6.f,770.f}
    
//    //if (In.vTexcoord.x > 103.f / 512.f)
//    //    discard;
    
//    //if (In.vTexcoord.x < 0.98828125)
//    //      discard;
//    //if (In.vTexcoord.y < 223.f / 1024.f)
//    //    discard;
    
//    //if (In.vTexcoord.x >506.f/512.f && In.vTexcoord.y < 223.f/1024.f && In.vTexcoord.y > 990.f/1024.f)
//    //    discard;
    
//    if ((vPos.y + fDelayTime * 5.f) < 0.f)
//    {
//        vPos.y += fDelayTime * 5.f;
//    }
//    else
//    {
//        vPos.y = 0.f;
//    }
    
//        //if (In.vTexcoord.x < (g_fTimeAcc - 0.9f))
//        //    discard;
        
//    //if (In.vTexcoord.y < 1.f - g_fTimeAcc)
//    //    discard;
    
//    //if ((1.f - g_fTimeAcc) < 0.f && In.vTexcoord.y < (-g_fTimeAcc))
//    //    discard;
   
    
//    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, vPos + In.vTexcoord * vScale);
    
    
//    In.vTexcoord.x = clamp(In.vTexcoord.x - fDelayTime * 5.f, 0.2f, 0.9f);
    
//    vector vTrailDiffuse = g_Texture.Sample(LinearClampSampler, In.vTexcoord);
    
//    float fMtrlR = vMtrlDiffuse.r;
//    if (fMtrlR < 0.01f)
//        discard;
    
    
//    vMtrlDiffuse.rgb = vTrailDiffuse.rgb;
//    //vMtrlDiffuse.a = fGrayScale +0.2f -(g_fTimeAcc * 3.f);
//    vMtrlDiffuse.a = fMtrlR + 0.4f - (fDelayTime * 2.f);
    
//    //if (In.vTexcoord.x > 0.50f)
//    //    vMtrlDiffuse += vSideColor;
    
//    //vMtrlDiffuse.a += (In.vTexcoord.x - 0.2f + (0.5 - In.vTexcoord.y));
//    //vMtrlDiffuse.r += (In.vTexcoord.x - 0.2f);
//    //vMtrlDiffuse.g += (In.vTexcoord.x - 0.2f);
//    //vMtrlDiffuse.b += (In.vTexcoord.x - 0.2f);
    
//    float fGrayScale = (vMtrlDiffuse.r + vMtrlDiffuse.g + vMtrlDiffuse.b) / 3.f;
//    //if (fGrayScale < 0.2f)
//    //    discard;
//    if (vMtrlDiffuse.a < 0.01f)
//        discard;
    
    
//    Out.vDiffuse = vMtrlDiffuse;
//    Out.vWorld = In.vWorldPos;
//    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    
//    /* SceneFactor (R:IsAdditionalExposure, G:IsFaceMesh) */
//    Out.vFactor = float4(1.f, 0.f, 1.f, 0.f);
	
//    return Out;
//}


PS_OUT_SCENE_DWF PS_MAIN_Flame_Enchant_Cross(PS_IN_MESH In)
{
    PS_OUT_SCENE_DWF Out = (PS_OUT_SCENE_DWF) 0;

    float2 vScale = { 1.f, 1.f };
    float2 vPos = { 0.f, 0.0f };
    
    vPos.y -= g_fTimeAcc / 2.f;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, In.vTexcoord);
    
    vector vNoiseTex = g_Texture.Sample(LinearSampler, vPos + In.vTexcoord * vScale);
    vector vNoiseTex2 = g_Texture.Sample(LinearSampler, vPos + In.vTexcoord * vScale * 2.f);
    
    vNoiseTex = (vNoiseTex - 0.5f) * 2.f;
    vNoiseTex2 = (vNoiseTex2 - 0.5f) * 2.f;

    vector vNoiseTex3 = vNoiseTex + vNoiseTex2;
    
    vNoiseTex3.a = vMtrlDiffuse.b * 3.f;
    
    float fGrayScale = (vMtrlDiffuse.r + vMtrlDiffuse.g + vMtrlDiffuse.b) / 3.f;
    
    if (fGrayScale < 0.1f)
        discard;
    
    vMtrlDiffuse.a = fGrayScale;
    
    if (fGrayScale < 0.3f)
    vMtrlDiffuse.r = vNoiseTex3.r + 0.3f;
    vMtrlDiffuse.g = vNoiseTex3.g;
    
    if (vMtrlDiffuse.r < vMtrlDiffuse.g)
        vMtrlDiffuse.r = vMtrlDiffuse.g;

    vMtrlDiffuse.b = 0.f;
    
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vWorld = In.vWorldPos;

    
    /* SceneFactor (R:IsAdditionalExposure (not -1 is True), G:Outline (1 is False), B:FaceFow, A:FaceRev) */
    Out.vFactor = float4(1.f, 1.f, 0.f, 0.f);
	
    return Out;
}

PS_OUT_SCENE_DC PS_MAIN_Trail_Zhongli_Ring(PS_IN_MESH In)
{
    PS_OUT_SCENE_DC Out = (PS_OUT_SCENE_DC) 0;

    float2 vScale = { 123.f / 268.f, 1.f };
    float2 vPos = { 0.f, 0.f };
    
    
    vPos.y += g_fTimeAcc;
    float fFrac = frac(vPos.y + In.vTexcoord.y * vScale.y);
    if (fFrac >0.5f)
        discard;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, float2(vPos.x + In.vTexcoord.x * vScale.x, fFrac*2.f));
    
    float fGrayScale = (vMtrlDiffuse.r + vMtrlDiffuse.g + vMtrlDiffuse.b) / 3.f;
    
    if (fGrayScale < 0.1f)
        discard;
    
    vMtrlDiffuse.a = fGrayScale;
    
    vector vSideColor = { 1.f, 1.f, 0.95f, 1.f };
    vector vColor = { 1.f, 1.f, 0.8058f, 0.f };
    
    vMtrlDiffuse.rgb = vColor.rgb;
    vMtrlDiffuse.a = fGrayScale;
    
    if (In.vTexcoord.x > 0.55f)
        vMtrlDiffuse = vSideColor;
    //vMtrlDiffuse.a += (In.vTexcoord.x - 0.3f + (0.5 - In.vTexcoord.y));
    //vMtrlDiffuse.r += (In.vTexcoord.x - 0.3f);
    //vMtrlDiffuse.g += (In.vTexcoord.x - 0.3f);
    //vMtrlDiffuse.b += (In.vTexcoord.x - 0.3f);
    
    
    if (vMtrlDiffuse.a < 0.1f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vCShader = float4(5.f, 0.f, 0.f, 0.f);

    return Out;
}

Texture2D g_DissolveTexture;
float g_fDissolve;
float g_fDissolveSize;
float3 g_vDissolveColor;
float3 g_vAddColor;

PS_OUT_SCENE_DNWF PS_MAIN_Dissolve(PS_IN_MESH In)
{
    PS_OUT_SCENE_DNWF Out = (PS_OUT_SCENE_DNWF) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vDissolve = g_DissolveTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.1f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;
    Out.vDiffuse.rgb += g_vAddColor;
    Out.vWorld = In.vWorldPos;

    if (g_bNormalTexture == true)
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);

		/* DXT5 to DXT1 */
        vNormalDesc.b = 1.f;
        vNormalDesc.r = vNormalDesc.a;

        float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

        float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal.xyz);
        vNormal = mul(normalize(vNormal), WorldMatrix);

        Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    }
    else
    {
        Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    }
    
    if (vDissolve.r < g_fDissolve)
        discard;

    float fLength = g_fDissolve + g_fDissolveSize;
    if (vDissolve.r <= fLength)
    {
        float fAtt = sin(radians(90.f * (fLength - vDissolve.r) * (1 / g_fDissolveSize)));
        Out.vDiffuse.rgb += g_vDissolveColor * fAtt;
    }
    
    /* SceneFactor (R:IsAdditionalExposure (not -1 is True), G:Outline (1 is False), B:FaceFow, A:FaceRev) */
    Out.vFactor = float4(1.f, 0.f, 0.f, 0.f);
	
    return Out;
}

PS_OUT_SCENE_DC PS_Eff_Barbara_Star(PS_IN_MESH In)
{
    PS_OUT_SCENE_DC Out = (PS_OUT_SCENE_DC) 0;

    float2 vScale = { 1.f, 1.f };
    float2 vPos = { 0.f, 0.f };
    
    vPos.x = g_ColorRate;
    vPos.y += g_fTimeAcc;
    
    if (In.vTexcoord.y<1- g_fTimeAcc)
        discard;
    //vPos.y += g_fTimeAcc;

    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, In.vTexcoord * vScale);
    
    vector vRampTexture = g_Texture.Sample(LinearSampler, float2(g_fTimeAcc/20.f+vPos.x,0.05f));

    vRampTexture.rgb *= 2.f;
    vRampTexture.a = vMtrlDiffuse.r + vMtrlDiffuse.g;
    //+vMtrlDiffuse.b * 3.f;
  
    
    if (vRampTexture.a < 0.1f)
        discard;
    
    Out.vDiffuse = vRampTexture;
    Out.vCShader = float4(5.f, 0.f, 0.f, 0.f);

    return Out;
}

PS_OUT_SCENE_DWF PS_GLIDING_RING(PS_IN_MESH In)
{
    PS_OUT_SCENE_DWF Out = (PS_OUT_SCENE_DWF) 0;
    
    Out.vDiffuse = float4(1.f, 1.f, 0.7f, 1.f);
    Out.vWorld = In.vWorldPos;

    /* SceneFactor (R:IsAdditionalExposure (not -1 is True), G:Outline (1 is False), B:FaceFow, A:FaceRev) */
    Out.vFactor = float4(1.f, 0.f, 0.f, 0.f);
	
    return Out;
}

PS_OUT_SCENE_DWF PS_EFFIGY_FIRE_CUBE(PS_IN_MESH In)
{
    PS_OUT_SCENE_DWF Out = (PS_OUT_SCENE_DWF) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.1f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;
    //Out.vDiffuse.rgb = vMtrlDiffuse.rgb * float3(1.5686f, 1.1359f, 1.f);
    Out.vDiffuse.rgb = vMtrlDiffuse.rgb * float3(1.5686f, 0.9f, 1.f);
    Out.vDiffuse.rgb = pow(Out.vDiffuse.rgb * 2.f, 1.5f);
    Out.vDiffuse.rgb = saturate(Out.vDiffuse.rgb);
    
    Out.vWorld = In.vWorldPos;
	
    //Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    
    /* SceneFactor (R:IsAdditionalExposure (not -1 is True), G:Outline (1 is False), B:FaceFow, A:FaceRev) */
    Out.vFactor = float4(-1.f, 0.f, 0.f, 0.f);

    return Out;
}

PS_OUT_SCENE_DWF PS_ACCELERATING_RING(PS_IN_MESH In)
{
    PS_OUT_SCENE_DWF Out = (PS_OUT_SCENE_DWF) 0;
    
    Out.vDiffuse = float4(1.f, 1.f, 1.f, 0.8f);
    Out.vWorld = In.vWorldPos;

    /* SceneFactor (R:IsAdditionalExposure (not -1 is True), G:Outline (1 is False), B:FaceFow, A:FaceRev) */
    Out.vFactor = float4(1.f, 0.f, 0.f, 0.f);
	
    return Out;
}

PS_OUT_SCENE_DWF PS_EFFIGT_FIRE_CORE(PS_IN_MESH In)
{
    PS_OUT_SCENE_DWF Out = (PS_OUT_SCENE_DWF) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    vector vColor = { 0.9f, 0.65f, 0.3f,0.1f };  
    vector vColor2 = { 0.8f, 0.2f, 0.f, 0.3f }; 
    
    if (vMtrlDiffuse.g <= 0.1f)
        vMtrlDiffuse = vColor2;
    else
        vMtrlDiffuse = vColor;
        
    Out.vDiffuse = vMtrlDiffuse;
    Out.vWorld = In.vWorldPos;
    
    //vMtrlDiffuse = vColor2;
    
    //float fsawpcolor = vMtrlDiffuse.r;
    //vMtrlDiffuse.r = vMtrlDiffuse.g;
    //vMtrlDiffuse.g = fsawpcolor;
    
    //if (vMtrlDiffuse.r < 0.5f)
    //    vMtrlDiffuse.a = 0.5f;
    
    /* SceneFactor (R:IsAdditionalExposure (not -1 is True), G:Outline (1 is False), B:FaceFow, A:FaceRev) */
    Out.vFactor = float4(1.f, 0.f, 0.f, 0.f);
	
    return Out;
}

PS_OUT_DIFFUSE PS_MAIN_EFF_BIG_EXPLOSION(PS_IN_MESH In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;

    vector vMtrlDiffuse = { 1.f, 1.f, 1.f, 1.f };
 
   
    float3 vCamDir = normalize(g_vCamPosition - g_WorldMatrix._41_42_43_44);
    
    float fAngle = abs(dot(In.vNormal.xyz, vCamDir));
    fAngle = saturate((fAngle - 0.8f) * 5.f);
    vMtrlDiffuse.a = pow((1.f - fAngle), 3);

    float fHeight = 0.5f;
    if (In.vWorldPos.y <= g_fFloorHeight + fHeight)
        vMtrlDiffuse.a += saturate((fHeight - abs(In.vWorldPos.y - g_fFloorHeight)) / fHeight);
    
    if (vMtrlDiffuse.a <= 0.01f)
        vMtrlDiffuse.a = 0.01f;
    
    vMtrlDiffuse -= (vector(0.f, 0.5f, 1.f, 0.f) * saturate(g_fTimeAcc));
    
    Out.vDiffuse = vMtrlDiffuse;
	
    return Out;
}

PS_OUT_DIFFUSE PS_MAIN_MOND_SKYBOX(PS_IN_MESH In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a < 0.1f)
        discard;

    Out.vDiffuse        = vMtrlDiffuse;

    return Out;
}

float2 g_vSkyBoxScale, g_vSkyBoxOffset;

PS_OUT_SCENE_DN PS_MAIN_SKYBOX(PS_IN_MESH In)
{
    PS_OUT_SCENE_DN Out = (PS_OUT_SCENE_DN) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord * g_vSkyBoxScale + g_vSkyBoxOffset);
    
    if (vMtrlDiffuse.a < 0.1f)
        discard;

    Out.vDiffuse        = vMtrlDiffuse;
    Out.vNormal         = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

    return Out;
}

PS_OUT_SCENE_DNWFC PS_MAIN_DVALIN_PLATFORM_DESTROY(PS_IN_MESH In)
{
    PS_OUT_SCENE_DNWFC Out = (PS_OUT_SCENE_DNWFC) 0;

    vector vMtrlDiffuse     = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vCrackDiffuse    = g_Texture2.Sample(LinearSampler, In.vTexcoord * 0.25f);
    
    Out.vDiffuse    = vMtrlDiffuse;
    
    if (Out.vDiffuse.a < 0.01f)
        discard;
    
    if (vCrackDiffuse.a > 0.5f)
        Out.vDiffuse.rgb = float3(0.7000f, 0.8598f, 0.9569f);
    
    Out.vWorld      = In.vWorldPos;
    Out.vNormal     = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    
    Out.vCShader    = vector(10.f, 0.f, 0.f, 0.f);
    
    /* SceneFactor (R:IsAdditionalExposure (not -1 is True), G:Outline (1 is False), B:FaceFow, A:FaceRev) */
    Out.vFactor = float4(1.f, 0.f, 0.f, 0.f);

    return Out;
}

PS_OUT_SCENE_DNWF PS_DUNGEONPLATFORM(PS_IN_MESH In)
{
    PS_OUT_SCENE_DNWF Out = (PS_OUT_SCENE_DNWF) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord * g_vSkyBoxScale + g_vSkyBoxOffset);

    Out.vDiffuse    = vMtrlDiffuse;
    Out.vWorld      = In.vWorldPos;
    Out.vNormal     = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    
    /* SceneFactor (R:IsAdditionalExposure (not -1 is True), G:Outline (1 is False), B:FaceFow, A:FaceRev) */
    Out.vFactor = float4(1.f, 0.f, 0.f, 0.f);

    return Out;
}

PS_OUT_DIFFUSE PS_RUNE_SEALED(PS_IN_MESH In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, float2(In.vTexcoord.x * 2.f + g_vUVRate.x, In.vTexcoord.y * 0.5f));

    Out.vDiffuse    = vMtrlDiffuse;
    Out.vDiffuse.a  = (Out.vDiffuse.r + Out.vDiffuse.g + Out.vDiffuse.b) / 3.f;
    if (Out.vDiffuse.a < 0.01f)
        discard;

    return Out;
}

PS_OUT_SCENE_DNWFC PS_MAIN_CHAKRA(PS_IN_MESH In)
{
    PS_OUT_SCENE_DNWFC Out = (PS_OUT_SCENE_DNWFC) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vDiffuse    = vMtrlDiffuse;
    Out.vWorld      = In.vWorldPos;
    Out.vNormal     = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vCShader    = float4(20.00f, 0.000f, 0.000f, 0.f);
    
    /* SceneFactor (R:IsAdditionalExposure (not -1 is True), G:Outline (1 is False), B:FaceFow, A:FaceRev) */
    Out.vFactor = float4(1.f, 0.f, 0.f, 0.f);

    return Out;
}

PS_OUT_SCENE_DC PS_MAIN_LIGHTNING(PS_IN_MESH In)
{
    PS_OUT_SCENE_DC Out = (PS_OUT_SCENE_DC) 0;
    
    Out.vDiffuse = float4(1.f, 0.6f, 0.9f, 1.f);
    Out.vCShader = float4(10.f, 0.f, 0.f, 0.f);

    return Out;
}

PS_OUT_SCENE_DC PS_TRAIL_TEST(PS_IN_MESH In)
{
    PS_OUT_SCENE_DC Out = (PS_OUT_SCENE_DC) 0;

    if (In.vTexcoord.y < 1 - g_fTimeAcc)
        discard;
    
    float2 vPos = { 0.25f, 0.f };
    float2 vBgPos = {0.f,0.f };
    float fY = g_fTimeAcc;
    if (fY >=1.f)
        fY = 1.f;
    float2 vScale = { 0.5f, fY };
    
    vector vColor = { 161.f / 255.f, 125.f / 255.f, 1.f, 1.f };
    vector vDistortTex = g_MaskTexture.Sample(LinearSampler, vPos + In.vTexcoord * vScale);
    float fDistort = vDistortTex.b;
    vBgPos.x += fDistort;
    vector vMaskTex = g_Texture.Sample(LinearSampler, vPos+In.vTexcoord * vScale);
    vector vMtrlDiffuse = g_Texture2.Sample(LinearSampler, vBgPos+ In.vTexcoord);
    
    float fGrayscale = vMaskTex.r + vMaskTex.g + vMaskTex.b;
    
    vColor.a = fGrayscale;
   
    vColor.rgb += fGrayscale * 0.2f;
    
    vColor.a -= g_fTimeAcc;
    
    if(vColor.a < 0.1f)
        vColor = vMtrlDiffuse;
    
    if (fGrayscale -g_fTimeAcc <0.1f)
        vMaskTex.b -= (g_fTimeAcc - 1.f);
    
    if (vMaskTex.b <=0.f)
        discard;
    
    
        Out.vDiffuse = vColor;
    Out.vCShader = float4(15.f, 0.f, 0.f, 0.f);

    return Out;
}



/* Shadow */
VS_OUT_GEOPOSITION VS_SHADOW(VS_IN_MESH In)
{
    VS_OUT_GEOPOSITION Out = (VS_OUT_GEOPOSITION) 0;
	
    Out.vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

    return Out;
}

struct GS_IN
{
    float4 vPosition : POSITION;
};

struct GS_OUT
{
    float4  vPosition : SV_POSITION;
    uint    iRTIndex : SV_RenderTargetArrayIndex;
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
    float4 vPosition : SV_POSITION;
};

void PS_SHADOW(PS_SHADOWIN In)
{
}

technique11 DefaultTechnique
{
	pass DefaultPass
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
		PixelShader		= compile ps_5_0 PS_MAIN();
	}

	pass NonCullPass
	{
		SetRasterizerState(RS_NonCullSolid);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
		PixelShader		= compile ps_5_0 PS_MAIN();
	}

	pass TerrainPass
	{
		SetRasterizerState(RS_NonCullSolid);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
		PixelShader		= compile ps_5_0 PS_MAIN_TERRAIN();
	}

    pass ZhongliPillar
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_PILLAR();
    }

    pass WindRangePass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_WIND_RANGE();
    }

    pass WindBladePass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_WIND_BLADE();
    }

    pass ScratchPass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SCRATCH();
    }

    pass RushPass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RUSH();
    }

    pass Rush2Pass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RUSH2();
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
    pass Eff_SP_Explosion
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EFF_SP_EXPLODE();
    }
    pass Eff_Slash
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EFF_Slash();
    }

    pass BurstBackGroundPass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BURST_BACKGROUND();
    }

    pass ShieldPass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_ZHONGLI_SHIELD();
    }

    pass Eff_Element_Shield
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_MAIN_EFF_ELEM_SHIELD();
    }

    pass Eff_Burst_Ray
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BURST_RAY();
    }
    pass Eff_Slash_Diluc
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EFF_SlashDiluc();
    }
    pass Eff_Andrius_Claw
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EFF_Andrius_Claw();
    }
    pass Eff_Diluc_Flame_Slash
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_NoneWrite, 0);
        SetBlendState(BS_WeightedBlended, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Diluc_Flame_Slash();
    }
    pass Eff_Diluc_Outer_Flame_Slash
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_NoneWrite, 0);
        SetBlendState(BS_WeightedBlended, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Diluc_Outer_Flame_Slash();
    }
    pass Eff_Flame_Enchant_Cross
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Flame_Enchant_Cross();
    }
    pass Eff_Trail_Zhongli_Ring
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Trail_Zhongli_Ring();
    }
    pass Dissolve
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Dissolve();
    }
    pass Eff_Barbara_Star
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Eff_Barbara_Star();
    }
    pass Eff_Gliding_Ring
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_GLIDING_RING();
    }
    pass Effigy_Fire_Cube
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFFIGY_FIRE_CUBE();
    }
    pass Eff_Accelerating_Ring
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_ACCELERATING_RING();
    }
    pass EFFIGT_FIRE_CORE
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFFIGT_FIRE_CORE();
    }
    pass EFF_BIG_EXPLOSION
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EFF_BIG_EXPLOSION();
    }

    pass MOND_SKYBOX
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_MOND_SKYBOX();
    }

    pass EFF_DVALIN_SKYBOX
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SKYBOX();
    }

    pass DVALIN_PLATFORM_DESTROY
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DVALIN_PLATFORM_DESTROY();
    }

    pass DungeonPlatformPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_DUNGEONPLATFORM();
    }

    pass RuneSealedPass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_RUNE_SEALED();
    }

    pass SKILL_CHAKRA
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_CHAKRA();
    }
    pass Effect_Lightning
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHTNING();
    }
    pass Effect_Trail_Test
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TRAIL_TEST();
    }
}