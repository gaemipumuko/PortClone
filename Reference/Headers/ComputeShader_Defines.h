#pragma once

#include "Base.h"

typedef struct tagRenderTarget
{
	_bool						bUse = { FALSE };

	_bool						bUAV = { };
	_uint						iWidth = { };
	_uint						iHeight = { };
	DXGI_FORMAT					eFormat = { };
	_uint						iMipLevels = { };

	ID3D11Texture2D*			pTexture2D = { nullptr };
	ID3D11RenderTargetView*		pRTV = { nullptr };
	ID3D11ShaderResourceView*	pSRV = { nullptr };
	ID3D11UnorderedAccessView*	pUAV = { nullptr };
}RT_DESC;

typedef struct tagVertexShader
{
	ID3DBlob*				pBlob = { nullptr };
	ID3D11VertexShader*		pVertexShader = { nullptr };
}VS_DESC;

typedef struct tagPixelShader
{
	ID3DBlob*				pBlob = { nullptr };
	ID3D11PixelShader*		pPixelShader = { nullptr };
}PS_DESC;

typedef struct tagComputeShader
{
	ID3DBlob*				pBlob = { nullptr };
	ID3D11ComputeShader*	pComputeShader = { nullptr };
}CS_DESC;

typedef struct tagWindow
{
	float       fWINCX;
	float       fWINCY;
} CB_WINDOW;

typedef struct tagBloomBuffer
{
	float       fBloomScale;
	float       fBloomThreshold;
} CB_BLOOM;

typedef struct tagToneWVP
{
	XMFLOAT4	fToneWVPRight;
	XMFLOAT4	fToneWVPUp;
	XMFLOAT4	fToneWVPLook;
	XMFLOAT4	fToneWVPPosition;
} CB_TONEWVP;