/* SMPTE 2084 Factor (https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=7291452) */
static float m1 = 0.1593017578125f;
static float m2 = 78.84375f;
static float c1 = 0.8359375f;
static float c2 = 18.8515625f;
static float c3 = 18.6875f;

float3  EOTF_Linearization(float3 vColor)
{
    float3  vLinearColor = max(pow(vColor, 1.f / m2) - c1, 0);
    vLinearColor = vLinearColor / (c2 - c3 * pow(vColor, 1 / m2));
    vLinearColor = pow(vLinearColor, 1 / m1);

    return vLinearColor;
}

float3  Inv_EOTF_Normalization(float3 vColor)
{
    float3  vNormalColor = c1 + c2 * pow(vColor, m1);
    vNormalColor = vNormalColor / (1 + c3 * pow(vColor, m1));
    vNormalColor = pow(vNormalColor, m2);

    return vNormalColor;
}

float3  Conv_NormColor(float3 vColor)
{
    return float3(vColor.rgb / 255.f);
}
