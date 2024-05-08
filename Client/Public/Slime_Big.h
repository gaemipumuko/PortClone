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

class CSlime_Big final : public CCreature
{
private:
	CSlime_Big(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSlime_Big(const CSlime_Big& rhs);
	virtual ~CSlime_Big() = default;

private:
	typedef struct tagPartySlots
	{
		class CPartObject*	pPartySlot = { };
	} PARTY_SLOTS;

	vector<PARTY_SLOTS>	m_Parts;

public:
	enum PART { PART_BODY, PART_SHIELD, PART_STATUS, PART_END };

private:
	CCollider*				m_pFogCollider = { nullptr };

private:
	_vector					m_vInitPos = { };
	_bool					m_bReturn = { FALSE };
	_bool					m_bPlayerLook = { FALSE };
	_float					m_fAttackTime = { 0.f };

	_bool					m_bWalk = { FALSE };
	_bool					m_bInitWalk = { FALSE };
	_float					m_fWalkTime = { 0.f };
	
	_int					m_iAlertState = { 0 };

	_float					m_fFogTime = { 0.f };
	_bool					m_bFirstFog = { FALSE };

	FSM_SLIME_STATE			m_ePrevState = { FSM_SLIME_END };

	// Attack Collision
	_bool					m_bHitPlayer = TRUE;

	_float					m_fHp = 30000.f;
	_float					m_fShield = 25000.f;

	_bool					m_bInitPerification = { FALSE };

	_int					m_iFogColliderID = { -1 };
	_bool					m_bFogAttack = { FALSE };
	_float					m_fFogAttackDelay = 0.f;
	_int					m_iFogAttackNum = 0;

	//BAR
	class CHp_Bar*			m_pHpBar;
	CElement_OHD*			m_pElement_OHD = { nullptr };

private:
	virtual HRESULT Add_Parts(void* pArg);

public:
	class CPartObject* Get_PartObject(PART eType) { return m_Parts[eType].pPartySlot; }

private: 
	HRESULT Pattern(_float fTimeDelta);
	HRESULT Return();
	void	Add_Fog();

private:
	void			Collision_State(_float fTimeDelta);
	FSM_SLIME_STATE Get_State(FSM_OBJECT_STATE eForceState);

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
	static CSlime_Big*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END