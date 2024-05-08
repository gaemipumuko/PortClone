#include    "Engine_Shader_Defines.hlsli"

TextureCube g_CubeTexture;

VS_OUT_POSTEX3 VS_MAIN(VS_IN_POSTEX3 In)
{
    VS_OUT_POSTEX3 Out = (VS_OUT_POSTEX3) 0;
	
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

PS_OUT_DIFFUSE PS_MAIN(PS_IN_POSTEX3 In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;
    Out.vDiffuse = g_CubeTexture.Sample(LinearSampler, In.vTexcoord);

    Out.vDiffuse.rgb = EOTF_Linearization(Out.vDiffuse.rgb);

    return Out;
}

technique11 DefaultTechnique
{
    pass Cube
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}