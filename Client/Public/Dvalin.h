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

class CDvalin final : public CCreature
{
private:
	CDvalin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDvalin(const CDvalin& rhs);
	virtual ~CDvalin() = default;

private:
	typedef struct tagPartySlots
	{
		class CPartObject*	pPartySlot = { };
	} PARTY_SLOTS;
	_byte				m_byPartyIndex = { };	/* 현재 선택된 캐릭터 인덱스 */
	vector<PARTY_SLOTS>	m_Parts;

public:
	enum PART { PART_BODY, PART_STATUS, PART_END };
	// 드발린 공격 종류
	enum DVALIN_ATTACK_TYPE {
		DVALIN_ATTACK_GNAW,
		DVALIN_ATTACK_BREATH,
		DVALIN_ATTACK_RUSH_BACK_TO_FRONT,
		DVALIN_ATTACK_RUSH_FRONT_TO_BACK,
		DVALIN_ATTACK_RUSH_FRONT_TO_FRONT,
		DVALIN_ATTACK_PULSE_BOMBS,
		DVALIN_ATTACK_OMEGA_BOMB,
		DVALIN_ATTACK_FLOORCOLLAPSE,
		DVALIN_ATTACK_STARFALL,
		DVALIN_ATTACK_END
	};
	// 플랫폼 판별용 열거체
	enum DVALIN_PLATFORM_NUMBER {
		PLATFORM_INDEX_1,
		PLATFORM_INDEX_2,
		PLATFORM_INDEX_3,
		PLATFORM_INDEX_4,
		PLATFORM_INDEX_5,
		PLATFORM_INDEX_6,
		PLATFORM_INDEX_7,
		PLATFORM_INDEX_8,
		PLATFORM_INDEX_9,
		PLATFORM_INDEX_END
	};

private:
	// 설정용 변수
	CCharacterController*	pController = {};
	_vector					m_vInitPos = {};						// 초기 위치
	_vector					m_vPlayerPos = {};						// 플레이어 위치 저장용 변수

	// 스탯 
	_float				m_fHp = 219187.f;							// 체력
	_float				m_fMaxHp = 219187.f;						// 최대 체력

	// 보호막 관련 변수
	_float				m_fWindShield_Hp = 30000.f;					// 바람 보호막 체력
	_float				m_fWindShield_MaxHp = 30000.f;				// 바람 보호막 최대 체력

	_bool				m_bIsWindShieldActive = TRUE;				// 바람 보호막 활성화 여부
	_int				m_iWindShield_BreakCount = 0;				// 바람 보호막 파괴된 횟수

	_float				m_fCollapseDamage = { 0.f };
	_float				m_fCollapseDelay = { 0.f };
	_int				m_iPlatformindex = { 0 };


	// 패턴 관련 변수
	_float				m_fPatternTimer = 0.f;						// 패턴 타이머
	_bool				m_bIsFirstFrame = TRUE;						// 첫프레임에만 실행되도록 관리하기 위한 변수
	_bool				m_bCanRot = FALSE;

	// 공격 패턴 관련 변수
	DVALIN_ATTACK_TYPE	m_eAttack_Type = DVALIN_ATTACK_END;			// 어떤 타입의 스윙 공격인지 판별용 변수 
	_bool				m_bIsAttacking = FALSE;						// 공격중인지 아닌지 판별용 변수
	_int				m_iAttackIndex = 0;							// 공격의 몇번쨰인지 판별용 변수
	_bool				m_bIsFront = FALSE;							// 드발린이 중점이 아닌 앞쪽에 있는지 판별용 변수
	_bool				m_PrevPatternWasAttack = FALSE;				// 이전 패턴이 공격 패턴이었는지 판별용 변수
	_bool				m_PrevPatternWasRush = FALSE;				// 이전 패턴이 돌진 패턴이었는지 판별용 변수
	_bool				m_bAlreadyRoar = FALSE;						// 1차 포효
	_bool				m_bAlreadyRoar_2 = FALSE;					// 2차 포효
	_bool				m_bIsStarFall = FALSE;						// 별똥별 활성화
	_bool				m_bIsFloorCollapse = FALSE;					// 지형 붕괴중

	// 패턴 순서 관련 변수
	vector<_int>		vecFrontPatternOrder = {					// 드발린이 앞쪽에 있을때 나오는 패턴
		DVALIN_ATTACK_GNAW,
		DVALIN_ATTACK_BREATH,
		DVALIN_ATTACK_BREATH,
		DVALIN_ATTACK_GNAW,
		DVALIN_ATTACK_RUSH_FRONT_TO_FRONT,
		DVALIN_ATTACK_PULSE_BOMBS
	};
	vector<_int>		vecBackPatternOrder = {						// 드발린이 뒤쪽에 있을대 나오는 패턴
		//DVALIN_ATTACK_RUSH_BACK_TO_FRONT,
		DVALIN_ATTACK_OMEGA_BOMB
		//DVALIN_ATTACK_FLOORCOLLAPSE,
		//DVALIN_ATTACK_STARFALL
	};

