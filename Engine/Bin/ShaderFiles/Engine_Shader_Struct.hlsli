/* VS Input */
struct VS_IN_POSITION
{
    float3 vPosition    : POSITION;
};

struct VS_IN_POSTEX
{
    float3 vPosition    : POSITION;
    float2 vTexcoord    : TEXCOORD0;
};

struct VS_IN_POSTEX3
{
    float3 vPosition    : POSITION;
    float3 vTexcoord    : TEXCOORD0;
};

struct VS_IN_POSNORTEX
{
    float3 vPosition    : POSITION;
    float3 vNormal      : NORMAL;
    float2 vTexcoord    : TEXCOORD0;
};

struct VS_IN_MESH
{
    float3 vPosition    : POSITION;
    float3 vNormal      : NORMAL;
    float2 vTexcoord    : TEXCOORD0;
    float3 vTangent     : TANGENT;
};

struct VS_IN_ANIMMESH
{
	float3 vPosition	: POSITION;
	float3 vNormal		: NORMAL;
	float2 vTexcoord	: TEXCOORD0;
	float3 vTangent	    : TANGENT;
	uint4  vBlendIndices : BLENDINDEX;
	float4 vBlendWeights : BLENDWEIGHT;
};

/* VS Output */
struct VS_OUT_POSITION
{
    float4 vPosition    : SV_POSITION;
};

struct VS_OUT_GEOPOSITION
{
    float4 vPosition    : POSITION;
};

struct VS_OUT_POSTEX
{
    float4 vPosition    : SV_POSITION;
    float2 vTexcoord    : TEXCOORD0;
};

struct VS_OUT_POSTEX3
{
    float4 vPosition    : SV_POSITION;
    float3 vTexcoord    : TEXCOORD0;
};

struct VS_OUT_POSTEX_4
{
    float4 vPosition    : SV_POSITION;
    float2 vTexcoord0   : TEXCOORD0;
    float2 vTexcoord1   : TEXCOORD1;
    float2 vTexcoord2   : TEXCOORD2;
    float2 vTexcoord3   : TEXCOORD3;
};

struct VS_OUT_POSTEXPRJ
{
    float4 vPosition    : SV_POSITION;
    float2 vTexcoord    : TEXCOORD0;
	float4 vProjPos	    : TEXCOORD1;
};

struct VS_OUT_POSNORTEX
{
    float4 vPosition    : SV_POSITION;
    float4 vNormal      : NORMAL;
    float2 vTexcoord    : TEXCOORD0;
    float4 vWorldPos    : TEXCOORD1;
    float4 vProjPos     : TEXCOORD2;
};

struct VS_OUT_MESH
{
    float4 vPosition    : SV_POSITION;
    float4 vNormal      : NORMAL;
    float2 vTexcoord    : TEXCOORD0;
    float4 vWorldPos    : TEXCOORD1;
    float4 vProjPos     : TEXCOORD2;
    float3 vTangent     : TANGENT;
    float3 vBinormal    : BINORMAL;
};

/* PS Input */
struct PS_IN_POSITION
{
    float4 vPosition    : SV_POSITION;
};

struct PS_IN_TEXCOD
{
    float2 vTexcoord    : TEXCOORD0;
};

struct PS_IN_POSTEX
{
    float4 vPosition    : SV_POSITION;
    float2 vTexcoord    : TEXCOORD0;
};

struct PS_IN_POSTEX3
{
    float4 vPosition    : SV_POSITION;
    float3 vTexcoord    : TEXCOORD0;
};

struct PS_IN_POSTEX_4
{
    float4 vPosition    : SV_POSITION;
    float2 vTexcoord0   : TEXCOORD0;
    float2 vTexcoord1   : TEXCOORD1;
    float2 vTexcoord2   : TEXCOORD2;
    float2 vTexcoord3   : TEXCOORD3;
};

struct PS_IN_POSTEXPRJ
{
    float4 vPosition    : SV_POSITION;
    float2 vTexcoord    : TEXCOORD0;
    float4 vProjPos     : TEXCOORD1;
};

