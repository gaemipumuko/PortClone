#pragma once

#include "Client_Defines.h"
#include "Part_Body.h"

BEGIN(Client)

class CBody_Abyss_Mage final : public CPart_Body
{
private:
	CBody_Abyss_Mage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Abyss_Mage(const CBody_Abyss_Mage& rhs);
	virtual ~CBody_Abyss_Mage() = default;

private:
	virtual HRESULT	Invalidate_Animation(_float fTimeDelta);

private:
	HRESULT Add_Component(void* pArg);

private:
	CCollider* m_pSphereAttackColliderCom = { nullptr };

private:
	// 뼈에 따라서 부모의 회전값을 적용해주기 위한 변수들
	ROTATION_DIRECTION  m_eRotationDir = { ROT_END };
	_int m_iRRPrevState = -1;		// RR : Return Rotation
	_float	m_fAccumulateRRRadian = { 0.f };
	_bool	m_bReturnRotation = { FALSE };
	_bool	m_bInitRRAnimation = { FALSE };

	_bool	m_bAlreadyNormalAtk = FALSE;
	_bool	m_bAlreadySpecialAtk = FALSE;

	_int	m_iCycleCount = 0;
	CREATUREID	m_eCreatureID = CREATURE_END;

	_bool	m_bIsFirstFrame = TRUE;

	_bool	m_bRangeAttack = FALSE;
	_int	m_iAttackCount = 0;
	_float	m_fAttackDelay = 0.f;

	_bool	m_bSPASound = { FALSE };
	_bool	m_bDeadSound = { FALSE };
	_bool	m_bBlinkSound = { FALSE };

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void	Release_ColliderID() override;

public:
	static CBody_Abyss_Mage*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END