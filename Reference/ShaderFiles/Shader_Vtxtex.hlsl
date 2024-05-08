#include    "Engine_Shader_Defines.hlsli"

vector g_vGradeColor;
float2 g_vUVPos;

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

VS_OUT_POSTEXPRJ VS_MAIN_BWOIT(VS_IN_POSTEX In)
{
    VS_OUT_POSTEXPRJ Out = (VS_OUT_POSTEXPRJ) 0;
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

PS_OUT_DIFFUSE PS_MAIN(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    Out.vDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);

    if (Out.vDiffuse.a < 0.01f)
        discard;
    
    return Out;
}

PS_OUT_DIFFUSE PS_UVCOLOR_HORIZONTAL(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    Out.vDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);

    if (Out.vDiffuse.a < 0.01f)
        discard;
    
    if (In.vTexcoord.x < g_vUVRate2.x / g_vUVRate2.y)
        Out.vDiffuse.rgb *= g_vColor1;
    else
        Out.vDiffuse.rgb *= g_vColor2;
    
    return Out;
}

PS_OUT_SCENE_DC PS_EFFECT_HITTING_0(PS_IN_POSTEX In)
{
    PS_OUT_SCENE_DC Out = (PS_OUT_SCENE_DC) 0;
    
    vector vMtrlDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.g < 0.5)
        discard;
    
    vector vEffectDiffuse = { 1.f, 1.f, 0.9157f, 0.f };
    
    vEffectDiffuse.b = 1.5f - vMtrlDiffuse.r;
        
    if (vMtrlDiffuse.r < g_ColorRate)
        discard;
    
    vMtrlDiffuse = vEffectDiffuse;
    vMtrlDiffuse.a = 0.5f;

    Out.vDiffuse = vMtrlDiffuse;
    if (Out.vDiffuse.a < 0.01f)
        discard;
    
    Out.vCShader = float4(2.f, 0.f, 0.f, 0.f);

    return Out;
}

PS_OUT_SCENE_DC PS_EFFECT_HITTING_1(PS_IN_POSTEX In)
{
    PS_OUT_SCENE_DC Out = (PS_OUT_SCENE_DC) 0;
    
    vector vMtrlDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);

    vMtrlDiffuse.a = vMtrlDiffuse.r - g_ColorRate;
    
    vector vEffectDiffuse = { 1.f, 1.f, 0.9157f, 0.f };
    
    vEffectDiffuse.b = 1.5f - vMtrlDiffuse.r;

    vMtrlDiffuse = vEffectDiffuse;
    //vMtrlDiffuse.a = 0.1f;

    if (vMtrlDiffuse.r >= 0.8f)
        vMtrlDiffuse.a = 0.5f;

    Out.vDiffuse = vMtrlDiffuse;
    if (Out.vDiffuse.a < 0.01f)
        discard;
    
    Out.vCShader = float4(2.f, 0.f, 0.f, 0.f);

    return Out;
} 

