#include "stdafx.h"
#include "Elemental_Manager.h"

#include "Elemental_Particle.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CElemental_Manager)

CElemental_Manager::CElemental_Manager()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CElemental_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice	= pDevice;
	Safe_AddRef(m_pDevice);

	m_pContext	= pContext;
	Safe_AddRef(m_pContext);

	/* ElementalParticle */
	m_pElementalParticle	= CElemental_Particle::Create(m_pDevice, m_pContext);

	/* Component : Texture_Elemental */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Elemental"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Asset/Elemental/Elemental_%d.png"), 2))))
		return E_FAIL;

	/* Component : Model_ElementalSphere */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_ElementalSphere"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Asset/Elemental/ElementalSphere.dat"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CElemental_Manager::PreviousTick(_float fTimeDelta)
{
	for (auto& iter = m_ActivedElemental.begin(); iter != m_ActivedElemental.end();)
	{
		iter->second->PreviousTick(fTimeDelta);

		if (iter->second->Get_Intersect() == TRUE)
		{
			m_ElementalPool.emplace(iter->first, iter->second);
			iter = m_ActivedElemental.erase(iter);
		}
		else
			++iter;
	}

	return S_OK;
}

HRESULT CElemental_Manager::Tick(_float fTimeDelta)
{
	for (auto& iter : m_ActivedElemental)
		iter.second->Tick(fTimeDelta);

	return S_OK;
}

HRESULT CElemental_Manager::LateTick(_float fTimeDelta)
{
	for (auto& iter : m_ActivedElemental)
		iter.second->LateTick(fTimeDelta);

	return S_OK;
}

HRESULT CElemental_Manager::Create_Elemental(_fvector vInitPosition, _uint iNumElemental, ELEMENT eAttribute, ELEMENTAL_TYPE eType)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	if (pPlayer == nullptr)
	{
		MSG_BOX(TEXT("Failed to Find : Player"));
		return E_FAIL;
	}
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

	CTransform* pPlayerTransform =
		static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

	/* Elemental Desc */
	CElemental::ELEMENTAL_DESC tElementalDesc = { };
	tElementalDesc.eAttribute = eAttribute;
	tElementalDesc.vInitPosition = vInitPosition;
	tElementalDesc.pTargetTransform = pPlayerTransform;

	/* Create Elemental (Object Pooling) */
	for (_uint iElemental = 0; iElemental < iNumElemental; ++iElemental)
	{
		CElemental* pElemental = { nullptr };
		auto& iter = m_ElementalPool.find(eType);

		if (iter == m_ElementalPool.end())
		{
			switch (eType)
			{
			case ELEMENTAL_PARTICLE:
				pElemental = static_cast<CElemental*>(m_pElementalParticle->Clone(&tElementalDesc));
				break;
			default:
				return E_FAIL;
			}
		}
		else
		{
			pElemental = iter->second;
			pElemental->Invalidate_Elemental(&tElementalDesc);

			m_ElementalPool.erase(iter);
		}

		m_ActivedElemental.emplace(eType, pElemental);
	}

	return S_OK;
}

HRESULT CElemental_Manager::Clear_Elemental()
{
	for (auto& iter = m_ActivedElemental.begin(); iter != m_ActivedElemental.end();)
	{
		m_ElementalPool.emplace(iter->first, iter->second);
		iter = m_ActivedElemental.erase(iter);
	}

	return S_OK;
}

void CElemental_Manager::Free()
{
	__super::Free();

	for (auto& iter : m_ElementalPool)
		Safe_Release(iter.second);
	m_ElementalPool.clear();

	for (auto& iter : m_ActivedElemental)
		Safe_Release(iter.second);
	m_ActivedElemental.clear();

	Safe_Release(m_pElementalParticle);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);
}
