#include    "Engine_Shader_Defines.hlsli"

int2        g_iNumParticleRowCol = { 0, 0 };
int2        g_iCurrParticleRowCol = { 0, 0 };
float       g_fAngle = { 0.f };

vector ConvertLiteralSampling(sampler Sampler, float2 vUV)
{
    switch (g_iIndex)
    {
        case 0:
            return g_Textures[0].Sample(Sampler, vUV);
        case 1:
            return g_Textures[1].Sample(Sampler, vUV);
        case 2:
            return g_Textures[2].Sample(Sampler, vUV);
        case 3:
            return g_Textures[3].Sample(Sampler, vUV);
        case 4:
            return g_Textures[4].Sample(Sampler, vUV);
        case 5:
            return g_Textures[5].Sample(Sampler, vUV);
        case 6:
            return g_Textures[6].Sample(Sampler, vUV);
        case 7:
            return g_Textures[7].Sample(Sampler, vUV);
        case 8:
            return g_Textures[8].Sample(Sampler, vUV);
        case 9:
            return g_Textures[9].Sample(Sampler, vUV);
        case 10:
            return g_Textures[10].Sample(Sampler, vUV);
        case 11:
            return g_Textures[11].Sample(Sampler, vUV);
        case 12:
            return g_Textures[12].Sample(Sampler, vUV);
        case 13:
            return g_Textures[13].Sample(Sampler, vUV);
        case 14:
            return g_Textures[14].Sample(Sampler, vUV);
        case 15:
            return g_Textures[15].Sample(Sampler, vUV);
        case 16:
            return g_Textures[16].Sample(Sampler, vUV);
        case 17:
            return g_Textures[17].Sample(Sampler, vUV);
        case 18:
            return g_Textures[18].Sample(Sampler, vUV);
        case 19:
            return g_Textures[19].Sample(Sampler, vUV);
        case 20:
            return g_Textures[20].Sample(Sampler, vUV);
        case 21:
            return g_Textures[21].Sample(Sampler, vUV);
        case 22:
            return g_Textures[22].Sample(Sampler, vUV);
        case 23:
            return g_Textures[23].Sample(Sampler, vUV);
        case 24:
            return g_Textures[24].Sample(Sampler, vUV);
        case 25:
            return g_Textures[25].Sample(Sampler, vUV);
        case 26:
            return g_Textures[26].Sample(Sampler, vUV);
        case 27:
            return g_Textures[27].Sample(Sampler, vUV);
        case 28:
            return g_Textures[28].Sample(Sampler, vUV);
        case 29:
            return g_Textures[29].Sample(Sampler, vUV);
        case 30:
            return g_Textures[30].Sample(Sampler, vUV);
        case 31:
            return g_Textures[31].Sample(Sampler, vUV);
        default:
            return g_Textures[0].Sample(Sampler, vUV);
    }
}

struct VS_IN
{
    float3 vPosition    : POSITION;
    float2 vPSize       : PSIZE;

    row_major float4x4 TransformMatrix : WORLD;
};

struct VS_OUT
{
    float4  vPosition        : POSITION;
    float2  vPSize           : PSIZE;
    
    float4  vFactor          : COLOR;   // R : Alpha, G : Random, B : LifeTime
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    Out.vFactor.r   = In.TransformMatrix._13; // ?
    Out.vFactor.g   = In.TransformMatrix._24;
    Out.vFactor.b   = In.TransformMatrix._34;
    
    In.TransformMatrix._24 = 0.f;
    In.TransformMatrix._34 = 0.f;
    
    vector vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);
    
    Out.vPosition = mul(vPosition, g_WorldMatrix);
    Out.vPSize = float2(In.vPSize.x * In.TransformMatrix._11, In.vPSize.y * In.TransformMatrix._22);

    return Out;
}

struct GS_IN
{
    float4  vPosition        : POSITION;
    float2  vPSize           : PSIZE;
    
    float4  vFactor          : COLOR;   // R : Alpha, G : Random, B : LifeTime
};

struct GS_OUT
{
    float4  vPosition       : SV_POSITION;
    float2  vTexcoord       : TEXCOORD0;
	float4  vProjPos	    : TEXCOORD1;
    
    float4  vFactor         : COLOR;   // R : Alpha, G : Random, B : LifeTime
};