PS_OUT_SCENE_DC PS_EFFECT_HITTING_2(PS_IN_POSTEX In)
{
    PS_OUT_SCENE_DC Out = (PS_OUT_SCENE_DC) 0;
    
    vector vMtrlDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);
    vector vEffectDiffuse = { 1.f, 1.f, 0.9f, 0.f };    // ¹àÀº ³ë¶û
    vector vEffectDiffuse2 = { 0.9686f, 0.8627f, 0.5725f, 0.f }; // ÁÖÈ²
    
    if (vMtrlDiffuse.r < g_ColorRate 
        && vMtrlDiffuse.g < g_ColorRate
        && vMtrlDiffuse.b < g_ColorRate)
        discard;

    vMtrlDiffuse.r = vEffectDiffuse2.r;
    vMtrlDiffuse.g = vEffectDiffuse2.g;
    vMtrlDiffuse.b = vEffectDiffuse2.b;
    vMtrlDiffuse.a = vEffectDiffuse2.a;
    
    Out.vDiffuse = vMtrlDiffuse;
    if (Out.vDiffuse.a < 0.01f)
        discard;
    
    Out.vCShader = float4(2.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT_BWOIT PS_EFFECT_FLAME0(PS_IN_POSTEXPRJ In)
{
    PS_OUT_BWOIT Out = (PS_OUT_BWOIT) 0;
    
    float2 vPos = { 0.8f, 0.f };
    float2 vScale = { 1.0f,1.f };
    vector vMtrlDiffuse = g_Texture.Sample(LinearSampler, float2(1-In.vTexcoord.x,In.vTexcoord.y));
    if (vMtrlDiffuse.r < 0.1f)
        discard;
    
    //if (In.vTexcoord.x >= 0.5f)
    //    discard;
    
    
    vPos.x = vMtrlDiffuse.r-0.15f - g_ColorRate;

   if (vPos.x <=0.35)
        vPos.x = 0.35f;
    
   vector vRampColor = g_DiffuseTexture.Sample(LinearClampSampler, vPos + In.vTexcoord *0.1);
   vector vNoiseColor = g_MaskTexture.Sample(LinearSampler, In.vTexcoord * vScale);
        
    //vNoiseColor.r -= g_ColorRate;
    //if (vNoiseColor.r <0.f)
    //    discard;
    
    
    //if(vMtrlDiffuse.r >0.1f)
    //    vMtrlDiffuse.r -= g_ColorRate * 2.f;
    //else
    //    vMtrlDiffuse.r -= g_ColorRate / 4.f;
    
    //    if (vMtrlDiffuse.r < 0.f)
    //        discard;
    
         vRampColor.a = vMtrlDiffuse.r- g_ColorRate*1.1;
    if (vRampColor.a < 0.1f)
        discard;
    
        if (vRampColor.r > 0.3f)
            vRampColor.r -= g_ColorRate / 10.f;
    
    vector vColor = vRampColor;
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

PS_OUT_BWOIT PS_EFFECT_FLAME1(PS_IN_POSTEXPRJ In)
{
    PS_OUT_BWOIT Out = (PS_OUT_BWOIT) 0;
    
    float2 vPos = g_vUVRate;
    float2 vScale = { 0.5f, 1.f };
    float2 vRampPos = { 0.5f, 0.f };
    
    //if (In.vTexcoord.x >= 0.5f)
    //    discard;
       
    vector vMtrlDiffuse = g_Texture.Sample(LinearSampler, vPos+In.vTexcoord*vScale);
    if (vMtrlDiffuse.r < 0.1f)
        discard;
   
    vRampPos.x = vMtrlDiffuse.r - 0.2f - g_ColorRate;

    if (vRampPos.x <= 0.35)
        vRampPos.x = 0.35f;
    
    vector vRampColor = g_DiffuseTexture.Sample(LinearClampSampler, float2(vRampPos.x, 1.f));

    vRampColor.a = vMtrlDiffuse.g - g_ColorRate;
    
    if (vRampColor.a <0.1f)
        discard;
    
    
    vector vColor = vRampColor;
    
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


PS_OUT_DIFFUSE PS_EFFECT_RUSH_CIRCLE(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    vector vMtrlDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);
    vector vEffectDiffuse = { 0.0392f, 0.7059f, 0.9804f, 0.f }; // rgb(10, 180, 250) ¸¶³ª»ö±ò

    if (vMtrlDiffuse.g < 0.4f)
        discard;
    
    vMtrlDiffuse = vEffectDiffuse;
    vMtrlDiffuse.a = 0.5f;
    
    Out.vDiffuse = vMtrlDiffuse;

    return Out;
}

PS_OUT_DIFFUSE PS_EFFECT_ICE_FLOOR(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    vector vMtrlDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord * g_vUVScaleRate);
    vector vEffectDiffuse = g_Texture2.Sample(LinearSampler, In.vTexcoord);
    vector vEffectGreen = vEffectDiffuse.g;
    
    if (vEffectDiffuse.r < 0.1f && vEffectDiffuse.g < 0.1f)
        discard;
    
    // »ö°¨ ¹à°Ô
    vector vColorBright = { 0.7f, 0.7f, 0.7, 1.f };
    vMtrlDiffuse += vColorBright;
    
    vEffectDiffuse = vMtrlDiffuse;
    vEffectDiffuse.a = vEffectGreen.g + 0.7f;
    
    // »ç¶óÁö°Ô
    if (g_ColorRate > 0.f)
    {
        if (vMtrlDiffuse.r < g_ColorRate)
            discard;
    }

    Out.vDiffuse = vEffectDiffuse;

    return Out;
}

PS_OUT_DIFFUSE PS_EFFECT_FOG(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    vector vMtrlDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);

    //vector vWindColor = { 0.59215686274, 0.82352941176, 0.94117647058, 0.f };
    //vWindColor.a = vMtrlDiffuse.r;
    
    if (vMtrlDiffuse.a < 0.1f)
        discard;
    
    vMtrlDiffuse.a -= g_ColorRate;
    
    Out.vDiffuse = vMtrlDiffuse;

    return Out;
}

