#include "Light_Manager.h"
#include "Light.h"

#include "Shader.h"

CLight_Manager::CLight_Manager()
{
}

HRESULT CLight_Manager::Add_Light(_uint iLevelIndex, const LIGHT_DESC& LightDesc)
{
	if (m_iNumLevel <= iLevelIndex)
		return E_FAIL;

	CLight* pLight = CLight::Create(LightDesc);
	if (pLight == nullptr)
		return E_FAIL;

	m_pLights[iLevelIndex].push_back(pLight);

	return S_OK;
}

const LIGHT_DESC* CLight_Manager::Find_LightDesc(_uint iLevelIndex, _uint iIndex) const
{
	if (m_iNumLevel <= iLevelIndex)
		return nullptr;

	if (m_pLights[iLevelIndex].size() <= iIndex)
		return nullptr;

	auto LightDesc = next(m_pLights[iLevelIndex].begin(), iIndex);
	if (LightDesc == m_pLights[iLevelIndex].end())
		return nullptr;

	return (*LightDesc)->Get_LightDesc();
}

HRESULT CLight_Manager::Update_LightPosition(_uint iLevelIndex, _uint iIndex, const _fvector& vLightPosition)
{
	if (m_iNumLevel <= iLevelIndex)
		return E_FAIL;

	if (m_pLights[iLevelIndex].size() <= iIndex)
		return E_FAIL;

	auto LightDesc = next(m_pLights[iLevelIndex].begin(), iIndex);
	if (LightDesc == m_pLights[iLevelIndex].end())
		return E_FAIL;

	(*LightDesc)->Set_LightPosition(vLightPosition);

	return S_OK;
}

HRESULT CLight_Manager::Initialize(_uint iNumLevel)
{
	if (m_pLights != nullptr)
		return E_FAIL;

	m_pLights = new list<class CLight*>[iNumLevel];

	m_iNumLevel = iNumLevel;

	return S_OK;
}

HRESULT CLight_Manager::Render(_uint iLevelIndex, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	if (m_iNumLevel <= iLevelIndex)
		return E_FAIL;

	for (auto& pLight : m_pLights[iLevelIndex])
		pLight->Render(pShader, pVIBuffer);

	return S_OK;
}

CLight_Manager* CLight_Manager::Create(_uint iNumLevel)
{
	CLight_Manager* pInstance = new CLight_Manager();

	if (FAILED(pInstance->Initialize(iNumLevel)))
	{
		MSG_BOX(TEXT("Failed to Create : CLight_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight_Manager::Clear(_uint iLevel)
{
	if (m_pLights == nullptr)
		return;

	if (iLevel >= m_iNumLevel)
		return;

	for (auto& pLight : m_pLights[iLevel])
	{
		Safe_Release(pLight);
	}
	m_pLights[iLevel].clear();
}

void CLight_Manager::Free()
{
	__super::Free();

	for (size_t idx = 0; idx < m_iNumLevel; idx++)
	{
		for (auto& pLight : m_pLights[idx])
			Safe_Release(pLight);

		m_pLights[idx].clear();
	}

	Safe_Delete_Array(m_pLights);
}
