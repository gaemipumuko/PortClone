#include    "Engine_Shader_Defines.hlsli"

VS_OUT_POSITION VS_MAIN(VS_IN_POSITION In)
{
    VS_OUT_POSITION Out = (VS_OUT_POSITION) 0;

	vector	vPosition = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;

	return Out;
}

PS_OUT_DIFFUSE PS_MAIN(PS_IN_POSITION In)
{
    PS_OUT_DIFFUSE Out = (PS_OUT_DIFFUSE) 0;

    Out.vDiffuse = g_vColor;
	
	return Out;
}

technique11 DefaultTechnique
{
	pass Cell
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader	= compile vs_5_0 VS_MAIN();
        GeometryShader  = NULL;
		PixelShader		= compile ps_5_0 PS_MAIN();
	}
}