PS_OUT_DIFFUSE PS_EFFECT_ITEM_GLOW(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    vector vGradeColor = g_vGradeColor;
    
    float fGrayScale = (vMtrlDiffuse.x + vMtrlDiffuse.y + vMtrlDiffuse.z) / 3.f;
    
    if (fGrayScale<0.1f)
        discard;
    float fMax = vMtrlDiffuse.r + vMtrlDiffuse.g + vMtrlDiffuse.b;
    
    
    vGradeColor.r *= (1.2 + sin(g_fTimeAcc*3.f) / 5.f);
    vGradeColor.g *= (1.2 + sin(g_fTimeAcc*3.f) / 5.f);
    vGradeColor.b *= (1.2 + sin(g_fTimeAcc*3.f) / 5.f);
    
    vGradeColor.a = fMax;
    
    if (vGradeColor.a <0.1f)
        discard;
    
    Out.vDiffuse = vGradeColor;
    
    return Out;
}

PS_OUT_DIFFUSE PS_EFF_LIGHT_WAVE(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    float fGrayScale = (vMtrlDiffuse.x + vMtrlDiffuse.y + vMtrlDiffuse.z) / 3.f;
    
    if (fGrayScale < 0.1f)
        discard;
    
    vMtrlDiffuse.a -= g_fTimeAcc;
    
    if (vMtrlDiffuse.a<0.1f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;

    return Out;
}

PS_OUT_SCENE_DC PS_EFF_PIERCING_TRAIL(PS_IN_POSTEX In)
{
    PS_OUT_SCENE_DC Out = (PS_OUT_SCENE_DC) 0;
    
    float2 vScale = { 1.f,1.f };
    float2 vPos = { 0.f, -0.5f };
    
    if (vPos.y<0.f)
        vPos.y += g_fTimeAcc*3.f;
    
    if (vPos.y >= 0.f)
        vPos.y = 0.f;
    
    if (vPos.y + In.vTexcoord.y < 0.f)
        discard;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, vPos + In.vTexcoord * vScale);
    vector vClampDiff = g_Texture.Sample(LinearClampSampler, In.vTexcoord);
    
    vector vColor = { 1.f, 1.f, 0.7058f, 0.f };
    
    
    vMtrlDiffuse.r -= g_fTimeAcc*2.f;
    float fGrayScale = (vMtrlDiffuse.x + vMtrlDiffuse.y + vMtrlDiffuse.z) / 3.f;
    
    if (fGrayScale < 0.1f)
        discard;
    if (vMtrlDiffuse.b <= 0.01f)
        discard;
    
    vColor.a = fGrayScale+0.5f;// - (g_fTimeAcc * 0.5f);
    
    vMtrlDiffuse = vColor;
    
    if (vMtrlDiffuse.a < 0.1f)
        discard;
    
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vCShader = float4(5.f, 0.f, 0.f, 0.f);
    
    return Out;
}


PS_OUT_DIFFUSE PS_EFF_TRANSITION(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;

    
    float2 vScale = { 1.f, 0.95f };
    float2 vPos = { 0.f, 0.1f };
    
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, vPos + In.vTexcoord * vScale);
    vector vIntensity = g_Texture.Sample(LinearClampSampler, In.vTexcoord);
    

    vector vColor = { 1.f, 1.0f, .8f, 0.9f };
    vColor.a = vIntensity.g - g_fTimeAcc * 9.f;
    
    float fGrayScale = (vMtrlDiffuse.x + vMtrlDiffuse.y + vMtrlDiffuse.z) / 3.f;
  
    //if (fGrayScale < 0.1f)
    //    discard;

    vMtrlDiffuse.a = fGrayScale;
   
    
    if (vColor.a < 0.1f)
        discard;
    
    //vColor += vMtrlDiffuse;
    

    Out.vDiffuse = vColor;
    
    return Out;
}

