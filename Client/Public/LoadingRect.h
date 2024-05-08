#pragma once

#include "Client_Defines.h"
#include "Ortho.h"

BEGIN(Client)

class CLoadingRect : public COrtho
{
private:
	CLoadingRect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLoadingRect(const CLoadingRect& rhs);
	virtual ~CLoadingRect() = default;

private:
	_float4x4	m_BackMatrix = { };	/* BackGround */
	_float4x4	m_IconMatrix = { };	/* Main Icon */
	_float4x4	m_LBarMatrix = { };	/* Loading Bar */

private:
	_float		m_fLerpRatio = { };			/* 현재 보간 시간 */

	_float		m_fRealLoadRatio = { };
	_float2		m_fPrevRealRatio = { };
	_float2		m_vCurrLoadRatio = { };

	_float4		m_vColor1 = { 0.4f, 0.4f, 0.4f, 1.f };	/* Gage */
	_float4		m_vColor2 = { 0.96f, 0.96f, 0.96f, 1.f };	/* Base */

public:
	_bool	Is_Finished() { return (m_vCurrLoadRatio.x >= m_vCurrLoadRatio.y); }
	void	Setup_LoadRatio(_float fLoadLatio) { m_fRealLoadRatio = fLoadLatio; }

private:
	HRESULT Add_Component(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CLoadingRect*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END