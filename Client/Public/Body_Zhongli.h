#pragma once

#include "Client_Defines.h"
#include "Part_Body.h"

BEGIN(Client)

class CBody_Zhongli final : public CPart_Body
{
private:
	CBody_Zhongli(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Zhongli(const CBody_Zhongli& rhs);
	virtual ~CBody_Zhongli() = default;

private:
	virtual HRESULT	Invalidate_Animation(_float fTimeDelta);

private:
	HRESULT Add_Component(void* pArg);

private:
	_vector		m_vPrevRootTranslation = { };
	_bool		m_bCameraControl = { TRUE };
	_int		m_iCameraScene = { 0 };

	_float		m_fFlyMargin = { 2.5f }; // 낙공 모션

	_bool		m_bLerpIdle = { FALSE };

	/* 강공, 평타 */
	_bool		m_bChargedReady = { FALSE };
	_bool		m_bAttack = { FALSE };

	_bool		m_bFallingSpear = { FALSE };

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Release_ColliderID() override;

public:
	static CBody_Zhongli*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END