PS_OUT_DIFFUSE PS_EFF_GROUND_STOMP(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    float2 vScale = { 1.f, 1.f };
    float2 vPos = { 0.f, 0.0f };
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, vPos + In.vTexcoord * vScale);
   

    vector vColor = { 1.f, 1.0f, .8f, 1.f };
    
    if (vMtrlDiffuse.g <0.1f)
        discard;
    if (vMtrlDiffuse.b > 0.05f)
        vColor.a = 0.f;

    vColor.a -= (-0.2f + (g_fTimeAcc * 2.f));
   
    
    Out.vDiffuse = vColor;

    return Out;
}

PS_OUT_SCENE_DC PS_EFF_IMPACT_GLOW(PS_IN_POSTEX In)
{
    PS_OUT_SCENE_DC Out = (PS_OUT_SCENE_DC) 0;
    
    float2 vScale = { 0.5f, 1.f };
    float2 vPos = g_vUVPos;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, vPos + In.vTexcoord * vScale);
   

    //YELLOW vector vColor = { 1.f, 1.0f, .75f, 1.f };
    //BLUE vector vColor = { 190.f / 255.f, 247.f/255.f, 1.f, 1.f };
    //vector vColor = { 0.75f, 1.f, 1.f, 1.f };
    vector vColor = g_vColor;
       
    
    if (vMtrlDiffuse.r < 0.01f)
        discard;

   if(vMtrlDiffuse.r - (g_fTimeAcc*2.f)<0.01f)
        discard;
   
    //if (vMtrlDiffuse.r <= 1.f - g_fTimeAcc * 2.f)
    //    vColor.a = vMtrlDiffuse.r - g_fTimeAcc * 2.f;
    
    vMtrlDiffuse = vColor;
    
    //vMtrlDiffuse.a -= (g_fTimeAcc);
    
    if (vMtrlDiffuse.a < 0.05f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;
    Out.vCShader = float4(5.f, 0.f, 0.f, 0.f);

    return Out;
}

PS_OUT_DIFFUSE PS_EFF_ENCHANT_FLAME(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    float2 vScale = { 1.f, 1.f };
    float2 vPos = { 0.f, 0.0f };
    
    vPos.y -= g_fTimeAcc/2.f;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearClampSampler, In.vTexcoord);
    
    vector vNoiseTex = g_Texture.Sample(LinearSampler, vPos + In.vTexcoord * vScale);
    vector vNoiseTex2 = g_Texture.Sample(LinearSampler, vPos + In.vTexcoord * vScale*2.f);
    
    vNoiseTex = (vNoiseTex - 0.5f) * 2.f;
    vNoiseTex2 = (vNoiseTex2 - 0.5f) * 2.f;

    vector vNoiseTex3 = vNoiseTex + vNoiseTex2;
    
    vNoiseTex3.a = vMtrlDiffuse.b*3.f;
    
    float fGrayScale = (vMtrlDiffuse.r + vMtrlDiffuse.g + vMtrlDiffuse.b) / 3.f;
    
    if (fGrayScale< 0.01f)
        discard;
    
    vMtrlDiffuse.a = fGrayScale;
    vMtrlDiffuse.r = vNoiseTex3.r + 0.5f;
    vMtrlDiffuse.g = vNoiseTex3.g;
    
    if (vMtrlDiffuse.r < vMtrlDiffuse.g)
        vMtrlDiffuse.r = vMtrlDiffuse.g;

        vMtrlDiffuse.b = 0.f;
    //vMtrlDiffuse.r += vNoiseTex.r;
    
    //vMtrlDiffuse.a = fGrayScale;
    //vNoiseTex.a = vMtrlDiffuse.b;
    //vMtrlDiffuse.r += vMtrlDiffuse.b;
    //if (vMtrlDiffuse.r<vMtrlDiffuse.g)
    //        vMtrlDiffuse.r = vMtrlDiffuse.g;
        
    //vMtrlDiffuse.b = 0;
    
    //if (vMtrlDiffuse.r > vMtrlDiffuse.g)
    //    vMtrlDiffuse.r += vNoiseTex.r;
    
   
    Out.vDiffuse = vMtrlDiffuse;

    return Out;
}

PS_OUT_DIFFUSE PS_EFF_CROSS(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE)0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    float fGrayScale = (vMtrlDiffuse.x + vMtrlDiffuse.y + vMtrlDiffuse.z) / 3.f;

    if (fGrayScale < 0.1f)
        discard;

    Out.vDiffuse = vMtrlDiffuse;

    return Out;
}

