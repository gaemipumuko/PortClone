#pragma once

#include "Client_Defines.h"
#include "Part_Body.h"

BEGIN(Client)

class CBody_Raiden final : public CPart_Body
{
private:
	CBody_Raiden(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Raiden(const CBody_Raiden& rhs);
	virtual ~CBody_Raiden() = default;

private:
	virtual HRESULT	Invalidate_Animation(_float fTimeDelta);

private:
	HRESULT Add_Component(void* pArg);

private:
	_bool		m_bCameraControl = { TRUE };
	_int		m_iCameraScene = { 0 };

	_bool		m_bUpdateRootTranslation = { TRUE };

	/* 강공 */
	_bool		m_isResetHitCollision = { FALSE };

	_bool		m_bChargedReady = { FALSE };
	_bool		m_bAttack = { FALSE };

	_bool		m_bChargedAttackStart = { FALSE };
	_bool		m_bChargedAttackEnd = { FALSE };

	_float		m_fPosition = {};

	_float		m_fExtraAttackTime = {};

	/* 원소스킬 */
	_int		m_iInitES = {};

public:
	void	Call_Func();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Release_ColliderID() override;

public:
	static CBody_Raiden*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END