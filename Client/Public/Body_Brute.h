#pragma once

#include "Client_Defines.h"
#include "Part_Body.h"

BEGIN(Client)

class CBody_Brute final : public CPart_Body
{
private:
	CBody_Brute(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Brute(const CBody_Brute& rhs);
	virtual ~CBody_Brute() = default;

private:
	virtual HRESULT	Invalidate_Animation(_float fTimeDelta);

private:
	HRESULT Add_Component(void* pArg);

private:
	// 뼈에 따라서 부모의 회전값을 적용해주기 위한 변수들
	ROTATION_DIRECTION  m_eRotationDir = { ROT_END };
	_int m_iRRPrevState = -1;		// RR : Return Rotation
	_float	m_fAccumulateRRRadian = { 0.f };
	_bool	m_bReturnRotation = { FALSE };
	_bool	m_bInitRRAnimation = { FALSE };

	// 공격 제어
	_bool m_bIsSwing_Type_3 = FALSE;
	_int  m_iAttackIndex = 0;
	
	_bool m_bFirstFrame = TRUE;

	_bool bSound_1 = FALSE;
	_bool bSound_2 = FALSE;
	_bool bSound_3 = FALSE;

public:
	void Set_IsSwing_Type_3(_bool _IsSwingType_3) { m_bIsSwing_Type_3 = _IsSwingType_3; }

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
	static CBody_Brute*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END