PS_OUT_SCENE_DC PS_EFF_SMOKE(PS_IN_POSTEX In)
{
    PS_OUT_SCENE_DC Out = (PS_OUT_SCENE_DC) 0;

    float2 fPos = { 0.f, 0.f};
    float2 fScale = { 0.8f, 0.8f };
    float2 fDiffTexScale = { 0.5f, 0.5f };
    float2 fDiffPos = g_vUVPos;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, fDiffPos+ In.vTexcoord * fDiffTexScale);
    
    if (vMtrlDiffuse.a<0.1f)
        discard;
    
    float fBrightness = vMtrlDiffuse.r * vMtrlDiffuse.g;
    float fX = max(fBrightness, vMtrlDiffuse.b * 0.55);
    float fClamp = clamp(fX, 0.5f, 0.9f);

    
    vector vDissTex = g_Texture2.Sample(LinearClampSampler, fPos +In.vTexcoord * fScale);

    
    vector vRampDiff = g_Texture.Sample(LinearClampSampler, float2((fClamp - g_fTimeAcc * 0.7f), 0.5f));
    vector vColor = vRampDiff;
    
    
    //float fDissolveR = 1 - vMtrlDiffuse.r;
    float fDissolveG = 1 - vMtrlDiffuse.g;
    fDissolveG -= g_fTimeAcc;
    
    ////fDissolveR -= g_fTimeAcc;
    vDissTex.r -= g_fTimeAcc;
    vDissTex.g -= g_fTimeAcc;
    
    if (vDissTex.r < 0.05f || vDissTex.g < 0.05f || fDissolveG<0.05f)
        vColor.a -= min(min(vDissTex.r, vDissTex.g),fDissolveG);
    
    if (fDissolveG < 0.01f)
       discard;
    if (vDissTex.r < 0.01f || vDissTex.g < 0.01f)
       discard;
    
    
    Out.vDiffuse = vColor;
    Out.vCShader = float4(5.f, 0.f, 0.f, 0.f);
    return Out;
}

PS_OUT_DIFFUSE PS_EFF_SQUENCE1(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;

    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, g_vUVRate2+In.vTexcoord * g_vUVScaleRate);
    
    vector vMaskTexture = g_Texture.Sample(LinearSampler, In.vTexcoord);

    
    float fGrayScale = (vMtrlDiffuse.r+ vMtrlDiffuse.g + vMtrlDiffuse.b) / 3.f;
    
    if (fGrayScale < 0.1f)
        discard;
 
    float fOutline = vMtrlDiffuse.r;
    
    vMtrlDiffuse.a = vMtrlDiffuse.g;
    //vMtrlDiffuse.b = 1 - vMtrlDiffuse.g;
    vMtrlDiffuse.g = vMtrlDiffuse.r;
    
    //vMtrlDiffuse.a = vMaskTexture.r;
    
    if (vMtrlDiffuse.a < 0.1f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;

    return Out;
}

PS_OUT_DIFFUSE PS_EFF_SQUENCE(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, g_vUVRate2 + In.vTexcoord * g_vUVScaleRate);
    
    vector vMaskTexture = g_Texture.Sample(LinearSampler, In.vTexcoord);

    
    float fGrayScale = (vMtrlDiffuse.r + vMtrlDiffuse.g + vMtrlDiffuse.b) / 3.f;
    
    if (fGrayScale < 0.1f)
        discard;
 
    vector vColor = { 1.f, 1.f, 0.75f, 1.f };
    
    vColor.a = vMtrlDiffuse.r;
    
    vMtrlDiffuse.g -= g_fTimeAcc*3;
    if (vMtrlDiffuse.g < 0.1f)
         discard;
    
    Out.vDiffuse = vColor;
    
    return Out;
}

PS_OUT_DIFFUSE PS_EFF_WATER_FOG(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    float fPosX;
    
    vector vMaskTexture = g_Texture.Sample(LinearSampler, In.vTexcoord);

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, float2(0.5+g_ColorRate / 10.f, 0.1f));
    
    float fGrayScale = (vMaskTexture.r + vMaskTexture.g + vMaskTexture.b) / 3.f;
    
    if (fGrayScale < 0.01f)
        discard;
   
    //if(vMaskTexture.g>0.6f)
    //    vMaskTexture.g = 0.6f;
    
    vMtrlDiffuse.a = vMaskTexture.r + vMaskTexture.g;
    
    if (vMtrlDiffuse.a > 0.6f)
        vMtrlDiffuse.a = 0.6f;
    
    if (vMtrlDiffuse.a < 0.01f)
        discard;
    
    
    //vector vColor = vMtrlDiffuse;
    
    Out.vDiffuse = vMtrlDiffuse;
    
    return Out;
	
}