[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Triangles)
{
    GS_OUT Out[4] = { (GS_OUT) 0, (GS_OUT) 0, (GS_OUT) 0, (GS_OUT) 0 };

    float fDegree = g_fAngle;
    float fAngle = radians(fDegree);

    float3 vLook = normalize(g_vCamPosition - In[0].vPosition).xyz;
    float3 vRight = normalize(cross(float3(sin(fAngle), cos(fAngle), 0.f), vLook)) * In[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;

    Out[0].vPosition = vector(In[0].vPosition.xyz + vRight + vUp, 1.f);
    Out[0].vTexcoord = float2(0.f, 0.f);
    
    Out[1].vPosition = vector(In[0].vPosition.xyz - vRight + vUp, 1.f);
    Out[1].vTexcoord = float2(1.0f, 0.f);
   
    Out[2].vPosition = vector(In[0].vPosition.xyz - vRight - vUp, 1.f);
    Out[2].vTexcoord = float2(1.0f, 1.0f);

    Out[3].vPosition = vector(In[0].vPosition.xyz + vRight - vUp, 1.f);
    Out[3].vTexcoord = float2(0.f, 1.0f);
    
    Out[0].vFactor.r = In[0].vFactor.r;
    Out[1].vFactor.r = In[0].vFactor.r;
    Out[2].vFactor.r = In[0].vFactor.r;
    Out[3].vFactor.r = In[0].vFactor.r;
    
    Out[0].vFactor.g = In[0].vFactor.g;
    Out[1].vFactor.g = In[0].vFactor.g;
    Out[2].vFactor.g = In[0].vFactor.g;
    Out[3].vFactor.g = In[0].vFactor.g;
    
    Out[0].vFactor.b = In[0].vFactor.b;
    Out[1].vFactor.b = In[0].vFactor.b;
    Out[2].vFactor.b = In[0].vFactor.b;
    Out[3].vFactor.b = In[0].vFactor.b;
    
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);

    Out[0].vProjPos = Out[0].vPosition;
    Out[1].vProjPos = Out[1].vPosition;
    Out[2].vProjPos = Out[2].vPosition;
    Out[3].vProjPos = Out[3].vPosition;
    
    Triangles.Append(Out[0]);
    Triangles.Append(Out[1]);
    Triangles.Append(Out[2]);
    Triangles.RestartStrip();

    Triangles.Append(Out[0]);
    Triangles.Append(Out[2]);
    Triangles.Append(Out[3]);
    Triangles.RestartStrip();
}

struct PS_IN
{
    float4  vPosition       : SV_POSITION;
    float2  vTexcoord       : TEXCOORD0;
	float4  vProjPos	    : TEXCOORD1;
    
    float4  vFactor         : COLOR;   // R : Alpha, G : Random, B : LifeTime
};  

