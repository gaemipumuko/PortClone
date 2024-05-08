#include "FiniteStateMachine.h"

CFiniteStateMachine::CFiniteStateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CFiniteStateMachine::CFiniteStateMachine(const CFiniteStateMachine& rhs)
	: CComponent(rhs)
{
}

void CFiniteStateMachine::Append_GeneralState(vector<_int> iStates)
{
	for (auto& iter : iStates)
		m_GeneralStateList.push_back(iter);
}

void CFiniteStateMachine::Append_RegulateState(vector<_int> iStates)
{
	for (auto& iter : iStates)
		m_RegulateStateList.push_back(iter);
}

_int CFiniteStateMachine::Change_State(_int iState)
{
	for (auto& iter : m_GeneralStateList)
	{
		if (iter == m_iCurrentState)
			return m_iCurrentState;
	}

	m_iPrevState = m_iCurrentState;
	m_iCurrentState = iState;

	return m_iCurrentState;
}

_int CFiniteStateMachine::Change_ForceState(_int iState)
{
	m_iPrevState = m_iCurrentState;
	m_iCurrentState = iState;

	return m_iCurrentState;
}

_int CFiniteStateMachine::Change_RegulateState(_int iState)
{
	for (auto& iter : m_RegulateStateList)
	{
		if (iter == m_iCurrentState)
			return m_iCurrentState;
	}

	m_iCurrentState = iState;

	return m_iCurrentState;
}

HRESULT CFiniteStateMachine::Initialize_Prototype(_int iNumStates)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFiniteStateMachine::Initialize_Clone(void* pArg)
{
	return S_OK;
}

CFiniteStateMachine* CFiniteStateMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iNumStates)
{
	CFiniteStateMachine* pInstance = new CFiniteStateMachine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iNumStates)))
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
