#pragma once

#include "Client_Defines.h"
#include "Component.h"

BEGIN(Client)

class CFiniteStateMachine final : public CComponent
{
private:
	CFiniteStateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFiniteStateMachine(const CFiniteStateMachine& rhs);
	virtual ~CFiniteStateMachine() = default;

private:
	FSM_STATE		m_eCurrentState = { FSM_END };
	FSM_STATE		m_eNextAtk = { FSM_ATTACK_1 };

	_bool			m_bRenderState = { TRUE };

public:
	FSM_STATE		Get_CurretState() { return m_eCurrentState; }
	void			Set_IdleState() { m_eCurrentState = FSM_IDLE; }

	FSM_STATE		Get_NextAttack() { return m_eNextAtk; }
	void			Set_NextAttack(FSM_STATE eState) { m_eNextAtk = eState; }

	_bool			Get_RenderState() { return m_bRenderState; }
	void			Set_RenderState(_bool bRenderState) { m_bRenderState = bRenderState; }

public:
	virtual FSM_STATE	Change_State(FSM_STATE eState);
	virtual FSM_STATE	Change_ForceState(FSM_STATE eState);
	virtual FSM_STATE	Change_RegulateState(FSM_STATE eState);	/* IDLE, MOVE 상태에서만 상태가 변경 가능 */

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);

public:
	static CFiniteStateMachine*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent*				Clone(void* pArg);
	virtual void					Free() override;
};

END