	// 플랫폼, 이동 관련 변수
	DVALIN_PLATFORM_NUMBER m_ePlayerCurrentPlatform = PLATFORM_INDEX_END;		// 플레이어가 있는 현재 플랫폼
	DVALIN_PLATFORM_NUMBER m_eDvalinCurrentPlatform = PLATFORM_INDEX_END;		// 드발린이 있는 현재 플랫폼
	vector<_float>		m_vecDvalinPlatfromRot = { 269.f, 310.f, 350.f, 31.f, 72.f, 112.f, 150.f, 185.f, 230.f }; // 플랫폼에 따른 드발린의 회전 값
												// 270.f, 310.f, 350.f, 30.f, 70.f, 110.f, 150.f, 190.f, 230.f
	_vector				m_vCenterStandardDir = { 0.f, 0.f, 1.f, 0.f };			//  { 0.f, 0.f, 0.f } 좌표에서 z축 방향 단위벡터 (플레이어 위치 판별용)

	// 그로기, 사망 패턴 관련 변수
	_bool				m_bIsDying = FALSE;
	_bool				m_bAlreadyGroggy = FALSE;					// 이미 그로기

	// 플레이어 공격 충돌 관련 변수
	_bool				m_bHitPlayer = { TRUE };
	_bool				m_bBreathe = { FALSE };
	CCollider*			m_pBreatheColliderCom = { nullptr };
	CTransform*			m_pBreatheTransformCom = { nullptr };

	// UI 관련 변수
	class	CHp_Bar*	m_pHpBar = { nullptr };
	CElement_OHD*		m_pElement_OHD = { nullptr };

	// 패턴 테스트
	_int				m_iPattern = { 0 };
	_bool				m_bPatternControl = { FALSE };

	// 플랫폼 붕괴
	_bool				m_bIsFloorCollapse_1 = FALSE;		// 하드하게 붕괴 타이밍
	_bool				m_bIsFloorCollapse_2 = FALSE;		// 하드하게 붕괴 타이밍

	
public:
	void Set_IsDying(_bool _isDying) { m_bIsDying = _isDying; }
	void Set_CanRot(_bool _canRot) { m_bCanRot = _canRot; }
	_float4 Get_PlatformPosition();
	void Set_IsFloorCollapse_1(_bool _IsFloorCollapse) { m_bIsFloorCollapse_1 = _IsFloorCollapse; }
	void Set_IsFloorCollapse_2(_bool _IsFloorCollapse) { m_bIsFloorCollapse_2 = _IsFloorCollapse; }

private:
	virtual HRESULT Add_Parts(void* pArg);

private:
	void BreatheEffect();

private:
	HRESULT	Pattern(_float fTimeDelta);			// 패턴
	HRESULT	Set_FloorCollapse(_bool _SetCollapse, _int __platformNum);

	// 공격 관련
	HRESULT Identify_Dvalin_AttackType();		// 어떤 타입의 공격을 한것인가 판별
	HRESULT Dvalin_Attack_Pattern();			// 공격 제어
	
	// 플레이어 위치 판별
	_bool	VerifyIsPlayerOnLeft();						// 플레이어가 왼쪽에 있는지 판별
	_bool	VerifyIsPlayerNearby(_float _fDistance);	// 플레이어가 주변에 있는지 판별
	_bool	VerifyIsPlayerInFront();					// 플레이어가 앞쪽에 있는지 판별
	void	VerifyPlayerCurrentPlatform();				// 플레이어의 현재 플랫폼 위치 판별

	// 드발린 회전 편의성
	void	SetDvalinPosForPlatform(_vector _selfLook);	// 플랫폼에 따른 드발린 위치 세팅
	void	SetDvalinRotForPlatform(DVALIN_PLATFORM_NUMBER _platformNum);	// 플랫폼에 따른 드발린 위치 세팅

	HRESULT Set_CollapseFX(_float fDegree);
	
private:
	void	Collision_State(_float fTimeDelta);
	void	ElementalReaction(_float* pDamage, ELEMENTAL_REACTION eReact);
	void	Set_ElementUI(ELEMENT eElement);
	void	Set_Damage(_float fDamage, ELEMENT eElement);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CDvalin*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END