#pragma once

#include "Client_Defines.h"
#include "Creature.h"

#include "Hp_Bar.h"
#include "Stamina_Bar.h"
//Test
#include "AlphaRect.h"
#include "Element_OHD.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CCollider;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CPage_Main;
class CCharacterController;

class CPlayer final : public CCreature
{
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

private:
	typedef struct tagPlayerInfo
	{
		CTransform*					pTransform = {};
		CFiniteStateMachine*		pFiniteStateMachineCom = {};
		CCharacterController* pController = { nullptr };

		/* 자동전투 시 필요 */
		_float				fKnockback = { 0.f };
		_float4				vKnockbackDir = {};

		_bool				bAutoMode = { FALSE };
		_bool				bRun = { FALSE };

		_bool				bDead = { FALSE };

		/* 상승기류 탔을때 */
		_bool				bGravity = { TRUE };
	}PLAYER_INFO;

private:
	typedef struct tagPartySlots
	{
		class CPartObject* pPartySlot[MAX_PARTYSLOT] = { };
	} PARTY_SLOTS;

	_byte				m_byPartyIndex = { };	/* 현재 선택된 캐릭터 인덱스 */
	vector<PARTY_SLOTS>	m_Parts;

public:
	enum PART { PART_BODY, PART_WEAPON, PART_GLIDER, PART_STATUS, PART_END };

private:	/* 자동전투 */
	vector<PLAYER_INFO> PlayerInfo;

	CTransform* m_pTransformCom2P = { nullptr };
	CTransform* m_pTransformCom3P = { nullptr };
	CTransform* m_pTransformCom4P = { nullptr };
	CFiniteStateMachine* m_pFiniteStateMachineCom2P = { nullptr };
	CFiniteStateMachine* m_pFiniteStateMachineCom3P = { nullptr };
	CFiniteStateMachine* m_pFiniteStateMachineCom4P = { nullptr };

	CPage_Main*			m_pPageMain = { nullptr };

private:	/* 자동전투 */
	_bool				m_bCombatMode = { FALSE };
	_float				m_fIdleTime = { 0.f };

private:
	_bool				m_bRaidenUltOn = { false };
	_float				m_fRaidenUltTime = {0.f};


	_bool				m_bMoveRun = { FALSE };
	_float				m_fAttackReadyTime = { 0.f };
	FSM_STATE			m_ePrevState = { FSM_END };
	PLAYERINDEX			m_ePlayerNum = { PLAYER_END };
	PLAYERINDEX			m_ePrevPlayerNum = { PLAYER_END };
	vector<CREATUREID>	m_vecPlayerIndex;

	_float				m_fMoveSpeedDuration[4] = {};
	_float				m_fMoveSpeed[4] = {1.f, 1.f, 1.f, 1.f};

	//Test
	CAlphaRect*		m_pOverHeadUI = {nullptr};
	CAlphaRect*		m_pInteractionBox = { nullptr };
	CElement_OHD*	m_pElement_OHD = { nullptr };
	_int			m_iNumPressed = {};


	_bool		m_bIsMoving = { FALSE };

	
	_bool		m_isUndead = { FALSE };
	_float		m_fSpeedCheat = { 0.f };

	_uint		m_iElement = { ELEMENT_END };
	_float		m_fInvincibility[PLAYER_END] = {};
	_bool		m_bInvincibility [PLAYER_END] = {};

	CHANNELID	m_eFlySoundChannel = { MAXCHANNEL };

private:
	_bool		m_bHit[PLAYER_END] = {};
	_float4		m_vTargetMonsterPos = {};
	_uint		m_iExtraAttackCount = { 0 };
	_uint		m_iElementalBurstCount = { 0 };
	_uint		m_iBasicAttackCount = { 0 };
	list<_int>	m_MonsterHitCollider[PLAYER_END];

	_float4		m_vLastPosition = {};

	_bool		m_bStartBGM = { FALSE };
	wstring		m_strNextBGM = L"";
	_bool		m_bBgmChange = { FALSE };
	_bool		m_bBattleSound = { FALSE };
	_bool		m_bLocationFont = { TRUE };
	_float		m_fSoundChange = { 0.f };

	LOCATIONID	m_eLocation = { LOCATION_END };

private:
	_bool		m_bSplintSound = { FALSE };

private:

