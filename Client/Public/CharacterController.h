#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CTransform;
END

BEGIN(Client)

class CCharacterController final : public CBase
{
private:
	CCharacterController();
	virtual ~CCharacterController() = default;

public:
	HRESULT Initialize(_float fHeight, _float fRadius, _float fStepOffset, _float4 vPosition, CTransform* pTransformCom);
	
public:
	void Movement(_float fTimeDelta, _float4 vDirection, _float fSpeed = 1.0f);
	void Apply_Gravity(_float fTimeDelta);

public:
	void Ready_Jump(_float fForce);
	_bool isRise();

	_bool Get_InAir() { return m_binAir; }
	_bool Get_InGround() { return m_binGround; }
	_bool Get_InFly() { return m_bisFly; }

	void Set_InAir(_bool binAir) { m_binAir = binAir; }
	void Set_InGround(_bool binGround) { m_binGround = binGround; }
	void Set_Position(_float4 vPosition);
	void Set_MovementSpeed(_float fMovementSpeed) { m_fMovementSpeedControl = fMovementSpeed; };
	void Add_Direction(_vector Direction);
	_bool CanFly();

	_float Compute_Height();
	HRESULT WakeUp_Controller();
	HRESULT Sleep_Controller();

private:
	CGameInstance* m_pGameInstance = { nullptr };

private:
	CTransform* m_pTransformCom = { nullptr };
	PxController* m_pController = { nullptr };
	PxFilterData m_tControllerFilterData = {};

private:
	_float m_fHeight;
	_float m_fRadius;
	PxVec3 m_PxGravity = PxVec3(0.0f, -9.8f, 0.0f);
	_bool m_bSleep = false;

private:
	_bool m_binAir = false;
	_bool m_binGround = false;
	_bool m_bisFly = false;
	_bool m_bisClimb = false;
	_bool m_bisJump = false;

	_float m_fForce = 2.5f;
	_float m_fJumpPower = 0.0f;
	_float m_fJumpTime = 0.0f;

	_float m_fMovementSpeedControl = { 1.f };

public:
	static CCharacterController* Create(_float fHeight, _float fRadius, _float fStepOffset/* Default : 0.5f */, _float4 vPosition, CTransform* pTransformCom);
	virtual void Free() override;
};

END
