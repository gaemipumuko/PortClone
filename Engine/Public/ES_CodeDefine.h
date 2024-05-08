#pragma once

#include "Base.h"

typedef struct tagRenderTarget
{
	_bool						bUse = { FALSE };

	_bool						bUAV = { };
	_uint						iWidth = { };
	_uint						iHeight = { };
	DXGI_FORMAT					eFormat = { };

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

typedef struct tagColorGradeBuffer
{
	XMFLOAT3	vColorFilter;	/* 필터 */
	float		fSaturation;	/* 채도 */
	float       fExposure;		/* 노출 */
	float       fContrast;		/* 대비 */
} CB_COLORGRADE;

typedef struct tagOrthoWVP
{
	XMFLOAT4	vOrthoWVPRight;
	XMFLOAT4	vOrthoWVPUp;
	XMFLOAT4	vOrthoWVPLook;
	XMFLOAT4	vOrthoWVPPosition;
} CB_ORTHOWVP;

typedef struct tagOutline
{
	XMFLOAT4X4	SceneViewMatrix;
	XMFLOAT4X4	SceneProjMatrix;

	float		fSceneFar;
} CB_OUTLINE;