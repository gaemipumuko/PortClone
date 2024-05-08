#pragma once

#include "Base.h"

BEGIN(Engine)

class CGraphic_Device final : public CBase
{
private:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;

private:
	_uint	m_iWinCX = { };
	_uint	m_iWinCY = { };

public:
	HRESULT Initialize(HWND hWnd, GRAPHIC_DESC::WINMODE eWinMode,
		_uint iWinCX, _uint iWinCY, _Inout_ ID3D11Device** ppDevice,
		_Inout_ ID3D11DeviceContext** ppContext);

	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();

	HRESULT Present();

	void Set_PlayVideo(_bool bPlay) { m_bIsPlayingVideo = bPlay; }

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	IDXGISwapChain*				m_pSwapChain = { nullptr };

	ID3D11RenderTargetView*		m_pBackBufferRTV = { nullptr };
	ID3D11DepthStencilView*		m_pDepthStencilView = { nullptr };

	_bool						m_bIsPlayingVideo = { false };

private:
	HRESULT Ready_SwapChain(HWND hWnd, GRAPHIC_DESC::WINMODE eWinMode, _uint iWinCX, _uint iWinCY);
	HRESULT Ready_BackBufferRenderTargetView();
	HRESULT Ready_DepthStencilRenderTargetView(_uint iWinCX, _uint iWinCY);

public:
	static CGraphic_Device* Create(HWND hWnd, GRAPHIC_DESC::WINMODE eWinMode,
		_uint iWinCX, _uint iWinCY, _Inout_ ID3D11Device** ppDevice,
		_Inout_ ID3D11DeviceContext** ppDeviceContextOut);
	virtual void			Free() override;
};

END