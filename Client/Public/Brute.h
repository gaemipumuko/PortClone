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

class CBrute final : public CCreature
{
private:
	CBrute(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBrute(const CBrute& rhs);
	virtual ~CBrute() = default;

private:
	typedef struct tagPartySlots
	{
		class CPartObject* pPartySlot = { };
	} PARTY_SLOTS;
	vector<PARTY_SLOTS>	m_Parts;

public:
	enum PART { PART_BODY, PART_WEAPON, PART_STATUS, PART_END };
	enum AXE_ATTACK_TYPE { 
		AXE_ATTACK_TYPE_1, 
		AXE_ATTACK_TYPE_2, 
		AXE_ATTACK_TYPE_3,
		AXE_ATTACK_TYPE_WHIRL, 
		AXE_ATTACK_TYPE_DANCE, 
		AXE_ATTACK_TYPE_JUMP,
		AXE_ATTACK_TYPE_END
	};

private:
	// 스탯 관련 변수
	_float	m_fHp = { 70000.f };

	// 패턴 관련
	CREATUREID	m_eCreatureID = CREATURE_END;
	_vector		m_vInitPos = { };							// 초기 위치
	_float		m_fPatternTimer = 0.f;						// 패턴 타이머
	_bool		m_bIsEnemyDetected = FALSE;					// 적 인식 여부

	// 이동 관련
	_bool		m_bAlreadyWalkedBackward = FALSE;			// 뒤로 이미 걸어갔다

	// 방패 관련 
	

	// 도끼 스윙 공격
	AXE_ATTACK_TYPE	m_eAttack_Type = AXE_ATTACK_TYPE_END;		// 어떤 타입의 스윙 공격인지 판별용 변수
	_bool			m_bIsAttacking = FALSE;						// 스윙 공격중인지 아닌지 판별용 변수
	_int			m_iAttackIndex = 0;							// 스윙 공격의 몇번쨰인지 판별용 변수
	
	// 플레이어 공격 충돌 관련 변수
	_bool	m_bHitPlayer = TRUE;
	_bool	m_bInitPerification = { FALSE };

	// UI
	class CHp_Bar* m_pHpBar;
	CElement_OHD* m_pElement_OHD = { nullptr };

private:
	HRESULT Pattern(_float fTimeDelta);						// 전체 패턴 흐름 제어

	// 이동 관련
	HRESULT IdentifyWalkType();								// 어떤 타입의 이동을 할것인지 판별

	// 쉴드 공격 관련
	HRESULT Identify_Shield_AttackType();					// 어떤 타입의 방패 공격을 할것인지 판별
	HRESULT Shield_Attack_Pattern();						// 방패 공격 제어
	
	// 도끼 공격 관련
	HRESULT Identify_Axe_AttackType();						// 어떤 타입의 도끼 스윙 공격을 할것인지 판별
	HRESULT Axe_Attack_Pattern();							// 도끼 스윙 공격 제어

	// 플레이어 위치 판별
	_bool	VerifyIsPlayerOnLeft();							// 플레이어가 왼쪽에 있는지 판별
	_bool	VerifyIsPlayerNearby(_float _fDistance);		// 플레이어가 주변에 있는지 판별
	_bool	VerifyIsPlayerInFront();						// 플레이어가 앞쪽에 있는지 판별

private:
	virtual HRESULT Add_Parts(void* pArg);
	void	Collision_State();
	FSM_BRUTE_STATE	Get_State(FSM_OBJECT_STATE eForceState);

	void	ElementalReaction(_float fDamage, ELEMENTAL_REACTION eReact);
	void	Set_ElementUI(ELEMENT eElement);
	void	Set_Damage(_float fDamage, ELEMENT eElement);

public:
	class CPartObject* Get_PartObject(PART eType) { return m_Parts[eType].pPartySlot; }
	_float	Get_Hp() { return m_fHp; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CBrute* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void			Free() override;
};

END