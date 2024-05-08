#pragma once

#include "Client_Defines.h"
#include "Creature.h"

#include "Element_OHD.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CCollider;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffigy_Fire final : public CCreature
{
private:
	CEffigy_Fire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffigy_Fire(const CEffigy_Fire& rhs);
	virtual ~CEffigy_Fire() = default;

private:
	typedef struct tagPartySlots
	{
		class CPartObject*	pPartySlot = { };
	} PARTY_SLOTS;
	_byte				m_byPartyIndex = { };	/* 현재 선택된 캐릭터 인덱스 */
	vector<PARTY_SLOTS>	m_Parts;

public:
	enum PART { PART_BODY, PART_STATUS, PART_END };
	enum EFFIGY_ATTACK_TYPE {
		EFFIGY_ATTACK_CLAP,
		EFFIGY_ATTACK_ROLLING,	
		EFFIGY_ATTACK_TRANSFORM,
		EFFIGY_ATTACK_RANGED_CUBE,
		EFFIGY_ATTACK_PUNCH,
		EFFIGY_ATTACK_SNAKE,
		EFFIGY_ATTACK_SELF_EXPLOSION,
		EFFIGY_ATTACK_PILLAR,
		EFFIGY_ATTACK_END
	};

private:
	// 설정용 변수
	CCharacterController*	pController = {};
	_vector					m_vInitPos = { };						// 초기 위치

	// 스탯 
	_float				m_fHp = 90000.f;
	_float				m_fMaxHp = 90000.f;

	// 불 보호막 관련 함수
	_float				m_fFireShield_Hp = 9000.f;
	_float				m_fFireShield_Recharge_Rate_Max = 9000.f;
	_float				m_fFireShield_Recharge_Rate_Current = 0.f;

	_bool				m_bIsFireShieldActive = FALSE;				// 불 보호막이 활성화 여부
	_bool				m_bIsRechargeCrystal_Destroy = FALSE;		// 불 충전 크리스탈 파괴 여부

	// 패턴 관련 변수
	_float				m_fPatternTimer = 0.f;						// 패턴 타이머
	_bool				m_bIsFirstFrame = TRUE;						// 첫프레임에만 실행되도록 관리하기 위한 변수
	_bool				m_bIsEnemyDetected = FALSE;					// 적 인식 여부
	_bool				m_bIsCoreExposure = FALSE;					// 코어 노출되었다.

	vector<_int>		nearPatternOrder = { 1, 2, 3 };
	vector<_int>		farPatternOrder = { 5, 6, 7 };
	_vector				m_vPlayerPos = {};

	// 공격 패턴 관련 변수
	EFFIGY_ATTACK_TYPE	m_eAttack_Type = EFFIGY_ATTACK_END;			// 어떤 타입의 스윙 공격인지 판별용 변수 
	_bool				m_bIsAttacking = FALSE;						// 공격중인지 아닌지 판별용 변수
	_bool				m_PrevPatternWasAttack = FALSE;				// 이전 패턴이 공격 패턴이었는지 판별용 변수
	_int				m_iAttackIndex = 0;							// 공격의 몇번쨰인지 판별용 변수

	// 이동 관련 변수
	_vector				m_vRandomPos = {};							// 이동한 랜덤한 위치 저장할 변수

	// 구르기 패턴 관련 변수
	_float				m_fLerpSpeed = 0.f;
	_bool				m_bPlayerInFront = TRUE;
	_int				m_iRollingIndex = 0;

	// 불기둥 패턴 관련 변수
	_bool				m_bIsPillarPattern = FALSE;
	_float				m_fPillarTimer = 0.f;						// 기둥패턴 지속시간 타이머
	_float				m_fPlayerPillarTimer = 0.f;					// 플레이어에게 솟아오르는 기둥 타이머
	_float				m_fRandomPillarTimer = 0.f;					// 랜덤으로 솟아오르는 기둥 타이머

	// 자폭 패턴 관련 변수
	_float				m_fSelfExplosionTimer = 0.f;

	// 사망 패턴 관련 변수
	_bool	m_bIsDying = FALSE;
	_bool	m_bIsDead = FALSE;
	_float	m_fDieTimer = 0.f;						// 패턴 타이머

	// 플레이어 공격 충돌 관련 변수
	_bool	m_bHitPlayer = TRUE;

	// 이펙트 제어 관련 변수
	_bool	m_bEffectExecuted = FALSE;

	// UI 관련 변수
	_bool	m_bAlert = { FALSE };
	class	CHp_Bar* m_pHpBar = { nullptr };
	CElement_OHD* m_pElement_OHD = { nullptr };

	// 패턴 테스트
	_int	m_iPattern = { 0 };
	_bool	m_bPatternControl = { FALSE };

	// Alert
	_bool	m_bHPAlert = { FALSE };

	_vector m_vPlayerPosss = {};

	CHANNELID m_eRollingSoundChannel = { MAXCHANNEL };

public:
	void	Set_Alert(_bool bAlert) { m_bHPAlert = bAlert; }
	
private:
	virtual HRESULT Add_Parts(void* pArg);

private:
	HRESULT	Pattern(_float fTimeDelta);			// 패턴
	HRESULT	Pattern_Pillar(_float fTimeDelta);	// 기둥 패턴

	// 공격 관련
	HRESULT Identify_Effigy_AttackType();		// 어떤 타입의 공격을 한것인가 판별
	HRESULT Effigy_Attack_Pattern();			// 공격 제어
	
	// 플레이어 위치 판별
	_bool	VerifyIsPlayerOnLeft();						// 플레이어가 왼쪽에 있는지 판별
	_bool	VerifyIsPlayerNearby(_float _fDistance);	// 플레이어가 주변에 있는지 판별
	_bool	VerifyIsPlayerInFront();					// 플레이어가 앞쪽에 있는지 판별

private:
	void	Collision_State();
	void	ElementalReaction(_float fDamage, ELEMENTAL_REACTION eReact);
	void	Set_ElementUI(ELEMENT eElement);
	void	Set_Damage(_float fDamage, ELEMENT eElement, _bool bElementalReaction = FALSE);

public:
	void Set_IsCoreExposure(_bool _IsCoreExposure) { m_bIsCoreExposure = _IsCoreExposure; }
	_bool Get_IsCoreExposure() { return m_bIsCoreExposure; }
	_bool Get_IsFireShieldActive() { return m_bIsFireShieldActive; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CEffigy_Fire*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END