PS_OUT_BWOIT PS_MAIN(PS_IN In)
{
    PS_OUT_BWOIT Out = (PS_OUT_BWOIT) 0;

    vector  vColor;
    float2  vUnitUV, vCurrUV;
    
    vUnitUV.x = 1.f / g_iNumParticleRowCol.x;
    vUnitUV.y = 1.f / g_iNumParticleRowCol.y;
    
    vCurrUV.x = g_iCurrParticleRowCol.x * vUnitUV.x + In.vTexcoord.x * vUnitUV.x;
    vCurrUV.y = g_iCurrParticleRowCol.y * vUnitUV.y + In.vTexcoord.y * vUnitUV.y;
    
    vColor = ConvertLiteralSampling(LinearSampler, vCurrUV);
    
    float fAlpha    = (vColor.g + vColor.b) * 0.5f;
    float fAverage  = (vColor.g + vColor.b) / 2.f;
    
    if (0.98f <= fAverage)
    {
        fAverage = 1.f;
        vColor = float4(fAverage, fAverage, fAverage, fAlpha);
    }
    else
    {
        vColor = float4(g_vColor.r, g_vColor.g, g_vColor.b, fAlpha * 0.2f);
    }
    
    
    vColor.a = vColor.a * In.vFactor.r;
    vColor.rgb = vColor.rgb * 3.f;
    
    if (vColor.a == 0.f)
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

PS_OUT_BWOIT PS_ICICLE(PS_IN In)
{
    PS_OUT_BWOIT Out = (PS_OUT_BWOIT) 0;

    vector  vColor;
    vector vIceColor = { 0.22745f, 1.f, 0.9529411f, 1.f };
    float   vAlpha;
    float2  vUnitUV, vCurrUV;
    
    vUnitUV.x = 1.f / g_iNumParticleRowCol.x;
    vUnitUV.y = 1.f / g_iNumParticleRowCol.y;
    
    vCurrUV.x = g_iCurrParticleRowCol.x * vUnitUV.x + In.vTexcoord.x * vUnitUV.x;
    vCurrUV.y = g_iCurrParticleRowCol.y * vUnitUV.y + In.vTexcoord.y * vUnitUV.y;
    
    vColor = ConvertLiteralSampling(LinearSampler, vCurrUV);
   
    
        vAlpha = vColor.r;
    
        vColor = vIceColor; // * vColor.r;
     //   vColor.a = vAlpha - g_fTimeAcc;
    
    
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

PS_OUT_BWOIT PS_FEATHER(PS_IN In)
{
    PS_OUT_BWOIT Out = (PS_OUT_BWOIT) 0;

    vector vColor;
    float2 vUnitUV, vCurrUV;
    
    vUnitUV.x = 1.f / g_iNumParticleRowCol.x;
    vUnitUV.y = 1.f / g_iNumParticleRowCol.y;
    
    vCurrUV.x = g_iCurrParticleRowCol.x * vUnitUV.x + In.vTexcoord.x * vUnitUV.x;
    vCurrUV.y = g_iCurrParticleRowCol.y * vUnitUV.y + In.vTexcoord.y * vUnitUV.y;
    
    vColor = ConvertLiteralSampling(LinearSampler, vCurrUV);

    float fAlpha = (vColor.r + vColor.g + vColor.b) / 3;
    
    if(fAlpha <= 0.1f)
        discard;
    
    //if (vColor.g < vColor.b)
    //    vColor.g = vColor.b;
    
    //if (vColor.r < vColor.g)
    //    vColor.r = vColor.g;
  
    //vColor.g = 0.2f;
    //vColor.b = 0.2f;
    
    //vColor.r -= 0.2f;
    //vColor.g = 1 - vColor.r * 0.98f;
    //vColor.b = 1 - vColor.r * 0.98f;
    
    //if(vColor.r < 0.99f)
    //{
    //    vColor.g = 1 - fAlpha * 0.98f;
    //    vColor.b = 1 - fAlpha * 0.98f;
    //}

    
    //vColor.b = 0.f;
    //vColor.r = vColor.g;
    //vColor.g = 0.f;
    //vNewColor.a = vColor.r;
    //vColor = vNewColor;
    
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
PS_OUT_BWOIT PS_AFEATHER(PS_IN In)
{
    PS_OUT_BWOIT Out = (PS_OUT_BWOIT) 0;

    vector vColor;
    float2 vUnitUV, vCurrUV;
    
    vUnitUV.x = 1.f / g_iNumParticleRowCol.x;
    vUnitUV.y = 1.f / g_iNumParticleRowCol.y;
    
    vCurrUV.x = g_iCurrParticleRowCol.x * vUnitUV.x + In.vTexcoord.x * vUnitUV.x;
    vCurrUV.y = g_iCurrParticleRowCol.y * vUnitUV.y + In.vTexcoord.y * vUnitUV.y;
    
    vColor = ConvertLiteralSampling(LinearSampler, vCurrUV);

    float fAlpha = (vColor.r + vColor.g + vColor.b) / 3;
    
    if (g_fTimeAcc >0.f)
    {
    vColor.g -= g_fTimeAcc;
    vColor.b -= g_fTimeAcc/2.f;
    vColor.a -= g_fTimeAcc/2.f;
    }
    
    if (fAlpha <= 0.1f)
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

PS_OUT_BWOIT PS_SHIELDBREAK(PS_IN In)
{
    PS_OUT_BWOIT Out = (PS_OUT_BWOIT) 0;

    vector vColor;
    float2 vUnitUV, vCurrUV;
    
    vUnitUV.x = 1.f / g_iNumParticleRowCol.x;
    vUnitUV.y = 1.f / g_iNumParticleRowCol.y;
    
    vCurrUV.x = g_iCurrParticleRowCol.x * vUnitUV.x + In.vTexcoord.x * vUnitUV.x;
    vCurrUV.y = g_iCurrParticleRowCol.y * vUnitUV.y + In.vTexcoord.y * vUnitUV.y;
    
    vColor = ConvertLiteralSampling(LinearSampler, vCurrUV);
    
    float fAlpha = (vColor.g + vColor.b) * 0.5;

    vColor = float4(g_vColor.r, g_vColor.g, g_vColor.b, fAlpha);
    
    vColor.a = vColor.a * In.vFactor.r;
    
    if (vColor.a == 0.f)
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

PS_OUT_BWOIT PS_Shard(PS_IN In)
{
    PS_OUT_BWOIT Out = (PS_OUT_BWOIT) 0;

    vector vColor;
    float2 vUnitUV, vCurrUV;
    
    vUnitUV.x = 1.f / g_iNumParticleRowCol.x;
    vUnitUV.y = 1.f / g_iNumParticleRowCol.y;
    
    vCurrUV.x = g_iCurrParticleRowCol.x * vUnitUV.x + In.vTexcoord.x * vUnitUV.x;
    vCurrUV.y = g_iCurrParticleRowCol.y * vUnitUV.y + In.vTexcoord.y * vUnitUV.y;
    
    vColor = ConvertLiteralSampling(LinearSampler, vCurrUV);
    
    //float fAlpha = (vColor.g + vColor.r)*1.5f;
    float fAlpha = vColor.r;

    vColor = float4(g_vColor.r, g_vColor.g, g_vColor.b, fAlpha);
    
    //vColor.a = vColor.a * In.vFactor.r;
    
    if (vColor.a == 0.f)
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

PS_OUT_BWOIT PS_BRIGHTSHARD(PS_IN In)
{
    PS_OUT_BWOIT Out = (PS_OUT_BWOIT) 0;

    vector vColor;
    float2 vUnitUV, vCurrUV;
    
    vUnitUV.x = 1.f / g_iNumParticleRowCol.x;
    vUnitUV.y = 1.f / g_iNumParticleRowCol.y;
    
    vCurrUV.x = g_iCurrParticleRowCol.x * vUnitUV.x + In.vTexcoord.x * vUnitUV.x;
    vCurrUV.y = g_iCurrParticleRowCol.y * vUnitUV.y + In.vTexcoord.y * vUnitUV.y;
    
    vColor = ConvertLiteralSampling(LinearSampler, vCurrUV);
    
    //float fAlpha = (vColor.g + vColor.r)*1.5f;
    float fAlpha = vColor.r;

    vColor = float4(g_vColor.r, g_vColor.g, g_vColor.b, fAlpha);
    
    vColor.rgb += g_fTimeAcc;

    //vColor.a = vColor.a * In.vFactor.r;
    
    if (vColor.a == 0.f)
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

PS_OUT_BWOIT PS_SOLID_DEBRIS(PS_IN In)
{
    PS_OUT_BWOIT Out = (PS_OUT_BWOIT) 0;

    vector vColor;
    float2 vUnitUV, vCurrUV;
    
    vUnitUV.x = 1.f / g_iNumParticleRowCol.x;
    vUnitUV.y = 1.f / g_iNumParticleRowCol.y;
    
    vCurrUV.x = g_iCurrParticleRowCol.x * vUnitUV.x + In.vTexcoord.x * vUnitUV.x;
    vCurrUV.y = g_iCurrParticleRowCol.y * vUnitUV.y + In.vTexcoord.y * vUnitUV.y;
    
    vColor = ConvertLiteralSampling(LinearSampler, vCurrUV);
    
    //float fAlpha = (vColor.g + vColor.r)*1.5f;
    float fAlpha = vColor.g;

    vColor = float4(g_vColor.r, g_vColor.g, g_vColor.b, fAlpha);
    
    //vColor.a = vColor.a * In.vFactor.r;
    
    if (vColor.a == 0.f)
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

PS_OUT_BWOIT PS_IMPACT_GLOW(PS_IN In)
{
    PS_OUT_BWOIT Out = (PS_OUT_BWOIT) 0;

    vector vColor;
    float2 vUnitUV, vCurrUV;
    
    vUnitUV.x = 1.f / g_iNumParticleRowCol.x;
    vUnitUV.y = 1.f / g_iNumParticleRowCol.y;
    
    vCurrUV.x = g_iCurrParticleRowCol.x * vUnitUV.x + In.vTexcoord.x * vUnitUV.x;
    vCurrUV.y = g_iCurrParticleRowCol.y * vUnitUV.y + In.vTexcoord.y * vUnitUV.y;
    
    vColor = ConvertLiteralSampling(LinearSampler, vCurrUV);
    
    //float fAlpha = (vColor.g + vColor.r)*1.5f;

    float fAlpha = vColor.r - g_fTimeAcc;

    vColor = float4(g_vColor.r, g_vColor.g, g_vColor.b, fAlpha);
    
    if (vColor.a <= 0.01f)
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

PS_OUT_BWOIT PS_TORNADO(PS_IN In)
{
    PS_OUT_BWOIT Out = (PS_OUT_BWOIT) 0;

    vector vColor;
    float2 vUnitUV, vCurrUV;
    
    vUnitUV.x = 1.f / g_iNumParticleRowCol.x;
    vUnitUV.y = 1.f / g_iNumParticleRowCol.y;
    
    vCurrUV.x = g_iCurrParticleRowCol.x * vUnitUV.x + In.vTexcoord.x * vUnitUV.x;
    vCurrUV.y = g_iCurrParticleRowCol.y * vUnitUV.y + In.vTexcoord.y * vUnitUV.y;
    
    vColor = ConvertLiteralSampling(LinearSampler, vCurrUV);
    if (vColor.a <= 0.01f)
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

PS_OUT_BWOIT PS_DUST(PS_IN In)
{
    PS_OUT_BWOIT Out = (PS_OUT_BWOIT) 0;

    vector vColor;
    vector vMtrlDiffuse;
    float2 vUnitUV, vCurrUV;
    
    vUnitUV.x = 1.f / g_iNumParticleRowCol.x;
    vUnitUV.y = 1.f / g_iNumParticleRowCol.y;
    
    vCurrUV.x = g_iCurrParticleRowCol.x * vUnitUV.x + In.vTexcoord.x * vUnitUV.x;
    vCurrUV.y = g_iCurrParticleRowCol.y * vUnitUV.y + In.vTexcoord.y * vUnitUV.y;
    
    vMtrlDiffuse = ConvertLiteralSampling(LinearSampler, vCurrUV);
    vColor = float4(g_vColor.r, g_vColor.g, g_vColor.b, 1.f);
    
    //vColor = { 148.f / 255.f, 124.f / 255.f, 93.f / 255.f, 1.f };
    
    if (vMtrlDiffuse.b > 0.9f)
    {
        vColor.r *= 1.05f;
        vColor.g *= 1.05f;
        vColor.b *= 1.05f;
    }
        
    float fGrayScale = (vMtrlDiffuse.x + vMtrlDiffuse.y * 2.f + vMtrlDiffuse.z) / 3.f;
    
    fGrayScale -= (g_fTimeAcc * 2.f);
    
    if (fGrayScale < 0.1f)
        discard;

    
    vColor.a = fGrayScale * 1.5f;
    
    
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

PS_OUT_BWOIT PS_Heat_Smoke(PS_IN In)
{
    PS_OUT_BWOIT Out = (PS_OUT_BWOIT) 0;

    vector vColor;
    vector vFlameColor = { 1.f,1.f,0.5f,0.7f};
    float2 vUnitUV, vCurrUV;
    
    
    vUnitUV.x = 1.f / g_iNumParticleRowCol.x;
    vUnitUV.y = 1.f / g_iNumParticleRowCol.y;
    
    vCurrUV.x = g_iCurrParticleRowCol.x * vUnitUV.x + In.vTexcoord.x * vUnitUV.x;
    vCurrUV.y = g_iCurrParticleRowCol.y * vUnitUV.y + In.vTexcoord.y * vUnitUV.y;
    
    vColor = ConvertLiteralSampling(LinearSampler, vCurrUV);
    if (vColor.r <= 0.1f)
        discard;
        
    vFlameColor.z -= g_fTimeAcc*3.f;
    if (vFlameColor.z <= 0.f)
    {
        vFlameColor.y -= (g_fTimeAcc * 3.f - 0.5f);
    }
    if (vFlameColor.y <= 0.f)
    {
        vFlameColor.x -= (g_fTimeAcc * 3.f - 1.5f);
        vFlameColor.w -= (g_fTimeAcc);

    }
    
    vColor.a = vColor.r;
    vColor = vFlameColor;
    vColor.a = vFlameColor.a-0.2f;
    
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

PS_OUT_BWOIT PS_Blink_Shrink(PS_IN In)
{
    PS_OUT_BWOIT Out = (PS_OUT_BWOIT) 0;

    vector vColor;
    float2 vUnitUV, vCurrUV;
        
    vUnitUV.x = 1.f / g_iNumParticleRowCol.x;
    vUnitUV.y = 1.f / g_iNumParticleRowCol.y;
    
    vCurrUV.x = g_iCurrParticleRowCol.x * vUnitUV.x + In.vTexcoord.x * vUnitUV.x;
    vCurrUV.y = g_iCurrParticleRowCol.y * vUnitUV.y + In.vTexcoord.y * vUnitUV.y;
    
    vColor = ConvertLiteralSampling(LinearClampSampler, vCurrUV.xy * (1 + g_fTimeAcc / 4.f));
    
    vector vWhite = { 183.f / 255.f, 248.f / 255.f, 248.f / 255.f, 1.f };
    
    if ((vColor.r + vColor.g + vColor.b)/3.f <= 0.1f)
        discard;
    
    //if(vColor.r > vColor.g)
    //    vWhite.a = vColor.r;
    //else
    //    vWhite.a = vColor.g;
    
    //if (vColor.r > vColor.b)
    //    vWhite.a = vColor.r;
    //else
    //    vWhite.a = vColor.b;
    
    //if (vColor.g > vColor.b)
    //    vWhite.a = vColor.g;
    //else
    //    vWhite.a = vColor.b;
    //아래처럼 하면 간소화
    vWhite.a = max(max(vColor.r, vColor.g), vColor.b);
    
    vColor = vWhite;
    vColor.a += cos(g_fTimeAcc*7.f)*0.5;
    vColor.r += cos(g_fTimeAcc*7.f)*0.2;


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

PS_OUT_BWOIT PS_Star_Color(PS_IN In)
{
    PS_OUT_BWOIT Out = (PS_OUT_BWOIT) 0;

    vector vColor;
    float2 vUnitUV, vCurrUV;
    
    vUnitUV.x = 1.f / g_iNumParticleRowCol.x;
    vUnitUV.y = 1.f / g_iNumParticleRowCol.y;
    
    vCurrUV.x = g_iCurrParticleRowCol.x * vUnitUV.x + In.vTexcoord.x * vUnitUV.x;
    vCurrUV.y = g_iCurrParticleRowCol.y * vUnitUV.y + In.vTexcoord.y * vUnitUV.y;
    
    
    
    vColor = g_Textures[16].Sample(LinearClampSampler, vCurrUV.xy * (1+ g_fTimeAcc / 4.f));

    vector vRampTexture = g_Textures[17].Sample(LinearSampler, float2(g_vColor.r +g_fTimeAcc / 10.f, 0.05f));
    
    //71 46 6 37 60 128
    
    if ((vColor.r + vColor.g + vColor.b) / 3.f <= 0.1f)
        discard;
    
    vRampTexture.a = max(max(vColor.r, vColor.g), vColor.b)+0.2f;
    
    vColor = vRampTexture;

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

PS_OUT_BWOIT PS_Trailing_Line(PS_IN In)
{
    PS_OUT_BWOIT Out = (PS_OUT_BWOIT) 0;

    vector vColor;
    float2 vUnitUV, vCurrUV;
    
    if (In.vTexcoord.x < (1.f-g_fTimeAcc/1.5f))
        discard;
    
    vUnitUV.x = 1.f / g_iNumParticleRowCol.x;
    vUnitUV.y = 1.f / g_iNumParticleRowCol.y;
    
    vCurrUV.x = g_iCurrParticleRowCol.x * vUnitUV.x + In.vTexcoord.x * vUnitUV.x;
    vCurrUV.y = g_iCurrParticleRowCol.y * vUnitUV.y + In.vTexcoord.y * vUnitUV.y;
    
    vColor = ConvertLiteralSampling(LinearClampSampler, vCurrUV.xy );
    
    vector vWhite = { 1.f,1.f,1.f, 1.f };
    
    if ((vColor.r + vColor.g + vColor.b) / 3.f <= 0.1f)
        discard;
    
    vWhite.a = vColor.b;
    
    vColor = vWhite;
    //vColor.a += cos(g_fTimeAcc / 5.f)*0.5;
    
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

PS_OUT_BWOIT PS_Water_Fog(PS_IN In)
{
    PS_OUT_BWOIT Out = (PS_OUT_BWOIT) 0;
    
    float fPosX;
    
    vector vMaskTexture = g_Textures[19].Sample(LinearSampler, In.vTexcoord);

    vector vMtrlDiffuse = g_Textures[17].Sample(LinearSampler, float2(g_fTimeAcc / 10.f, 0.1f));
    
    float fGrayScale = (vMaskTexture.r + vMaskTexture.g + vMaskTexture.b) / 3.f;
    
    if (fGrayScale < 0.05f)
        discard;
   
    //if(vMaskTexture.g>0.6f)
    //    vMaskTexture.g = 0.6f;
    
    vMtrlDiffuse.a = vMaskTexture.r + vMaskTexture.g;
    
    if (vMtrlDiffuse.a > 0.6f)
        vMtrlDiffuse.a = 0.6f;
    
    if (vMtrlDiffuse.a < 0.05f)
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

PS_OUT_BWOIT PS_EFFIGY_PAPERBURN(PS_IN In)
{
    PS_OUT_BWOIT Out = (PS_OUT_BWOIT) 0;
    
    if (In.vFactor.b > 1.f)
        discard;
    
    float2 vUnitUV, vCurrUV, vRampUV;
    
    vUnitUV.x = 1.f / g_iNumParticleRowCol.x;
    vUnitUV.y = 1.f / g_iNumParticleRowCol.y;
    
    vCurrUV.x = g_iCurrParticleRowCol.x * vUnitUV.x + In.vTexcoord.x * vUnitUV.x;
    vCurrUV.y = g_iCurrParticleRowCol.y * vUnitUV.y + In.vTexcoord.y * vUnitUV.y;
    
    vRampUV = float2(1.f - In.vFactor.b * 2.f, In.vTexcoord.y);
    
    vector vMtrlDiffuse     = ConvertLiteralSampling(LinearSampler, vCurrUV);
    vector vRampDiffuse     = g_Textures[22].Sample(LinearClampSampler, vRampUV);
    vector vDissolveDiffuse = g_Textures[23].Sample(LinearSampler, vCurrUV);
    
    // vector vColor       = float4(vMtrlDiffuse.g, vMtrlDiffuse.r, vMtrlDiffuse.b, 1.f) * vRampDiffuse;
    vector vColor       = float4(3.f, 0.75f, 0.5f, 1.f) * vRampDiffuse;
    vColor              = saturate(vColor * vMtrlDiffuse.g);
    
    float  fAlphaFactor = min(2.f - In.vFactor.b * 2.f, 1.f);
    vColor.a            = vColor.a * fAlphaFactor;
    
    /* DIssolve */
    if (vDissolveDiffuse.r < In.vFactor.b)
        discard;
    
    float fLength = In.vFactor.b * 2.f + 0.05f;
    if (vDissolveDiffuse.r <= fLength)
    {
        float fAtt = sin(radians(90.f * (fLength - vDissolveDiffuse.r) * (1 / 0.2f)));
        Out.vDiffuse.rgb = saturate(Out.vDiffuse.rgb + float3(3.f, 0.75f, 0.5f) * fAtt);
    }

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

PS_OUT_BWOIT PS_EFFIGY_SPARK(PS_IN In)
{
    PS_OUT_BWOIT Out = (PS_OUT_BWOIT) 0;
    
    float2 vUnitUV, vCurrUV;
    
    vUnitUV.x = 1.f / g_iNumParticleRowCol.x;
    vUnitUV.y = 1.f / g_iNumParticleRowCol.y;
    
    vCurrUV.x = g_iCurrParticleRowCol.x * vUnitUV.x + In.vTexcoord.x * vUnitUV.x;
    vCurrUV.y = g_iCurrParticleRowCol.y * vUnitUV.y + In.vTexcoord.y * vUnitUV.y;
    
    vector vMtrlDiffuse = ConvertLiteralSampling(LinearSampler, vCurrUV);
    vector vColor       = float4(vMtrlDiffuse.rgb * float3(1.0000f, 0.5039f, 0.0000f), 1.f) * 2.f;
    vColor.a            = (vMtrlDiffuse.r + vMtrlDiffuse.g + vMtrlDiffuse.b) / 3.f;

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

PS_OUT_BWOIT PS_PHOENIX_SMOKE(PS_IN In)
{
    PS_OUT_BWOIT Out = (PS_OUT_BWOIT) 0;

    vector vColor;
    float2 vUnitUV, vCurrUV;
    
    vUnitUV.x = 1.f / g_iNumParticleRowCol.x;
    vUnitUV.y = 1.f / g_iNumParticleRowCol.y;
    
    vCurrUV.x = g_iCurrParticleRowCol.x * vUnitUV.x + In.vTexcoord.x * vUnitUV.x;
    vCurrUV.y = g_iCurrParticleRowCol.y * vUnitUV.y + In.vTexcoord.y * vUnitUV.y;
    
    vColor = ConvertLiteralSampling(LinearSampler, vCurrUV);
    if (vColor.a <= 0.01f)
        discard;
    
    vColor.rgb = saturate(vColor.rgb * 1.25f);

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

PS_OUT_BWOIT PS_DVL_STARRAIN(PS_IN In)
{
    PS_OUT_BWOIT Out = (PS_OUT_BWOIT) 0;
    
    if (In.vFactor.b > 1.f)
        discard;
    
    float2 vUnitUV, vCurrUV, vRampUV;
    
    vUnitUV.x = 1.f / g_iNumParticleRowCol.x;
    vUnitUV.y = 1.f / g_iNumParticleRowCol.y;
    
    vCurrUV.x = g_iCurrParticleRowCol.x * vUnitUV.x + In.vTexcoord.x * vUnitUV.x;
    vCurrUV.y = g_iCurrParticleRowCol.y * vUnitUV.y + In.vTexcoord.y * vUnitUV.y;
    
    vector vMtrlDiffuse = ConvertLiteralSampling(LinearSampler, vCurrUV);
    
    vector vColor       = float4(0.7000f, 0.8598f, 0.9569f, (vMtrlDiffuse.r + vMtrlDiffuse.g + vMtrlDiffuse.b) / 3.f);

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

technique11 DefaultTechnique
{
    pass Particle
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_WeightedBlended, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = compile gs_5_0 GS_MAIN();
        PixelShader     = compile ps_5_0 PS_MAIN();
    }

    pass Particle_Icicle
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_WeightedBlended, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = compile gs_5_0 GS_MAIN();
        PixelShader     = compile ps_5_0 PS_ICICLE();
    }

    pass Particle_Feather
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_WeightedBlended, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_FEATHER();
    }

    pass Particle_AFeather
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_WeightedBlended, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_AFEATHER();
    }

    pass Particle_ShieldBreak
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_WeightedBlended, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_SHIELDBREAK();
    }

    pass Particle_Shard
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_WeightedBlended, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_Shard();
    }

    pass Particle_Bright_Shard
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_WeightedBlended, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_BRIGHTSHARD();
    }

    pass Particle_Solid_Debris
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_WeightedBlended, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_SOLID_DEBRIS();
    }

    pass Particle_Impact_Glow
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_WeightedBlended, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_IMPACT_GLOW();
    }

    pass Particle_Tornado
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_WeightedBlended, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_TORNADO();
    }

    pass Particle_Dust
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_WeightedBlended, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_DUST();
    }

    pass Particle_Heat_Smoke
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_WeightedBlended, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_Heat_Smoke();
    }

    pass Particle_Blink_Shrink
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_WeightedBlended, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_Blink_Shrink();
    }

    pass Particle_Star_Colored
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_WeightedBlended, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_Star_Color();
    }

    pass Particle_Trailing_Line
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_WeightedBlended, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_Trailing_Line();
    }

    pass Particle_Water_Fog
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_WeightedBlended, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_Water_Fog();
    }

    pass Particle_Effigy_PaperBurn
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_WeightedBlended, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = compile gs_5_0 GS_MAIN();
        PixelShader     = compile ps_5_0 PS_EFFIGY_PAPERBURN();
    }

    pass Particle_Effigy_Spark
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_WeightedBlended, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = compile gs_5_0 GS_MAIN();
        PixelShader     = compile ps_5_0 PS_EFFIGY_SPARK();
    }

    pass PhoenixHitSmokePass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_WeightedBlended, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = compile gs_5_0 GS_MAIN();
        PixelShader     = compile ps_5_0 PS_PHOENIX_SMOKE();
    }

    pass DvalinStarRainPass
    {
        SetRasterizerState(RS_NonCullSolid);
        SetDepthStencilState(DSS_CompareNWrite, 0);
        SetBlendState(BS_WeightedBlended, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader    = compile vs_5_0 VS_MAIN();
        GeometryShader  = compile gs_5_0 GS_MAIN();
        PixelShader     = compile ps_5_0 PS_DVL_STARRAIN();
    }
}