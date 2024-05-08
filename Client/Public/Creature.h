#pragma once

#include "Client_Defines.h"
#include "DeciderObject.h"

#include "Damage_Font.h"
#include "Camera_Main.h"
#include "Transform.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CFiniteStateMachine;
END

BEGIN(Client)

class CCreature abstract : public CDeciderObject
{
public:
	typedef struct tagCreatureDesc
	{
		_int			iInitialCell;

		CREATUREID		eCreatureID;
		RECEIVED_COM	tBodyModelCom;
	} CREATURE_DESC;

protected:
	CCreature(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCreature(const CCreature& rhs);
	virtual ~CCreature() = default;

protected:
	CRenderer*				m_pRendererCom = { nullptr };
	CTransform*				m_pTransformCom = { nullptr };
	CFiniteStateMachine*	m_pFiniteStateMachineCom = { nullptr };

protected:
	_int				m_iInitialCell = { };
	CREATUREID			m_eCreatureID = { CREATURE_END };
	class CCharacterController* m_pController = { nullptr };

protected:
	wstring					m_strStatusTag = L"";
	_float					m_fElectroChargedTime = { 0.f };
	_float					m_fSelfDamage = { 0.f };

protected:
	_bool					m_bDropChest = { FALSE };

protected:
	virtual HRESULT		Add_Parts(void* pArg) PURE;
	HRESULT				Add_Component(void* pArg);
	HRESULT				Initialize_Controller(_float fHeight, _float fRadius, _float fStepOffset, CTransform* pTransformCom);

protected:
	HRESULT				Drop_Item(_int iItemIndex, _int ItemQuantity);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	CREATUREID	Get_CreatureID() { return m_eCreatureID; }
	_vector		Get_Position() { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }

public:
	void Create_DamageFont(_float fDamage, ELEMENT eElemnet = ELEMENT_END, _fvector vPos = {}, ELEMENT eImmuneElement = ELEMENT_END);
	void Create_ReactionFont(ELEMENTAL_REACTION eReaction, _fvector vPos = {}, ELEMENTAL_REACTION eImmuneReaction = REACT_END);
	void Create_HealFont(_float fHeal, _fvector vPos = {});

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

END