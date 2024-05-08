#pragma once

#ifndef _DEBUGNUI

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CRenderer;
class CGameInstance;
END

BEGIN(Client)

class CDebugTool final : public CBase
{
private:
	CDebugTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CDebugTool() = default;

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	CRenderer*				m_pRenderer = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };

private: /* For.Pivot */
	_float4	m_vPivotScale = { 1.f, 1.f, 1.f, 1.f };
	_float4	m_vPivotRotation = { 0.f, 180.f, 0.f, 0.f };
	_float4	m_vPivotTranslation = { 0.f, 0.f, 0.f, 1.f };

private: /* For.Free Camera */
	_float	m_fSpeedPerSec = { };
	_float	m_fRotationPerSec = { };

private: /* For.Cursor Mode */
	_bool	m_bFixCursor = { };
	_bool	m_bClipCursor = { };

private: /* For. BloomCB */
	_float	m_fBloomScale = { 1.f };
	_float	m_fBloomThreshold = { 1.f };

private: /* For. ColorGradeCB */
	_float3 m_vColorFilter = { 1.2f, 0.8f, 0.8f };
	_float  m_fSaturation = { 1.f };
	_float  m_fExposure = { 1.484f };
	_float  m_fContrast = { 1.00232875f };

private: /* ImGui Window */
	void	Main_Window();
	void	ConsoleLog_Window();

	void	MainWin_Player();
	void	MainWin_Debug();
	void	MainWin_Render();

public:
	HRESULT	Initialize();
	HRESULT	Render();

public:
	static CDebugTool*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void		Free() override;
};

END

#endif