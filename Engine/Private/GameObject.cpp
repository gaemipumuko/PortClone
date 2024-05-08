#include "GameObject.h"

#include "Component.h"

#include "GameInstance.h"

_uint						CGameObject::m_iUniqueID = 0;
vector<CGameObject*>		CGameObject::m_MonsterTarget;

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

CGameObject::CGameObject(const CGameObject& rhs)
	: m_pGameInstance(rhs.m_pGameInstance)
	, m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
	, m_bCloned(TRUE)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CGameObject::Add_Component(_uint iLevel, const wstring& strPrototypeTag, const wstring& strComponentTag, 
	class _Inout_ CComponent** ppComponent, void* pArg)
{
	if (Find_Component(strComponentTag) != nullptr)
		return E_FAIL;

	CGameInstance* pInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pInstance);

	CComponent* pClone = pInstance->Clone_Component_Prototype(iLevel, strPrototypeTag, pArg);
	if (pClone == nullptr)
		return E_FAIL;

	*ppComponent = pClone;
	Safe_AddRef(pClone);

	m_Components.emplace(strComponentTag, pClone);

	Safe_Release(pInstance);

	return S_OK;
}

HRESULT CGameObject::Release_Component(const wstring& strComponentTag)
{
	CComponent* pComponent = Find_Component(strComponentTag);
	if (pComponent == nullptr)
		return E_FAIL;

	m_Components.erase(strComponentTag);
	Safe_Release(pComponent);

	return S_OK;
}

CComponent* CGameObject::Find_Component(const wstring& strComponentTag)
{
	auto	iter = m_Components.find(strComponentTag);

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

wstring CGameObject::Get_ParticleTag(wstring strParticleTag)
{
	wstring strTag = strParticleTag;
	strTag += to_wstring(m_iObjectID);
	strTag += to_wstring(m_iParticleIndex++);

	//cout << ConvertWCToC(strTag) << endl;

	return strTag;
}

void CGameObject::Release_Target()
{
	for (auto& pMonsterTarget : m_MonsterTarget)
	{
		Safe_Release(pMonsterTarget);
	}
}

void CGameObject::Add_Target(CGameObject* pGameObject)
{
	m_MonsterTarget.push_back(pGameObject);
}

void CGameObject::Clear_Target()
{
	m_MonsterTarget.clear();
}

HRESULT CGameObject::Initialize_Clone(void* pArg)
{
	m_iObjectID = m_iUniqueID++;

	return S_OK;
}

void CGameObject::Free()
{
	__super::Free();

	for (auto& iter : m_MTrailList)
		Safe_Release(iter.pModelComponent);
	m_MTrailList.clear();

	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);
}
