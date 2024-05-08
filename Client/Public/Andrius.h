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

class CAndrius final : public CCreature
{
private:
	CAndrius(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAndrius(const CAndrius& rhs);
	virtual ~CAndrius() = default;

private:
	typedef struct tagPartySlots
	{
		class CPartObject*	pPartySlot[MAX_PARTYSLOT] = { };
	} PARTY_SLOTS;

	enum PART { PART_BODY, PART_STATUS, PART_END };

	_byte				m_byPartyIndex = { };	/* 현재 선택된 캐릭터 인덱스 */
	vector<PARTY_SLOTS>	m_Parts;

private:
	// 스탯 관련 변수
	_float	m_fHp = 0.f;
	_float	m_fMaxHp = 89937.f;

	// 패턴 관련
	_float	m_fPatternTimer = 0.f;
	_vector m_vFieldCenterPos = { 953.0f, 0.f, 187.0f, 1.0f };	// 필드의 중앙좌표
	_bool	m_bIsPhase2 = FALSE;							// 2페이즈 유무
	_bool	m_bIsAdjustingRange = FALSE;					// 맵 밖으로 나가려는것을 조절 중인지
	FSM_ANDRIUS_STATE m_FSM_NextState = FSM_ANDRIUS_END;	// 다음 행동패턴

	// 할퀴기 패턴 관련
	_bool	m_bClaw = FALSE;
	_bool	m_bClawClock = FALSE;
	_bool	m_bSweep = FALSE;

	// 돌진 패턴 관련 변수
	_vector m_vRushStartPos = { 0.f, 0.f, 0.f, 1.f };
	_vector m_vScramStartPos = { 0.f, 0.f, 0.f, 1.f };
	_bool	m_bIsRushStarted = FALSE;
	_float	m_fCircleTimer = 0.f;		// 돌진 동그라미 이펙트 타이머

	// 점프 패턴 관련 변수
	_vector m_vLastPlayerPos = { 0.f, 0.f, 0.f, 1.f };

	// 잔상 패턴 관련 변수
	_vector m_vDirToBossFromCenter_Nor = {};
	_vector m_vDirCenterToTarget = {};
	_float	m_fTurnRushCircleRadiusScale = 1.f;
	_bool	m_bIsTurnRushStarted = FALSE;
	_int	m_iTurnRushPatternIndex = 0;
	_float  m_fRotVal = 0.1f;
	_float  m_fMinTurnRushCircleRadius = 0.f;

	// 우박 패턴 관련 변수
	_bool	m_bIsHailing = FALSE;		// 우박패턴 활성화 유무
	_float	m_fHailTimer = 0.f;			// 우박패턴 지속시간 타이머
	_float	m_fPlayerHailTimer = 0.f;	// 플레이어에게 떨어지는 우박 타이머
	_float	m_fRandomHailTimer = 0.f;	// 랜덤 으로 떨어지는 우박 타이머

	// 바람 칼날 패턴 관련 변수
	_bool	m_bIsAlreadyWindBladeShot = FALSE;

	// 얼음 충격파 패턴 관련 변수
	_bool	m_bIsIceShockWaving = FALSE;
	_bool	m_bIsAlreadyIceShockWaveShot = FALSE;
	_uint	m_iIceShockWaveIndex = 3;
	_uint	m_iIceShockWaveIce_Floor_Index = 3;
	_float  m_fIceShockWaveTotalTimer = 0.f;		// 패턴 딜레이
	_float  m_fIceShockWaveTimer = 0.f;			// 패턴 지속시간 타이머

	// 사망 패턴 관련 변수
	_bool	m_bIsDying = FALSE;
	_bool	m_bIsDead = FALSE;

	// 플레이어 공격 충돌 관련 변수
	_bool	m_bHitPlayer = TRUE;

	// 이펙트 제어 관련 변수
	_bool	m_bEffectExecuted = FALSE;

	// UI 관련 변수
	class	CHp_Bar* m_pHpBar;
	CElement_OHD* m_pElement_OHD = { nullptr };
	
public:
	_bool Get_IsRushStarted() { return m_bIsRushStarted; }

private:
	virtual HRESULT Add_Parts(void* pArg);

private:
	HRESULT	Pattern(_float fTimeDelta);			// 패턴
	HRESULT	Pattern_Hail(_float fTimeDelta);	// 우박 패턴
	HRESULT	Pattern_IcePillar(_float fTimeDelta);	// 기둥 패턴
	HRESULT	Pattern_IceShockWave(_float fTimeDelta);	// 얼음 충격파 패턴
	HRESULT	Pattern_WindBlade(_float fTimeDelta);	// 바람 칼날 패턴

	_bool	VerifyIsPlayerOnLeft();				// 플레이어가 왼쪽에 있는지 판별
	_bool	VerifyIsPlayerNearby();				// 플레이어가 주변에 있는지 판별
	_bool	VerifyIsPlayerInFront();			// 플레이어가 앞쪽에 있는지 판별

	_bool	VerifyIsImminentWallCollisionForAndrius();			// 안드리우스가 벽에 충돌직전인지 판별

		//VerifyIsAboutToCollideWithWall
	void	DecideNextState();					// 연속해서 이어질 패턴

	// 페이즈 1
	void	Random_Near_Attack_Phase1();		// 페이즈1 근접공격
	void	Random_Far_Attack_Phase1();			// 페이즈1 멀리있을때
	// 페이즈 2
	void	Random_Near_Attack_Phase2();		// 페이즈2 근접공격
	void	Random_Far_Attack_Phase2();			// 페이즈2 멀리있을때

private:
	void	Collision_State();
	void	ElementalReaction(_float fDamage, ELEMENTAL_REACTION eReact);
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
	static CAndrius*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END