PS_OUT_DIFFUSE PS_EFF_Barbara_Burst_Circle(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    float2 vPos = { 0.f,0.f };
    float2 vScale = { 1.f, 1.f };
    vPos.x += g_ColorRate / 5.f;
    vPos.y += g_ColorRate / 5.f;
    //vScale.y += (cos(g_ColorRate / 2.f) * 0.5f);
    
    vector vNoiseTexture = g_DiffuseTexture.Sample(LinearSampler, vPos + In.vTexcoord);
    float2 vUV = In.vTexcoord +vNoiseTexture.rg * 0.1f;
    
    vector vMaskTexture = g_Texture.Sample(LinearSampler, float2(vUV.x-0.070f, vUV.y-0.090f));
    vector vTurbulence = g_Texture2.Sample(LinearSampler, vPos+In.vTexcoord);
    
    
    float fGrayScale = (vMaskTexture.r + vMaskTexture.g + vMaskTexture.b) / 3.f;
    
    if (fGrayScale <0.1f)
        discard;
    
    vector vColor = { 24.f / 255.f, 76.f / 255.f, 178.f / 255.f, 1.f };
    
    vTurbulence.rgb *= 0.3f;
  
    vColor += (vMaskTexture * 0.7) + vTurbulence; // + vMtrlDiffuse * 0.5);
    vColor.a = 0.5f;
    Out.vDiffuse = vColor;
    
    return Out;
	
}

PS_OUT_DIFFUSE PS_EFF_Barbara_Normal_Atk_Ring(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;

    vector vMtrlDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);

    
    float fGrayScale = (vMtrlDiffuse.r + vMtrlDiffuse.g + vMtrlDiffuse.b*20.f) / 3.f;
    
    if (fGrayScale < 0.1f)
        discard;
    
    vector vColor = { 183.f / 255.f, 248.f / 255.f, 248.f / 255.f, 1.f };
    
    
    vColor.a = vMtrlDiffuse.g + vMtrlDiffuse.b * 10.f;
    
    vColor.a -= g_ColorRate*10.f;
    
    if (vColor.a < 0.1f)
        discard;
    
    Out.vDiffuse = vColor;
    
    return Out;
}

PS_OUT_DIFFUSE PS_EFF_Barbara_Normal_Atk_Star(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;

    vector vMtrlDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);

    float fGrayScale = (vMtrlDiffuse.r + vMtrlDiffuse.g + vMtrlDiffuse.b) / 3.f;
    
    if (fGrayScale < 0.1f)
        discard;
    
    vector vColor = { 183.f / 255.f, 248.f / 255.f, 248.f / 255.f, 1.f };
    
    vColor.a = vMtrlDiffuse.r*2.f + vMtrlDiffuse.g * 2.f;
    
    vColor.a -= g_ColorRate*10.f;
    
    if (vColor.a < 0.1f)
        discard;
    
    Out.vDiffuse = vColor;
    
    return Out;
}


PS_OUT_DIFFUSE PS_EFF_Liquid_Splash(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;

    float2 vPos = { 0.f, 0.f };
    float2 vScale = { 4.f,4.f};
    float2 vCorrection = {0.65f,0.5f };
    
    if ((vScale.x - g_fTimeAcc * 7)>0.f)
    {
        vScale.y -= g_fTimeAcc * 7.f;
        vScale.x -= g_fTimeAcc * 7.f;
    }
    
    //vector vDistTex = g_Texture.Sample(LinearClampSampler, (In.vTexcoord - vPos) * vScale+vPos);
    //vPos.x = vDistTex.r * 0.7f;
    //vPos.y = vDistTex.g * 0.3f;
    vector vNoiseTex = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    
    float fGrayScale = vMtrlDiffuse.r;
    
    if (fGrayScale < 0.1f)
        discard;
    vMtrlDiffuse += vNoiseTex;
    
    vector vColor = g_vColor;
    //vector vColor = { 0.85f,1.f, 1.f, 1.f };
    
    //vMtrlDiffuse.g -= (g_fTimeAcc*3.f );
    //if (vMtrlDiffuse.g*3.f >0.1f)
    //    discard;
    //if ((g_fTimeAcc*3.f ) > 1.f)
        vMtrlDiffuse.r -= g_fTimeAcc * 4.f;
    
    vColor.a = vMtrlDiffuse.r*2.f;
    
    if (vColor.a < 0.1f)
        discard;
    
    Out.vDiffuse = vColor;
    
    return Out;
}

