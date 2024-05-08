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

class CSlime_Small final : public CCreature
{
private:
	CSlime_Small(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSlime_Small(const CSlime_Small& rhs);
	virtual ~CSlime_Small() = default;

private:
	typedef struct tagPartySlots
	{
		class CPartObject*	pPartySlot = { };
	} PARTY_SLOTS;

	enum PART { PART_BODY, PART_STATUS, PART_END };

	vector<PARTY_SLOTS>	m_Parts;

private:
	_vector					m_vInitPos = { };
	_bool					m_bReturn = { FALSE };
	_bool					m_bPlayerLook = { FALSE };
	_float					m_fAttackTime = { 0.f };

	_bool					m_bWalk = { FALSE };
	_bool					m_bInitWalk = { FALSE };
	_float					m_fWalkTime = { 0.f };

	_bool					m_bInitPerification = { FALSE };
	
	FSM_SLIME_STATE	m_ePrevState = { FSM_SLIME_END };

	// Attack Collision
	_bool					m_bHitPlayer = TRUE;

	_float					m_fHp = 40000.f;

	//BAR
	class CHp_Bar* m_pHpBar;
	CElement_OHD* m_pElement_OHD = { nullptr };

private:
	virtual HRESULT Add_Parts(void* pArg);

private: 
	HRESULT Pattern(_float fTimeDelta);
	HRESULT Return();

private:
	void			Collision_State();
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
	static CSlime_Small*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END