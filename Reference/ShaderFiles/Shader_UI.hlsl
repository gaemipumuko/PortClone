#include    "Engine_Shader_Defines.hlsli"
bool g_bInput;
bool g_bEnlarge;

float g_fStamina;
float g_fBlinkTimer;
float g_fCoolDown;
float g_fRate;

float2 g_UVScale, g_UVPosition;


float3 RGBtoHSV(float3 rgb)
{
    float minComponent = min(min(rgb.r, rgb.g), rgb.b);
    float maxComponent = max(max(rgb.r, rgb.g), rgb.b);
    float delta = maxComponent - minComponent;

    float hue = 0.0;
    float saturation = (maxComponent > 0.0) ? (delta / maxComponent) : 0.0;
    float value = maxComponent;

    if (saturation > 0.0)
    {
        if (rgb.r >= maxComponent)
            hue = (rgb.g - rgb.b) / delta;
        else if (rgb.g >= maxComponent)
            hue = 2.0 + (rgb.b - rgb.r) / delta;
        else
            hue = 4.0 + (rgb.r - rgb.g) / delta;

        hue *= 60.0;

        if (hue < 0.0)
            hue += 360.0;
    }

    return float3(hue, saturation, value);
}

float3 HSVtoRGB(float3 hsv)
{
    float C = hsv.y * hsv.z;
    float X = C * (1.0 - abs(fmod(hsv.x / 60.0, 2.0) - 1.0));
    float m = hsv.z - C;

    float3 rgb = 0.0;

    if (hsv.x >= 0.0 && hsv.x < 60.0)
        rgb = float3(C, X, 0.0);
    else if (hsv.x >= 60.0 && hsv.x < 120.0)
        rgb = float3(X, C, 0.0);
    else if (hsv.x >= 120.0 && hsv.x < 180.0)
        rgb = float3(0.0, C, X);
    else if (hsv.x >= 180.0 && hsv.x < 240.0)
        rgb = float3(0.0, X, C);
    else if (hsv.x >= 240.0 && hsv.x < 300.0)
        rgb = float3(X, 0.0, C);
    else if (hsv.x >= 300.0 && hsv.x < 360.0)
        rgb = float3(C, 0.0, X);

    return (rgb + m);
}

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vCover = { 0.3f, 0.3f, 0.3f, 0.2f };
    vector vColor;
    vColor = g_Texture.Sample(LinearSampler, g_UVPosition + In.vTexcoord * g_UVScale);

    
    if (g_bInput == false)
    {
        vColor -= vCover;
    }
    
    Out.vColor = vColor;
    
        return Out;
}

PS_OUT PS_Def_Color(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
      

    Out.vColor = g_Texture.Sample(LinearSampler, g_UVPosition + In.vTexcoord * g_UVScale);
    
    if (Out.vColor.a < 0.1f)
        discard;
    
     Out.vColor.rgb = g_vColor.rgb;
    Out.vColor.a += g_vColor.a;
    
    if (g_UVScale.x<=0.f || g_UVScale.y<=0.f)
    {
        Out.vColor.a = 0.f;
        
    }
    
        return Out;
}

PS_OUT PS_Dual_Tex(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiff, vBackGround;
    
    vDiff = g_BrushTexture.Sample(LinearSampler, In.vTexcoord);
    vBackGround = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (vDiff.a >0.03f)
    {
        Out.vColor = vDiff;
    }
    else
    {
        Out.vColor = vBackGround;

    }
    
    Out.vColor.a = vBackGround.a;
    
    return Out;
}

PS_OUT PS_Stamina(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    
    Out.vColor = g_Texture.Sample(LinearSampler, g_UVPosition + In.vTexcoord * g_UVScale);

    if (g_fStamina <  0.15f)
        Out.vColor -= vector(0.f, 1.f, 0.5f, .3f);

    //if (g_fStamina < 1.f)
    //{
    //    float2 fCenter = float2(0.f, 0.5f);
    //    float2 fUVPos = In.vTexcoord - fCenter;
    //    float fAngle = atan2(fUVPos.x, fUVPos.y);
    //    float fCoolRate = saturate((fAngle + 3.141592) / (2 * 3.141592));

    //    if (fCoolRate < (g_fStamina))
    //    {
    //    }
    //}
    return Out;
}

PS_OUT PS_Ref_Red(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
      
    vector vTexColor;
    vector vResultColor;
    float3 vResultRGB;
    float3 TexRGB;
    
    vTexColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    TexRGB = g_vColor.rgb;
    
    float3 vHSVColor = RGBtoHSV(TexRGB);

    float fGrayScale;
    fGrayScale = (vTexColor.r + vTexColor.g + vTexColor.b) / 3.f;
    vHSVColor.y = 1.1f - fGrayScale; // 1.f인데 1.1로 보정
    
    vResultRGB = HSVtoRGB(vHSVColor);
    vResultColor.rgb = vResultRGB;
    vResultColor.a = fGrayScale;
    
    Out.vColor = vResultColor;
    
    return Out;
}


