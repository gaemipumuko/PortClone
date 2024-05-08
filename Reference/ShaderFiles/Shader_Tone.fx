#include "ES_ShaderDefine.fxh"

Texture2D           InputTexture1       : register(t0); // HDR
Texture2D           InputTexture2       : register(t1); // LUT
Texture2D           InputTexture3       : register(t2); // BLM
Texture2D           InputTexture4       : register(t3); // WORLD
RWTexture2D<float4> OutputTexture       : register(u1);

static const float e = 2.71828;

float W_f(float x, float e0, float e1)
{
    if (x <= e0)
        return 0;
    if (x >= e1)
        return 1;
    float a = (x - e0) / (e1 - e0);
    return a * a * (3 - 2 * a);
}

float H_f(float x, float e0, float e1)
{
    if (x <= e0)
        return 0;
    if (x >= e1)
        return 1;
    return (x - e0) / (e1 - e0);
}

float GranTurismoTonemapper(float x)
{
    float P = 1;
    float a = 1;
    float m = 0.22;
    float l = 0.4;
    float c = 1.33;
    float b = 0;
    float l0 = (P - m) * l / a;
    float L0 = m - m / a;
    float L1 = m + (1 - m) / a;
    float L_x = m + a * (x - m);
    float T_x = m * pow(x / m, c) + b;
    float S0 = m + l0;
    float S1 = m + a * l0;
    float C2 = a * P / (P - S1);
    float S_x = P - (P - S1) * pow(e, -(C2 * (x - S0) / P));
    float w0_x = 1 - W_f(x, 0, m);
    float w2_x = H_f(x, m + l0, m + l0);
    float w1_x = 1 - w0_x - w2_x;
    float f_x = T_x * w0_x + L_x * w1_x + S_x * w2_x;
    return f_x;
}

float3 GetLutColor(float3 vInColor)
{
    float4 vLutUV;
    float2 vLutSize = float2(0.00390625, 0.0625); // 1 / float2(256, 16);
    
    vInColor    = saturate(vInColor) * 15.0;
    vLutUV.w    = floor(vInColor.b);
    vLutUV.xy   = (vInColor.rg + 0.5) * vLutSize;
    vLutUV.x    += vLutUV.w * vLutSize.y;
    vLutUV.z    = vLutUV.x + vLutSize.y;
    
    return lerp(InputTexture2.SampleLevel(CSLinearSampler, vLutUV.xy, 0).rgb, InputTexture2.SampleLevel(CSLinearSampler, vLutUV.zy, 0).rgb, vInColor.b - vLutUV.w);
}

VS_OUT_POSTEX VS_TONE(VS_IN_POSITION In)
{
    VS_OUT_POSTEX Out = (VS_OUT_POSTEX) 0;
    
    float4x4    OrthoWVPMatrix;
    
    OrthoWVPMatrix[0] = g_vOrthoWVPRight;
    OrthoWVPMatrix[1] = g_vOrthoWVPUp;
    OrthoWVPMatrix[2] = g_vOrthoWVPLook;
    OrthoWVPMatrix[3] = g_vOrthoWVPPosition;
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), OrthoWVPMatrix);
    Out.vTexcoord.x = (Out.vPosition.x + 1.f) / 2.f;
    Out.vTexcoord.y = abs(Out.vPosition.y - 1.f) / 2.f;
    
    return Out;
}

float4 PS_TONE(PS_IN_POSTEX In) : SV_Target
{
    vector vHDRDesc     = InputTexture1.Sample(CSAnisotropicSampler, In.vTexcoord);
    vector vBloomDesc   = InputTexture3.Sample(CSPointSampler, In.vTexcoord);
    vector vWorldDesc   = InputTexture4.Sample(CSPointSampler, In.vTexcoord);
    if (vHDRDesc.r == 1.f && vHDRDesc.g == 0.f && vHDRDesc.b == 1.f)
        discard;
    
    /* Color Grading - Exposure */
    vHDRDesc.rgb = vHDRDesc.rgb * pow(2.f, g_fExposure);
    
    /* Color Grading - Contrast */
    vHDRDesc.rgb = ((vHDRDesc.rgb - 0.5f) * g_fContrast) + 0.5f;
    
    /* Color Grading - Filter */
    vHDRDesc.rgb = vHDRDesc.rgb * g_vColorFilter;
    
    /* Color Grading - Saturation */
    float   fLuminance = max(dot(vHDRDesc.rgb, LUM_FACTOR), 0.0001f);
    vHDRDesc.rgb = lerp(fLuminance, vHDRDesc.rgb, g_fSaturation);

    /* Tone Mapping */
    float4 vToneColor = float4(
        GranTurismoTonemapper(vHDRDesc.r),
        GranTurismoTonemapper(vHDRDesc.g),
        GranTurismoTonemapper(vHDRDesc.b),
        1.f
    );

    /* EOTF */
    vToneColor.rgb      = Inv_EOTF_Normalization(vToneColor.rgb);
    
    /* Bloom */
    vToneColor.rgb      = saturate(vToneColor.rgb + (g_fBloomScale * vBloomDesc.rgb * saturate(vWorldDesc.a + 0.2f)));
    
    /* LUT */
    float4 vFinalColor  = float4((vWorldDesc.a != 0.f) ? GetLutColor(vToneColor.rgb) : vToneColor.rgb, 1.f);

    return vFinalColor;
}