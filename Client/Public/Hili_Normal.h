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

class CHili_Normal final : public CCreature
{
private:
	CHili_Normal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHili_Normal(const CHili_Normal& rhs);
	virtual ~CHili_Normal() = default;

private:
	typedef struct tagPartySlots
	{
		class CPartObject*	pPartySlot = { };
	} PARTY_SLOTS;

	vector<PARTY_SLOTS>	m_Parts;

public:
	enum PART { PART_BODY, PART_WEAPON, PART_STATUS, PART_END };

private:
	_vector					m_vInitPos = { };
	_bool					m_bReturn = { FALSE };
	_bool					m_bPlayerLook = { FALSE };
	_float					m_fAttackTime = { 0.f };

	_bool					m_bWalk = { FALSE };
	_bool					m_bInitWalk = { FALSE };
	_float					m_fWalkTime = { 0.f };
	
	_int					m_iAlertState = { 0 };

	/* 공격 */
	_bool					m_bAttackReady = { FALSE };
	_bool					m_bAttack = { FALSE };
	_bool					m_bStoreDistance = { FALSE };
	_vector					m_vInitPlayerPos = { };

	_int					m_iIdlePattern = { };

	FSM_HILI_STATE			m_ePrevState = { FSM_HILI_END };

	/* 플레이어 정보 저장 */
	_vector					m_vPlayerPos; 
	_vector					m_vSetDir;
	_float					m_fDistance;
	_float					m_fInitDistance;
	_bool					m_bInitReady = { FALSE };

	// 플레이어 공격 충돌 관련 변수
	_bool					m_bHitPlayer = TRUE;

	_float					m_fHp = { 50000.f };

	_bool					m_bInitPerification = { FALSE };

	//BAR

	class CHp_Bar* m_pHpBar;
	CElement_OHD* m_pElement_OHD = { nullptr };

private:
	virtual HRESULT Add_Parts(void* pArg);

public:
	class CPartObject* Get_PartObject(PART eType) { return m_Parts[eType].pPartySlot; }
	_float	Get_Hp() { return m_fHp; }

private: 
	HRESULT Pattern(_float fTimeDelta);
	HRESULT Attack(_float fTimeDelta);
	HRESULT Return();
	/* 츄츄 Idle 종류 */
	HRESULT Walk(_float fTimeDelta);
	HRESULT Dance(_float fTimeDelta);
	HRESULT Sit(_float fTimeDelta);
	HRESULT Sleep(_float fTimeDelta);

private:
	void	Collision_State();
	FSM_HILI_STATE	Get_State(FSM_OBJECT_STATE eForceState);

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
	static CHili_Normal*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END