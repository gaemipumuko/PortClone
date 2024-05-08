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

class CAbyss_Mage final : public CCreature
{
private:
	CAbyss_Mage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAbyss_Mage(const CAbyss_Mage& rhs);
	virtual ~CAbyss_Mage() = default;

private:
	typedef struct tagPartySlots
	{
		class CPartObject* pPartySlot = { };
	} PARTY_SLOTS;
	vector<PARTY_SLOTS>	m_Parts;

public:
	enum PART { PART_BODY, PART_WEAPON, PART_STATUS, PART_END };
	enum ABYSS_MAGE_ATTACK_TYPE { 
		ABYSS_MAGE_ATTACK_NORMAL,
		ABYSS_MAGE_ATTACK_SPECIAL,
		ABYSS_MAGE_ATTACK_BLINK,
		ABYSS_MAGE_ATTACK_GATHER,
		ABYSS_MAGE_ATTACK_END
	};

private:
	// 스탯 관련 변수
	_float	m_fHp = { 100000.f };

	// 패턴 관련
	CREATUREID	m_eCreatureID = CREATURE_END;
	_vector		m_vInitPos = { };							// 초기 위치
	_float		m_fPatternTimer = 0.f;						// 패턴 타이머
	_bool		m_bIsEnemyDetected = FALSE;					// 적 인식 여부
	_bool		m_bIsFirstFrame = TRUE;						// 첫프레임에만 실행되도록 관리하기 위한 변수

	// 원소 보호막 관련 함수
	_float		m_fElementShield_Hp = 40000.f;
	_float		m_fElementShield_MaxHp = 40000.f;

	_bool		m_bIsElementShieldActive = FALSE;			// 원소 보호막이 활성화 여부

	// 공격 패턴 관련 변수
	ABYSS_MAGE_ATTACK_TYPE	m_eAttack_Type = ABYSS_MAGE_ATTACK_END;	// 어떤 타입의 공격인지 판별용 변수 
	_bool		m_bIsAttacking = FALSE;						// 공격중인지 아닌지 판별용 변수
	_bool		m_PrevPatternWasAttack = FALSE;				// 이전 패턴이 공격 패턴이었는지 판별용 변수
	_int		m_iAttackIndex = 0;							// 공격의 몇번쨰인지 판별용 변수
	vector<_int>		PatternOrder = { 1, 1, 2, 3 };
	_bool		m_bIsGatherSkill = FALSE;

	// 우박 패턴
	_float m_fRandomHailTimer = 0.f;

	// 플레이어 공격 충돌 관련 변수
	_bool	m_bHitPlayer = TRUE;
	_bool	m_bInitPerification = { FALSE };

	// UI
	class CHp_Bar* m_pHpBar;
	CElement_OHD* m_pElement_OHD = { nullptr };

private:
	HRESULT Pattern(_float fTimeDelta);						// 전체 패턴 흐름 제어
	HRESULT Pattern_Gather(_float fTimeDelta);				// Gather 패턴 제어

	// 공격 관련
	HRESULT Identify_Abyss_Mage_AttackType();		// 어떤 타입의 공격을 한것인가 판별
	HRESULT Abyss_Mage_Attack_Pattern();			// 공격 제어

	// 플레이어 위치 판별
	_bool	VerifyIsPlayerOnLeft();							// 플레이어가 왼쪽에 있는지 판별
	_bool	VerifyIsPlayerNearby(_float _fDistance);		// 플레이어가 주변에 있는지 판별
	_bool	VerifyIsPlayerInFront();						// 플레이어가 앞쪽에 있는지 판별

private:
	virtual HRESULT Add_Parts(void* pArg);
	void	Collision_State();

	void	ElementalReaction(_float fDamage, ELEMENTAL_REACTION eReact);
	void	Set_ElementUI(ELEMENT eElement);
	void	Set_Damage(_float fDamage, ELEMENT eElement);

public:
	class CPartObject* Get_PartObject(PART eType) { return m_Parts[eType].pPartySlot; }
	_float	Get_Hp() { return m_fHp; }
	_float	Get_ShieldHp() { return m_fElementShield_Hp; }
	_float	Get_ShieldMaxHp() { return m_fElementShield_MaxHp; }
	CREATUREID Get_CreatureID() { return m_eCreatureID; }
	
	void Set_ElementShield_Active();

	void Set_DeadState();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static  CAbyss_Mage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void		 Free() override;
};

END