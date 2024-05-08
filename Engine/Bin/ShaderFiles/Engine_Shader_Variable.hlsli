matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix			g_ViewMatrixInv, g_ProjMatrixInv;

float4x4		g_LightViewProj[3];

vector			g_vColor = vector(0.f, 0.f, 0.f, 1.f);
vector			g_vUVRate = vector(0.f, 0.f, 1.f, 1.f);
vector			g_vBrushPos = float4(30.0f, 0.f, 10.f, 1.f);
vector			g_vCamPosition = float4(0.f, 0.f, 0.f, 1.f);

vector			g_vColor1 = vector(0.f, 0.f, 0.f, 1.f);
vector			g_vColor2 = vector(0.f, 0.f, 0.f, 1.f);

Texture2D		g_Texture;
Texture2D		g_Texture2;
Texture2D		g_Textures[32];
Texture2D		g_DiffuseTexture;
Texture2D       g_HighDynamicTexture;

Texture2D		g_DepthTexture;
Texture2D		g_WorldTexture;
Texture2D		g_NormalTexture;
Texture2D		g_LightMapTexture;

Texture2D		g_MaskTexture;
Texture2D		g_BrushTexture;

Texture2DArray  g_TextureArray;

int             g_iIndex = -1;

float			g_fFar = 100.f;
float			g_fBrushRange = 20.f;
float           g_fTexArrayIndex = 1.f;
float           g_fTimeAcc;
float           g_ColorRate;
float           g_fRampIndex = -1.f;
float           g_fFloorHeight;

float2          g_vUVRate2 = float2(0.f, 0.f);
float3          g_vUVRate3 = float3(0.f, 0.f, 0.f);

float2          g_vUVScaleRate = float2(0.f, 0.f);
float2          g_vUVSpinRate = float2(0.f, 0.f);

bool			g_bFaceMesh;
bool			g_bNormalTexture;
bool			g_bLightMapTexture;
bool            g_bBlue;

bool            g_bLocalAnim;
float4x4        g_PureWorldMatrix;