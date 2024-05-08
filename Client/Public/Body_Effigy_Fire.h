#pragma once

#include "Client_Defines.h"
#include "Part_Body.h"

BEGIN(Client)

class CBody_Effigy_Fire final : public CPart_Body
{
public:
	enum EFFIGY_WEAPON { EFFIGY_WEAPON_BODY, EFFIGY_WEAPON_CLAP, EFFIGY_WEAPON_END };

private:
	CBody_Effigy_Fire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Effigy_Fire(const CBody_Effigy_Fire& rhs);
	virtual ~CBody_Effigy_Fire() = default;

private:
	class CTransform*	m_pAuraTransform = { nullptr };
	class CTransform*	m_pMagicCircleTransform = { nullptr };
	class CTransform*	m_pParticleTransform = { nullptr };

private:
	CCollider*			m_pWeaponColliderCom[EFFIGY_WEAPON_END] = {};
	_int				m_iWeaponColliderID[EFFIGY_WEAPON_END] = {};

private:
	_float4		m_vTrailOriginalColor = { 0.f, 0.f, 0.f, 1.f };
	_float4		m_vTrailAttenuationColor = { 1.f, 1.f, 1.f, 1.f };

private:
	virtual HRESULT	Invalidate_Animation(_float fTimeDelta);
	virtual HRESULT Bind_ShaderResources_Trail();

private:
	HRESULT Add_Component(void* pArg);
	
private:
	// 뼈에 따라서 부모의 회전값을 적용해주기 위한 변수들
	ROTATION_DIRECTION  m_eRotationDir = { ROT_END };
	_int m_iRRPrevState = -1;		// RR : Return Rotation
	_float	m_fAccumulateRRRadian = { 0.f };
	_bool	m_bReturnRotation = { FALSE };
	_bool	m_bInitRRAnimation = { FALSE };

	// 카메라 컷신
	_int	m_iCameraSceneIndex = 0;

	// 공격 제어
	_int  m_iAnimCount = 0;

	// 첫 프레임 실행
	_bool m_bIsFirstFrame = TRUE;

	// 패턴 제어
	_bool m_bHasMoved = FALSE;

	// 큐브 투사체
	_float m_fFire_Cube_Timer = 0.f;
	_bool m_bAlready_Shoot_Fire_Cube = FALSE;
	_int m_iAttack_Cube_Index = 0;
	vector<_vector>Fire_Cube_Attack_Pos = {};
	vector<_int>Fire_Cube_Mesh_Index = {};

	// 기지개 피기 파티클
	_bool m_bAlertParticle = { FALSE };

	// 점화 패턴
	_bool m_bIsIgnite = FALSE;
	_bool m_bIsIgniteSuccess = FALSE;

	// 스네이크 불던지기
	_int  m_iFireBallStep = { 0 };
	_int  m_iFireBallIndex = { 0 };

	// 주먹쥐고 내리치기
	_bool m_bPunchEff = { FALSE };
	_bool m_bPunchTrail = { FALSE };

	// 변형공격 이펙트
	_bool m_bTrsFishEff[2] = { FALSE };
	_bool m_bTrsBombEff	   = { FALSE };

	// 박수 짝짝
	_bool m_bClapEff = { FALSE };

	// 파티클 트레일
	_bool				m_bParticleMove = { FALSE };

	list<wstring>		m_ParticleTags;
	list<wstring>		m_ParticleMoveTags;

public:
	void	Set_HasMoved(_bool _bHasMoved) { m_bHasMoved = _bHasMoved; }
	void	Set_IsIgnite(_bool _IsIgnite) { m_bIsIgnite = _IsIgnite; }
	void	Set_IsIgniteSuccess(_bool _IsIgniteSuccess) { m_bIsIgniteSuccess = _IsIgniteSuccess; }

	_int	Get_WeaponCollider(_int iIndex) { return m_iWeaponColliderID[iIndex]; }

	HRESULT	Spawn_Fire_Cube();

private:
	HRESULT	Invalidate_Particle();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow(_float4x4* pLightViewProj, _int iNumArray) override;

public:
	virtual void	Release_ColliderID() override;

public:
	static CBody_Effigy_Fire*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END