PS_OUT PS_MAIN_SCROLL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (In.vPosition.y < 100.f)
        discard;
    
    vector vCover = { 0.3f, 0.3f, 0.3f, 0.2f };
    vector vColor;
    vColor = g_Texture.Sample(LinearSampler, g_UVPosition + In.vTexcoord * g_UVScale);

    
    
    Out.vColor = vColor;
    
    return Out;
}

PS_OUT PS_ELEMENT_OHD(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiff = g_DiffuseTexture.Sample(LinearSampler, g_UVPosition + In.vTexcoord * g_UVScale);
    
    vector vMask = g_Texture.Sample(LinearSampler, g_UVPosition + In.vTexcoord * g_UVScale);
    vector vColor = g_Texture2.Sample(LinearSampler, g_UVPosition + In.vTexcoord * g_UVScale);
    
    if (g_bEnlarge == true)
    {
        
        
        vColor.a = vMask.g - g_fTimeAcc;
        if (vMask.g<0.5f)
        vColor.a = vMask.r*0.9 - g_fTimeAcc;

        
        vColor.r += g_fTimeAcc / 2.f;
        vColor.g += g_fTimeAcc / 2.f;
        vColor.b += g_fTimeAcc / 2.f;

    }
    else
    {
        vColor.a = vMask.r;
    }
    
    Out.vColor = vColor;
    
    return Out;
}

PS_OUT PS_BACKGROUND(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

   // vector vBackgroundColor = float4(1.0f, 0.0f, 0.0f, 0.3f);

    vector startColor = float4(1.0f, 0.0f, 0.0f, 0.1f); 
    vector endColor = float4(1.f, 0.f, 0.f, 1.f);

    float gradientFactor = In.vTexcoord.y;

    vector gradientColor = lerp(startColor, endColor, gradientFactor);

    vector vTextureColor = g_Texture.Sample(LinearSampler, g_UVPosition + In.vTexcoord * g_UVScale);

    float minValue = 0.0f;
    float maxValue = 0.56f;

    bool isBlack = (vTextureColor.r >= minValue && vTextureColor.r <= maxValue &&
        vTextureColor.g >= minValue && vTextureColor.g <= maxValue &&
        vTextureColor.b >= minValue && vTextureColor.b <= maxValue);

      if (isBlack)
      {
          vector vCombinedColor = lerp(gradientColor, vTextureColor, gradientColor.a);

          if (g_bInput == false)
          {
              vCombinedColor -= gradientColor;
          }
          
          Out.vColor = vCombinedColor;
      }

      else
      {
          Out.vColor = vTextureColor;
      }

    return Out;
}

PS_OUT PS_CLOUD(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    vector vCover = { 0.3f, 0.3f, 0.3f, 0.2f };
    vector vColor = { 1.f, 0.f, 0.f, 0.1f };


    vColor = g_Texture.Sample(LinearSampler, g_UVPosition + In.vTexcoord * g_UVScale, vColor.a);

    if (g_bInput == false)
    {
        vColor -= vCover;
    }


    vColor.a *= 0.3f;

    Out.vColor = vColor;

    return Out;
}

PS_OUT PS_ZHONGLI_BACKGROUND(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    // vector vBackgroundColor = float4(1.0f, 0.0f, 0.0f, 0.3f);

    vector startColor = float4(1.0f, 1.0f, 0.0f, 0.1f);
    vector endColor = float4(1.f, 0.f, 0.f, 1.f);

    float gradientFactor = In.vTexcoord.y;

    vector gradientColor = lerp(startColor, endColor, gradientFactor);

    vector vTextureColor = g_Texture.Sample(LinearSampler, g_UVPosition + In.vTexcoord * g_UVScale);

    float minValue = 0.0f;
    float maxValue = 0.56f;

    bool isBlack = (vTextureColor.r >= minValue && vTextureColor.r <= maxValue &&
        vTextureColor.g >= minValue && vTextureColor.g <= maxValue &&
        vTextureColor.b >= minValue && vTextureColor.b <= maxValue);

    if (isBlack)
    {
        vector vCombinedColor = lerp(gradientColor, vTextureColor, gradientColor.a);

        if (g_bInput == false)
        {
            vCombinedColor -= gradientColor;
        }

        Out.vColor = vCombinedColor;
    }

    else
    {
        Out.vColor = vTextureColor;
    }

    return Out;
}

