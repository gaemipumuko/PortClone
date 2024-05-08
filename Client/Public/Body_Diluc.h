#pragma once

#include "Client_Defines.h"
#include "Part_Body.h"

BEGIN(Client)

class CBody_Diluc final : public CPart_Body
{
private:
	CBody_Diluc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Diluc(const CBody_Diluc& rhs);
	virtual ~CBody_Diluc() = default;

private:
	virtual HRESULT	Invalidate_Animation(_float fTimeDelta);

private:
	HRESULT Add_Component(void* pArg);

private:
	_bool		m_bCameraControl = { TRUE };
	_int		m_iCameraScene = { 0 };

	_bool		m_bUpdateRootTranslation = { TRUE };

	/* °­°ø */
	_bool		m_isResetHitCollision = { FALSE };

	_bool		m_bChargedReady = { FALSE };
	_bool		m_bAttack = { FALSE };

	_bool		m_bChargedAttackStart = { FALSE };
	_bool		m_bChargedAttackEnd = { FALSE };

	_float		m_fPosition = {};

	_float		m_fExtraAttackTime = {};

	_bool		m_bReset_SkillTime = { FALSE };

	_bool		m_bInitEffect = { FALSE };

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
	static CBody_Diluc*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END