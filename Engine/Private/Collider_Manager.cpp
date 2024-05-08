#include "Collider_Manager.h"
#include "Collider.h"

#include "GameObject.h"

#include "GameInstance.h"

CCollider_Manager::CCollider_Manager()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

void CCollider_Manager::Set_IsCollision(_uint iLayer, _int iObjectID, _bool bCollision)
{
	auto& iter = m_CollObjects[iLayer].find(iObjectID);

	if (iter == m_CollObjects[iLayer].end())
		return;

	iter->second.bCollision = bCollision;
}

HRESULT CCollider_Manager::Add_Collider_Object(_uint iLayer, OUT _int* pCreateObjectID, CCollider* pCollider, CGameObject* pGameObject)
{
	if (pCreateObjectID == nullptr || pCollider == nullptr)
		return E_FAIL;

	m_CollObjects[iLayer].emplace(m_iNumObjectID, COLLIDER_OBJECT{ TRUE, pCollider, pGameObject });
	*pCreateObjectID = m_iNumObjectID++;

	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CCollider_Manager::Release_Collider_Object(_uint iLayer, _int iObjectID)
{
	CGameObject* pGameObject = Find_ObjectID(iLayer, iObjectID);
	if (pGameObject == nullptr)
		return E_FAIL;

	m_CollObjects[iLayer].erase(iObjectID);
	Safe_Release(pGameObject);

	return S_OK;
}

CGameObject* CCollider_Manager::Find_ObjectID(_uint iLayer, _int iObjectID)
{
	auto& iter = m_CollObjects[iLayer].find(iObjectID);

	if (iter == m_CollObjects[iLayer].end())
		return nullptr;

	return iter->second.pGameObject;
}

CGameObject* CCollider_Manager::Find_AllCollision_Object(_uint iLayer, _int iObjectID)
{
	CCollider* pCollider = m_CollObjects[iLayer].find(iObjectID)->second.pCollider;

	if (pCollider != nullptr)
	{
		for (_uint iLayer = 0; iLayer < m_iNumLayer; ++iLayer)
		{
			for (auto& Object : m_CollObjects[iLayer])
			{
				if (Object.second.bCollision == FALSE)
					continue;

				if (pCollider->Intersect(Object.second.pCollider))
					return Object.second.pGameObject;
			}
		}
	}

	return nullptr;
}

vector<class CGameObject*> CCollider_Manager::Find_Collision_Objects(_uint iLayer, _int iObjectID, _uint iFindLayer)
{
	vector<class CGameObject*> vecObjects;

	CCollider* pCollider = nullptr;
	auto& iter = m_CollObjects[iLayer].find(iObjectID);
	if(iter != m_CollObjects[iLayer].end())
		pCollider = iter->second.pCollider;

	if (pCollider != nullptr)
	{
		for (auto& Object : m_CollObjects[iFindLayer])
		{
			if (Object.second.bCollision == FALSE)
				continue;

			if (pCollider->Intersect(Object.second.pCollider) && Object.first != iObjectID)
				vecObjects.push_back(Object.second.pGameObject);
		}
	}

	return vecObjects;
}

vector<class CGameObject*> CCollider_Manager::Find_Collision_Objects(CCollider* pCollider, _uint iFindLayer)
{
	vector<class CGameObject*> vecObjects;

	if (pCollider != nullptr)
	{
		for (auto& Object : m_CollObjects[iFindLayer])
		{
			if (Object.second.bCollision == FALSE)
				continue;

			if (pCollider->Intersect(Object.second.pCollider))
				vecObjects.push_back(Object.second.pGameObject);
		}
	}

	return vecObjects;
}

HRESULT CCollider_Manager::Initialize(_uint iNumLayer)
{
	m_CollObjects = new map<_int, COLLIDER_OBJECT>[iNumLayer];

	m_iNumLayer = iNumLayer;

	return S_OK;
}

void CCollider_Manager::Clear()
{
	for (size_t i = 0; i < m_iNumLayer; i++)
	{
		for (auto& Pair : m_CollObjects[i])
		{
			//Safe_Release(Pair.second.pCollider);
			Safe_Release(Pair.second.pGameObject);
		}

		m_CollObjects[i].clear();
	}
}

CCollider_Manager* CCollider_Manager::Create(_uint iNumLayer)
{
	CCollider_Manager* pInstance = new CCollider_Manager();

	if (FAILED(pInstance->Initialize(iNumLayer)))
	{
		MSG_BOX(TEXT("Failed to Create : CCollider_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);

	for (size_t i = 0; i < m_iNumLayer; i++)
	{
		for (auto& Pair : m_CollObjects[i])
		{
			Safe_Release(Pair.second.pCollider);
			Safe_Release(Pair.second.pGameObject);
		}
		m_CollObjects[i].clear();
	}
	Safe_Delete_Array(m_CollObjects);
}

/*

	vector<CGameObject*> CollisionObjects;
	for (auto& iter : m_pGameInstance->Find_Collision_Objects(m_pQBSkillColliderCom, m_eCollisionID == COLL_ENEMY ? COLL_FRIENDLY : COLL_ENEMY))
		CollisionObjects.push_back(iter);

	for (auto& iter : m_pGameInstance->Find_Collision_Objects(m_pQBSkillColliderCom, COLL_WILD))
		CollisionObjects.push_back(iter);

	for (auto& iter : m_pGameInstance->Find_Collision_Objects(m_bFullCharge ? m_pQSFullSkillColliderCom : m_pQSSkillColliderCom, m_eCollisionID == COLL_ENEMY ? COLL_FRIENDLY : COLL_ENEMY))
		CollisionObjects.push_back(iter);

	for (auto& iter : m_pGameInstance->Find_Collision_Objects(m_bFullCharge ? m_pQSFullSkillColliderCom : m_pQSSkillColliderCom, COLL_WILD))
		CollisionObjects.push_back(iter);

	for (auto& Object : CollisionObjects)
	{
		_bool bPreCheck = FALSE;
		for (auto& PreObject : m_PreCollisions)
		{
			if (Object == PreObject)
			{
				bPreCheck = TRUE;
				break;
			}
		}

		if (bPreCheck != TRUE)
		{
			if (m_bFullCharge == TRUE)
				m_pParent->Execute_Damage(Object, 2.f + 0.75f * m_pSkills[SKILL_Q].iSkillLevel.x);
			else
				m_pParent->Execute_Damage(Object, 2.f + 0.5f * m_pSkills[SKILL_Q].iSkillLevel.x);

			m_PreCollisions.push_back(Object);
		}
	}

	return S_OK;
*/