PS_OUT_DIFFUSE PS_EFF_Liquid_Spray(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, g_vUVRate2 + In.vTexcoord * g_vUVScaleRate);
    

    
    float fGrayScale = (vMtrlDiffuse.r + vMtrlDiffuse.g + vMtrlDiffuse.b) / 3.f;
    
    if (fGrayScale < 0.1f)
        discard;
 
    vector vColor = g_vColor;
    
    vColor.a = vMtrlDiffuse.r;
    
    vMtrlDiffuse.r -= g_fTimeAcc;
    if (vMtrlDiffuse.r < 0.1f)
        discard;
    
    Out.vDiffuse = vColor;
    
    return Out;
}

PS_OUT_DIFFUSE PS_EFF_Effigy_Recharge_Trail(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    float2 vPos = { 0.f,0.f};
    float2 vScale = { 1.f,1.f};
    
    vPos.x += g_fTimeAcc;
    
    vector vNoiseTex = g_Texture.Sample(LinearSampler, float2(In.vTexcoord.y,In.vTexcoord.x));
    
    vPos.x += vNoiseTex.r * 0.3f * vScale.x;
    
    In.vTexcoord.y = saturate(In.vTexcoord.y * vScale.y);
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, vPos + In.vTexcoord );
    
    
    float fGrayScale = (vMtrlDiffuse.r + vMtrlDiffuse.g) / 2.f;
   
    if (fGrayScale < 0.2f)
        discard;

    
    Out.vDiffuse = vMtrlDiffuse + vNoiseTex.r;
    
    return Out;
}

PS_OUT_DIFFUSE PS_RAIDEN_SYMBOL(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    vector vMtrlDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);

    float fGrayScale = (vMtrlDiffuse.r + vMtrlDiffuse.g + vMtrlDiffuse.b) / 3.f;
    
    if (fGrayScale < 0.1f)
        discard;
    
    if (vMtrlDiffuse.a < 0.1f)
        discard;

    vMtrlDiffuse.r = vMtrlDiffuse.r * 0.95f;
    vMtrlDiffuse.g = vMtrlDiffuse.r * 0.85f;
    vMtrlDiffuse.b = vMtrlDiffuse.r; 

    //Out.vDiffuse = (vMtrlDiffuse.r, vMtrlDiffuse.r, vMtrlDiffuse.r, vMtrlDiffuse.a);
    
    Out.vDiffuse = vMtrlDiffuse;

    return Out;
}

PS_OUT_SCENE_DC PS_RAIDEN_LIGHT(PS_IN_POSTEX In)
{
    PS_OUT_SCENE_DC Out = (PS_OUT_SCENE_DC) 0;
    
    if (In.vTexcoord.x > g_vUVRate.x * 5.f)
        discard;
    
    Out.vDiffuse = float4(1.f, 1.f, 1.f, 1.f);
    Out.vCShader = float4(2.f, 0.f, 0.f, 0.f);
    
    
    // vector vMtrlDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);

    // float fGrayScale = (vMtrlDiffuse.r + vMtrlDiffuse.g + vMtrlDiffuse.b) / 3.f;
    // 
    // if (fGrayScale < 1.f) 
    //     discard;
    // 
    // if (vMtrlDiffuse.a < 0.1f)
    //     discard;
    // 
    // vMtrlDiffuse.r = vMtrlDiffuse.r * 0.95f;
    // vMtrlDiffuse.g = vMtrlDiffuse.r * 0.85f;
    // vMtrlDiffuse.b = vMtrlDiffuse.r;
    
    //Out.vDiffuse = (vMtrlDiffuse.r, vMtrlDiffuse.r, vMtrlDiffuse.r, vMtrlDiffuse.a);
   
    //Out.vDiffuse = vMtrlDiffuse;
    

    return Out;
}

PS_OUT_DIFFUSE PS_LINE_TRAP(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
   
    Out.vDiffuse = float4(1.f, 1.f, 1.f, 1.f);

    return Out;
}

