#include "AssetFX_Manager.h"

#include "GameInstance.h"

CAssetFX_Manager::CAssetFX_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CAssetFX_Manager::Ready_FXPrototype(const wstring& strPrototypeTag, CAssetFX* pAssetFX)
{
	if (pAssetFX == nullptr)
		return E_FAIL;

	if (m_Prototypes.find(strPrototypeTag) != m_Prototypes.end())
		return E_FAIL;

	m_Prototypes.emplace(strPrototypeTag, pAssetFX);

	return S_OK;
}

HRESULT CAssetFX_Manager::Play_AssetFX(const wstring& strPrototypeTag, const wstring& strEffectTag, void* pArgs)
{
	auto& Prototype = m_Prototypes.find(strPrototypeTag);
	if (Prototype == m_Prototypes.end())
		return E_FAIL;

	if (m_Effects.find(strEffectTag) != m_Effects.end())
		return E_FAIL;	

	CAssetFX* pAssetFX = static_cast<CAssetFX*>(Prototype->second->Clone(pArgs));
	if (pAssetFX == nullptr)
		return E_FAIL;

	m_Effects.emplace(strEffectTag, pAssetFX);

	return S_OK;
}

HRESULT CAssetFX_Manager::Stop_AssetFX(const wstring& strEffectTag, _float fDelay)
{
	auto& Effect = m_Effects.find(strEffectTag);

	if (Effect == m_Effects.end())
		return E_FAIL;

	Effect->second->Set_IsTimeFX(TRUE);
	Effect->second->Set_Duration(fDelay);

	return S_OK;
}

HRESULT CAssetFX_Manager::Pause_AssetFX(const wstring& strEffectTag)
{
	auto& Effect = m_Effects.find(strEffectTag);

	if (Effect == m_Effects.end())
		return E_FAIL;

	return S_OK;
}

HRESULT CAssetFX_Manager::Initialize()
{
	return S_OK;
}

HRESULT CAssetFX_Manager::PreviousTick(_float fTimeDelta)
{
	for (auto& iter = m_Effects.begin(); iter != m_Effects.end();)
	{
		iter->second->PreviousTick(fTimeDelta);

		if (iter->second->Get_IsTimeFX() && iter->second->Get_Duration() < 0.f)
		{
			Safe_Release(iter->second);
			iter = m_Effects.erase(iter);
		}
		else
			++iter;
	}

	return S_OK;
}

HRESULT CAssetFX_Manager::Tick(_float fTimeDelta)
{
	for (auto& iter : m_Effects)
		iter.second->Tick(fTimeDelta);

	return S_OK;
}

HRESULT CAssetFX_Manager::LateTick(_float fTimeDelta)
{
	for (auto& iter : m_Effects)
		iter.second->LateTick(fTimeDelta);

	return S_OK;
}

CAssetFX_Manager* CAssetFX_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAssetFX_Manager* pInstance = new CAssetFX_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CAssetFX_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAssetFX_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	for (auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);
	m_Prototypes.clear();

	for (auto& Pair : m_Effects)
		Safe_Release(Pair.second);
	m_Effects.clear();
}