PS_OUT PS_BARBARA_BACKGROUND(PS_IN In)
{
    PS_OUT Out = (PS_OUT)0;

    // vector vBackgroundColor = float4(1.0f, 0.0f, 0.0f, 0.3f);

    vector startColor = float4(0.f, 0.0, 1.0f, 0.1f);
    vector endColor = float4(1.f, 0.f, 0.f, 1.f);

    float gradientFactor = In.vTexcoord.y;

    vector gradientColor = lerp(startColor, endColor, gradientFactor);

    vector vTextureColor = g_Texture.Sample(LinearSampler, g_UVPosition + In.vTexcoord * g_UVScale);

    float minValue = 0.0f;
    float maxValue = 0.56f;

    bool isBlack = (vTextureColor.r >= minValue && vTextureColor.r <= maxValue &&
        vTextureColor.g >= minValue && vTextureColor.g <= maxValue &&
        vTextureColor.b >= minValue && vTextureColor.b <= maxValue);

    if (isBlack)
    {
        vector vCombinedColor = lerp(gradientColor, vTextureColor, gradientColor.a);

        if (g_bInput == false)
        {
            vCombinedColor -= gradientColor;
        }

        Out.vColor = vCombinedColor;
    }

    else
    {
        Out.vColor = vTextureColor;
    }

    return Out;
}


PS_OUT PS_BURST_SKILL_ICON(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vColor = g_vColor;
    vector vCover = { 0.3f, 0.3f, 0.3f, 0.2f };
    vColor = g_Texture.Sample(LinearSampler, g_UVPosition + In.vTexcoord * g_UVScale);

    float fGrayScale = (vColor.r + vColor.g + vColor.b)/3.f;
    
    if (fGrayScale<0.1f)
        discard;
   
    if (g_fRate <1.f)
    {
        if (In.vTexcoord.y >1 -g_fRate)
        {
            vColor *= g_vColor;
        }

    }
    else
    {   
        vColor *= g_vColor;
        vColor.a += 0.2f;
    }
    
    
    Out.vColor = vColor;
    
        return Out;
}

PS_OUT PS_COOLDOWN_CIRCLE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vElementColor = g_vColor;
    
    vector vColor;
    vColor = g_Texture.Sample(LinearSampler, g_UVPosition + In.vTexcoord * g_UVScale);

    float fGrayScale = (vColor.r + vColor.g + vColor.b) / 3.f;
    
    if (fGrayScale < 0.1f)
        discard;
    
    vElementColor.a = vColor.a;
   
    if (g_fCoolDown > 0.f)
    {
        float2 fCenter = float2(0.5f, 0.5f);
        float2 fUVPos = In.vTexcoord - fCenter;
        float fAngle = 180 + degrees(atan2(fUVPos.x, fUVPos.y));
        float fCoolRate = fAngle / 360.f;
        
        if (fCoolRate > (g_fCoolDown))
        {
            discard;
        }

    }
    else
    {   
        discard;
    }
    
    
    Out.vColor = vElementColor;
    
    return Out;
}

PS_OUT PS_RED_ALPHA(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDefinedColor = g_vColor;
    
    
    vector vColor = g_Texture.Sample(LinearClampSampler, g_UVPosition + In.vTexcoord * g_UVScale);
    
    if ((g_UVPosition.x + In.vTexcoord.x * g_UVScale.x)  > 1.f)
        discard;
    
        vDefinedColor.a = vColor.r;
   
    if (vDefinedColor.a <0.1f)
        discard;
    
    Out.vColor = vDefinedColor;
    
    return Out;
}

PS_OUT PS_Scale_Discard(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
      
    if (In.vTexcoord.x > g_UVScale.x)
        discard;
    if (In.vTexcoord.y > g_UVScale.y)
        discard;
    Out.vColor = g_Texture.Sample(LinearSampler, g_UVPosition + In.vTexcoord);
    
    if (Out.vColor.a < 0.1f)
        discard;
    
    Out.vColor.rgb = g_vColor.rgb;
    Out.vColor.a += g_vColor.a;
    
    if (g_UVScale.x <= 0.f || g_UVScale.y <= 0.f)
    {
        Out.vColor.a = 0.f;
        
    }
    
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass DefinedColorPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Def_Color();
    }
    pass DualTexturePass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Dual_Tex();
    }
    pass StaminaRenderPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Stamina();
    }
    pass RedReference
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Ref_Red();
    }
    pass ItemPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SCROLL();
    }
    pass Element_OHD_Pass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_ELEMENT_OHD();
    }
    pass BackGroundPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BACKGROUND();
    }
    pass CloudPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_CLOUD();
    }
    pass ZhongliBackGroundPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_ZHONGLI_BACKGROUND();
    }
    pass BarbaraBackGroundPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BARBARA_BACKGROUND();
    }
    pass Skill_Icon_Pass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BURST_SKILL_ICON();
    }
    pass Cooldown_Circle
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_COOLDOWN_CIRCLE();
    }
    pass Red_Alpha
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RED_ALPHA();
    }
    pass Scale_Discard
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_Scale_Discard();
    }
}