struct PS_IN_POSNORTEX
{
    float4 vPosition    : SV_POSITION;
    float4 vNormal      : NORMAL;
    float2 vTexcoord    : TEXCOORD0;
    float4 vWorldPos    : TEXCOORD1;
    float4 vProjPos     : TEXCOORD2;
};

struct PS_IN_MESH
{
    float4 vPosition    : SV_POSITION;
    float4 vNormal      : NORMAL;
    float2 vTexcoord    : TEXCOORD0;
    float4 vWorldPos    : TEXCOORD1;
    float4 vProjPos     : TEXCOORD2;
    float3 vTangent     : TANGENT;
    float3 vBinormal    : BINORMAL;
};

/* PS Output */
struct PS_OUT_DIFFUSE
{
    float4  vDiffuse    : SV_TARGET0;
};

struct PS_OUT_DEFERRED
{
    float4  vDiffuse    : SV_TARGET0;
    float4  vDepth      : SV_TARGET1;
};

struct PS_OUT_BWOIT
{
    vector vDiffuse     : SV_TARGET0;
    vector vFactor      : SV_TARGET1;
    vector vWeight      : SV_TARGET2;
};

struct PS_OUT_SCENE_DN
{
    vector vDiffuse     : SV_TARGET0;
    vector vNormal      : SV_TARGET1;
};

struct PS_OUT_SCENE_DC
{
    vector vDiffuse     : SV_TARGET0;
    vector vCShader     : SV_TARGET7;
};

struct PS_OUT_SCENE_DNF
{
    vector vDiffuse     : SV_TARGET0;
    vector vNormal      : SV_TARGET1;
    vector vFactor      : SV_TARGET5;
};

struct PS_OUT_SCENE_DNW
{
    vector vDiffuse     : SV_TARGET0;
    vector vNormal      : SV_TARGET1;
    vector vWorld       : SV_TARGET2;
};

struct PS_OUT_SCENE_DWF
{
    vector vDiffuse     : SV_TARGET0;
    vector vWorld       : SV_TARGET2;
    vector vFactor      : SV_TARGET5;
};

struct PS_OUT_SCENE_DNWFC
{
    vector vDiffuse     : SV_TARGET0;
    vector vNormal      : SV_TARGET1;
    vector vWorld       : SV_TARGET2;
    vector vFactor      : SV_TARGET5;
    vector vCShader     : SV_TARGET7;
};

struct PS_OUT_SCENE_DNWF
{
    vector vDiffuse     : SV_TARGET0;
    vector vNormal      : SV_TARGET1;
    vector vWorld       : SV_TARGET2;
    vector vFactor      : SV_TARGET5;
};

struct PS_OUT_SCENE_DNWL
{
    vector vDiffuse     : SV_TARGET0;
    vector vNormal      : SV_TARGET1;
    vector vWorld       : SV_TARGET2;
    vector vLightMap    : SV_TARGET4;
};

struct PS_OUT_SCENE_DNWLF
{
    vector vDiffuse     : SV_TARGET0;
    vector vNormal      : SV_TARGET1;
    vector vWorld       : SV_TARGET2;
    vector vLightMap    : SV_TARGET4;
    vector vFactor      : SV_TARGET5;
};

struct PS_OUT_SCENE_DNWLFF
{
    vector vDiffuse     : SV_TARGET0;
    vector vNormal      : SV_TARGET1;
    vector vWorld       : SV_TARGET2;
    vector vLightMap    : SV_TARGET4;
    vector vFactor      : SV_TARGET5;
    vector vFace        : SV_TARGET6;
};

struct PS_OUT_SCENE_TERRAIN
{
    vector vDiffuse     : SV_TARGET0;
    vector vNormal      : SV_TARGET1;
    vector vWorld       : SV_TARGET2;
    vector vShadowWorld : SV_TARGET3;
    vector vFactor      : SV_TARGET5;
};

struct PS_OUT_SCENE
{
    vector vDiffuse     : SV_TARGET0;
    vector vNormal      : SV_TARGET1;
    vector vWorld       : SV_TARGET2;
    vector vShadowWorld : SV_TARGET3;
    vector vLightMap    : SV_TARGET4;
};