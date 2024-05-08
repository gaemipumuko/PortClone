/* Sampler State */
sampler PointSampler = sampler_state
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = wrap;
	AddressV = wrap;
};

sampler LinearSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = wrap;
	AddressV = wrap;
};

sampler LinearClampSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = clamp;
	AddressV = clamp;
};

sampler WrapClampSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = clamp;
};

sampler AnisotropicSampler = sampler_state
{
    Filter = ANISOTROPIC;
    MaxAnisotropy = 16;
    AddressU = wrap;
    AddressV = wrap;
};

/* Raterizer State */
RasterizerState RS_Default
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

RasterizerState RS_WireFrame
{
    FillMode = WireFrame;
    CullMode = Back;
    FrontCounterClockwise = false;
};

RasterizerState RS_CWSolid
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = true;
};

RasterizerState RS_CWWire
{
	FillMode = WireFrame;
	CullMode = Back;
	FrontCounterClockwise = true;
};

RasterizerState RS_NonCullSolid
{
	FillMode = Solid;
	CullMode = None;
	FrontCounterClockwise = false;
};

RasterizerState RS_SkyBox
{
	FillMode = Solid;
	CullMode = Front;
	FrontCounterClockwise = false;
};

/* Depth_Stencil State */
DepthStencilState DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};

DepthStencilState DSS_CompareNWrite
{
    DepthEnable = true;
    DepthFunc = less_equal;
	DepthWriteMask = zero;
};

DepthStencilState DSS_NoneWrite
{
	DepthEnable = false;
	DepthFunc = less_equal;
};

DepthStencilState DSS_None
{
	DepthEnable = false;
	DepthWriteMask = zero;
};

/* Blend State */
BlendState BS_Default
{
	BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
	BlendEnable[0] = true;

	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
	BlendOp = Add;
};

BlendState BS_AdditionalDiffuse
{
    BlendEnable[0]	= true;
    SrcBlend[0]		= Src_Alpha;
    DestBlend[0]	= Inv_Src_Alpha;
    BlendOp[0]		= Add;

    BlendEnable[1] = false;
};

BlendState BS_One
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;

	SrcBlend = One;
	DestBlend = One;
	BlendOp = Add;
};

BlendState BS_WeightedBlended
{
	BlendEnable[0] = true;
	SrcBlend[0] = One;
	DestBlend[0] = One;
	BlendOp[0] = Add;
    SrcBlendAlpha[0] = One;
    DestBlendAlpha[0] = One;
    BlendOpAlpha[0] = Add;

	BlendEnable[1] = true;
	SrcBlend[1] = One;
	DestBlend[1] = One;
	BlendOp[1] = Add;

	BlendEnable[2] = true;
	SrcBlend[2] = Zero;
	DestBlend[2] = Inv_Src_Alpha;
	BlendOp[2] = Add;
};

BlendState BS_EffectDeferred
{
	BlendEnable[0] = true;

	SrcBlend = Inv_Src_Alpha;
	DestBlend = Src_Alpha;
	BlendOp = Add;
};