PS_OUT_SCENE_DC PS_CHAIN_LIGHTNING(PS_IN_POSTEX In)
{
    PS_OUT_SCENE_DC Out = (PS_OUT_SCENE_DC) 0;
    
    vector vMtrlDiffuse = g_Texture.Sample(LinearSampler, float2(In.vTexcoord.x, In.vTexcoord.y * 0.25f + (0.25f * g_iIndex)));
    float fAverage = (vMtrlDiffuse.r + vMtrlDiffuse.g) * 0.5f;
    if (fAverage < 0.4f)
        discard;
    vMtrlDiffuse = float4(0.5f, 0.8f, 1.f, 1.f);
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vCShader = float4(10.f, 0.f, 0.f, 0.f);
    
    return Out;
}

PS_OUT_DIFFUSE PS_DOOR_PORTAL(PS_IN_POSTEX In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    
    
    vector vDistortTex = g_Texture2.Sample(LinearSampler, In.vTexcoord);
    
    In.vTexcoord.x += vDistortTex.r*0.7f + g_fTimeAcc/10.f;
    In.vTexcoord.y += vDistortTex.r*0.7f;
    
    vector vMtrlDiffuse = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vDiffuse = vMtrlDiffuse;

    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_MAIN();
    }

    pass AlphaBlendPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_MAIN();
    }

    pass UVColorHorizontalPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader     = compile ps_5_0 PS_UVCOLOR_HORIZONTAL();
    }

    pass Effect_HITTING_0
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader = compile ps_5_0 PS_EFFECT_HITTING_0();
    }

    pass Effect_HITTING_1
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader = compile ps_5_0 PS_EFFECT_HITTING_1();
    }

    pass Effect_HITTING_2
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader = compile ps_5_0 PS_EFFECT_HITTING_2();
    }

    pass Effect_Flame0
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_NoneWrite, 0);
        SetBlendState(BS_WeightedBlended, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_BWOIT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFFECT_FLAME0();
    }
    pass Effect_Flame1
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_NoneWrite, 0);
        SetBlendState(BS_WeightedBlended, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_BWOIT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFFECT_FLAME1();
    }

    pass Effect_Rush_Circle
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader = compile ps_5_0 PS_EFFECT_RUSH_CIRCLE();

    }

    pass Effect_Ice_Floor
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
        PixelShader = compile ps_5_0 PS_EFFECT_ICE_FLOOR();
    }

    pass Effect_Fog
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFFECT_FOG();
    }
    pass Eff_ITEM_GLOW
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFFECT_ITEM_GLOW();
    }
    pass Eff_Light_Wave
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFF_LIGHT_WAVE();
    }
    pass Eff_Piercing_Trail
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFF_PIERCING_TRAIL();
    }
    pass Eff_Transition
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFF_TRANSITION();
    }
    pass Eff_GroundStomp
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFF_GROUND_STOMP();
    }
    pass Eff_Impact_Glow
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFF_IMPACT_GLOW();
    }
    pass Eff_Enchant_Flame
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFF_ENCHANT_FLAME();
    }
    pass Eff_Cross
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFF_CROSS();
    }
    pass Eff_Smoke
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFF_SMOKE();
    }
    pass Eff_Squence
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFF_SQUENCE();
    }
    pass Eff_Water_Fog
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFF_WATER_FOG();
    }
    pass Eff_Barbara_Burst_Circle
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFF_Barbara_Burst_Circle();
    }
    pass Eff_Barbara_Normal_Atk_Ring
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFF_Barbara_Normal_Atk_Ring();
    }
    pass Eff_Barbara_Normal_Atk_Star
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFF_Barbara_Normal_Atk_Star();
    }
    pass Eff_Liquid_Splash
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFF_Liquid_Splash();
    }
    pass Eff_Liquid_Spray
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFF_Liquid_Spray();
    }
    pass Eff_Effigy_Recharge_Trail
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EFF_Effigy_Recharge_Trail();
    }
    pass Eff_Raiden_Skill_Symbol
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RAIDEN_SYMBOL();
    }
    pass Eff_Raiden_Skill_Light
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RAIDEN_LIGHT();
    }
    pass Line_Trap
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_LINE_TRAP();
    }
    pass Chain_Lightning
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_CHAIN_LIGHTNING();
    }
    pass Door_Portal
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DOOR_PORTAL();
    }
}