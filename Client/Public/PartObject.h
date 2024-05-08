#pragma once

#include "Client_Defines.h"
#include "DeciderObject.h"

BEGIN(Client)

class CPartObject abstract : public CDeciderObject
{
public:
	typedef struct tagPartObjDesc
	{
		class CCreature*	pParent;
	} PARTOBJ_DESC;

protected:
	CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartObject(const CPartObject& rhs);
	virtual ~CPartObject() = default;

protected:
	CTexture*		m_pDissolveCom = { nullptr };
	CTexture*		m_pPetrificationCom = { nullptr };
	CTexture*		m_pFreezingCom = { nullptr };

protected:
	CREATURETYPE	m_eTypeID = CTY_END;

protected:
	class CCreature*	m_pParent = { nullptr };

	_vector				m_vRootTranslation = { };
	_vector				m_vPrevRootTranslation = { };

	_float				m_fDissolve = { 0.f };
	_float				m_fDissolveSize = { 0.05f };
	_float3				m_vDissolveColor = {};
	_bool				m_bForceDissolve = { FALSE };

	_bool				m_bPetrification = { FALSE };
	_float				m_fPetrificationTime = { 0.f };

	_bool				m_bFreezing = { FALSE };
	_float				m_fFreezingTime = { 0.f };

	// Render
	_bool				m_bRenderEnable = { TRUE };

public:
	void Set_Petrification(_bool bPertrification = TRUE);
	void Set_Freezing(_bool bFreezing = TRUE);
	void Set_CreatureID(CREATUREID eID) { return; }
	void Set_RootTranslation(_vector vRootTranslation) { m_vRootTranslation = vRootTranslation; }
	void Set_Dissolve(_bool bDissolve) { m_bForceDissolve = bDissolve; }
	void Set_DissolveRatio(_float fDissolve) { m_fDissolve = fDissolve; }

	_bool	Get_Perification() { return m_bPetrification; }
	_bool	Get_Freezing() { return m_bFreezing; }
	_vector Get_RootTranslation() { return m_vRootTranslation; }
	_vector Get_PrevRootTranslation() { return m_vPrevRootTranslation; }
	_bool	Get_RenderEnable() { return m_bRenderEnable; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual _int				Get_CollisionID() { return -1; }
 	virtual _float				Get_Damage() { return 0; }
	virtual FSM_OBJECT_STATE	Get_ReceiveState()  { return FSM_OBJECT_END; }
	virtual ELEMENT				Get_ReceiveElement() { return ELEMENT_END; }
	virtual ELEMENTAL_REACTION	Get_ElementalReaction() { return REACT_END; }

	virtual void				Set_Damage(_float fDamage, ELEMENT eElement = ELEMENT_END, FSM_OBJECT_STATE eForceState = FSM_OBJECT_HIT_H, _bool isPetrification = FALSE) { return; }

	virtual _bool				isHostElement(ELEMENT eElement) { return FALSE; } // 부착된 원소인지 검사

	virtual void				Release_ColliderID() { return; }


public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

END