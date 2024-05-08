#include "Interaction_Manager.h"

CInteraction_Manager::CInteraction_Manager()
{
}

void CInteraction_Manager::Add_InteractionCount(wstring strType, _int iCount)
{
	if (isInteraction(strType) == TRUE)
		m_Interaction[strType] += iCount;
}

void CInteraction_Manager::Register_Interaction(wstring strType)
{
	m_Interaction.emplace(strType, 0);
}

void CInteraction_Manager::Unregister_Interaction(wstring strType)
{
	m_Interaction.erase(strType);
}

_bool CInteraction_Manager::isInteraction(wstring strType)
{
	if (m_Interaction.find(strType) != m_Interaction.end())
		return TRUE;
	else
		return FALSE;
}

_int CInteraction_Manager::Get_InteractionCount(wstring strType)
{
	if (isInteraction(strType) == TRUE)
		return m_Interaction[strType];
	else
		return 0;
}

void CInteraction_Manager::Set_InteractionCount(wstring strType, _int iCount)
{
	if (isInteraction(strType) == TRUE)
		m_Interaction[strType] = iCount;
}


CInteraction_Manager* CInteraction_Manager::Create()
{
	CInteraction_Manager* pInstance = new CInteraction_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CInteraction_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CInteraction_Manager::Initialize()
{
	return S_OK;
}

void CInteraction_Manager::Free()
{
	__super::Free();
}