	class CInventory* m_pInventory = { nullptr };

private:
	virtual HRESULT Add_Parts(void* pArg);

private:
	HRESULT Add_ExtraComponent();

public: /* 플레이어 정보 */
	PLAYER_INFO		Get_PlayerInfo(_int iIndex) {
		if (iIndex >= PlayerInfo.size())
			return {};

		return PlayerInfo[iIndex];
	}
	size_t			Get_TotalPlayer() { return PlayerInfo.size(); }
	void			Set_Gravity(_bool bGravity = TRUE) { PlayerInfo[m_ePlayerNum].bGravity = bGravity; }

public:
	void						Set_Damage(_float fDamage, ELEMENT eElement = ELEMENT_END, FSM_STATE eForceState = FSM_HIT_L, _float fLevitate = 0.f, _float fKnockback = 0.f, _float4 vKnockbackDir = {}, _float fInvincibility = 0.f);
	void						Set_AutoDamage(PLAYERINDEX eIndex, _float fDamage, ELEMENT eElement = ELEMENT_END, FSM_STATE eForceState = FSM_HIT_L, _float fLevitate = 0.f, _float fKnockback = 0.f, _float4 vKnockbackDir = {}, _float fInvincibility = 0.f);
	void						Set_MoveSpeed(_float fSpeed, PLAYERINDEX eIndex, _float fDuration = -1.f);
	void						Set_Heal(PLAYERINDEX eIndex, _float fHeal);
	void						Set_AllHeal(_float fHeal);
	void						Set_RatioHeal(PLAYERINDEX eIndex, _float fHpRatio, _float fAddHeal = 0.f);
	void						Set_AllRatioHeal(_float fHpRatio, _float fAddHeal = 0.f);
	void						Set_MaxHp();
	void						Set_MaxEnergy();
	void						Update_CurrentHp(PLAYERINDEX eIndex);
	class CPartObject*			Get_PartObject(PART eType, _int iIndex) { return m_Parts[eType].pPartySlot[iIndex]; }
	class CCharacterController* Get_Controller() { return PlayerInfo[m_ePlayerNum].pController; }
	PLAYERINDEX					Get_PlayerIndex(CREATUREID eId);
	_int						Get_BodyCollisionID();
	_int						Get_BodyCollisionID(PLAYERINDEX eIndex);
	_vector						Get_Position();
	_vector						Get_Position(PLAYERINDEX eIndex);
	_vector						Get_Look(PLAYERINDEX eIndex);
	_matrix						Get_WorldMatrix();

	PLAYERINDEX					Get_CurrentIndex() { return m_ePlayerNum; }

	ELEMENT						Get_Element() { return (ELEMENT)m_iElement; }
	_float						Get_ElementDamage();
	_float						Get_ElementDamage(PLAYERINDEX eIndex);
	_float						Get_BurstDamage();
	FSM_STATE					Get_State();
	_bool						Get_InAir();

	//TEMP
	_float						m_fSize = {0.1f};

	_bool						m_bRegion = { FALSE };
public:
	_bool	Get_CombatMode() { return m_bCombatMode; }
	void	ElementalReaction(_float fDamage, ELEMENTAL_REACTION eReact, PLAYERINDEX eIndex = PLAYER_END);

	void		Set_LocationID(LOCATIONID eLocation) { m_eLocation = eLocation; m_bBgmChange = TRUE; }
	LOCATIONID	Get_LocationID() { return m_eLocation; }

private:
	HRESULT	Combat_Mode(_float fTimeDelta);
	HRESULT Auto_Combat(_float fTimeDelta, PLAYERINDEX eIndex);	/* 자동전투 시 */

private:
	HRESULT	Input_State(_float fTimeDelta);
	HRESULT Change_Player();
	HRESULT Attack_Skill(_float fTimeDelta);

private:
	HRESULT	Character_Dead(_float fTimeDelta);
	HRESULT	Hit_Collision_State(_float fTimeDelta);
	HRESULT Hit_Collision_State(_float fTimeDelta, PLAYERINDEX eIndex);
	HRESULT MoveSpeed_Control(_float fTimeDelta, PLAYERINDEX eIndex);
	HRESULT	Create_Trail(FSM_STATE eState);
	HRESULT	Create_Trail(FSM_STATE eState, PLAYERINDEX eIndex);

private:
	void	Set_ElementUI(ELEMENT eElement);

	_bool	isHitCollider(list<_int> CheckList, _int ColliderID);

private:
	void Look_MonsterTarget();

private:
	_bool isFunctionTrue();
	_bool isFunctionTrue(PLAYERINDEX eIndex);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CPlayer*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END