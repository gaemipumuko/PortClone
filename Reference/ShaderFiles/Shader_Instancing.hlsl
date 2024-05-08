
#include    "Engine_Shader_Defines.hlsli"

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    
    row_major float4x4 matWorld : WORLD;
    uint iInstanceID : SV_InstanceID;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    vector vPosition = mul(float4(In.vPosition, 1.f), In.matWorld);

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    vPosition = mul(vPosition, matWVP);
    
    Out.vPosition = vPosition;
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vector(In.vPosition, 1.f), In.matWorld);
    Out.vProjPos = Out.vPosition;
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix)).xyz;
    Out.vBinormal = normalize(cross(Out.vNormal.xyz, Out.vTangent));
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vWorld : SV_TARGET2;
    vector vFactor : SV_TARGET5;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);

    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal.xyz);

    vNormal = mul(normalize(vNormal), WorldMatrix);

    if (vMtrlDiffuse.a < 0.1f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vWorld = In.vWorldPos;
    Out.vFactor = float4(1.f, 0.f, 0.f, 0.f);
    
    return Out;
}


/* Shadow */
VS_OUT_GEOPOSITION VS_SHADOW(VS_IN In)
{
    VS_OUT_GEOPOSITION Out = (VS_OUT_GEOPOSITION) 0;
	
    Out.vPosition = mul(vector(In.vPosition, 1.f), In.matWorld);
    //Out.vPosition = mul(Out.vPosition, g_WorldMatrix);

    return Out;
}

struct GS_IN
{
    float4 vPosition : POSITION;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    uint iRTIndex : SV_RenderTargetArrayIndex;
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
    pass InstancingPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass ShadowPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_SHADOW();
        GeometryShader = compile gs_5_0 GS_SHADOW();
        PixelShader = compile ps_5_0 PS_SHADOW();
    }
}