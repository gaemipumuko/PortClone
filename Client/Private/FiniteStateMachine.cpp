#include "stdafx.h"
#include "FiniteStateMachine.h"

#include "GameInstance.h"

CFiniteStateMachine::CFiniteStateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent(pDevice, pContext)
{
}

CFiniteStateMachine::CFiniteStateMachine(const CFiniteStateMachine& rhs)
    : CComponent(rhs)
{
}

FSM_STATE CFiniteStateMachine::Change_State(FSM_STATE eState)
{
    if (m_eCurrentState == FSM_IDLE ||
        m_eCurrentState == FSM_MOVE ||
        m_eCurrentState == FSM_CHASE ||
		m_eCurrentState == FSM_ATTACK_1 ||
		m_eCurrentState == FSM_ATTACK_2 ||
		m_eCurrentState == FSM_ATTACK_3 ||
		m_eCurrentState == FSM_CRAFT_ITEM ||
		m_eCurrentState == FSM_CRAFT_FOOD)
        m_eCurrentState = eState;

    return m_eCurrentState;
}

FSM_STATE CFiniteStateMachine::Change_ForceState(FSM_STATE eState)
{
	m_eCurrentState = eState;

	return m_eCurrentState;
}

FSM_STATE CFiniteStateMachine::Change_RegulateState(FSM_STATE eState)
{
	if (m_eCurrentState == FSM_IDLE ||
		m_eCurrentState == FSM_MOVE ||
		m_eCurrentState == FSM_CHASE)
		m_eCurrentState = eState;

	return m_eCurrentState;
}

HRESULT CFiniteStateMachine::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFiniteStateMachine::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	m_eCurrentState = FSM_IDLE;

    return S_OK;
}

CFiniteStateMachine* CFiniteStateMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFiniteStateMachine* pInstance = new CFiniteStateMachine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CFiniteStateMachine"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CFiniteStateMachine::Clone(void* pArg)
{
	CFiniteStateMachine* pInstance = new CFiniteStateMachine(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CFiniteStateMachine"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFiniteStateMachine::Free()
{
    __super::Free();
}
