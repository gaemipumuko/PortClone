#include "stdafx.h"
#include "Location.h"
#include "Player.h"
#include "GameInstance.h"

CLocation::CLocation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEventCollider(pDevice, pContext)
{
}

CLocation::CLocation(const CLocation& rhs)
	: CEventCollider(rhs)
{
}

HRESULT CLocation::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLocation::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (pArg == nullptr)
		return E_FAIL;

	if (FAILED(CLocation::Add_Components()))
		return E_FAIL;

	LOCATION_DESC* pDesc = (LOCATION_DESC*)pArg;
	m_eLocation = pDesc->eLocation;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pDesc->vSpawnPosition));

	CBounding_SPHERE::BOUNDING_SPHERE_DESC tBounding_Desc = {};
	tBounding_Desc.fRadius = pDesc->fRadius;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tBounding_Desc)))
		return E_FAIL;

	_matrix ColliderMatrix = XMMatrixIdentity();
	ColliderMatrix.r[3] = XMLoadFloat4(&pDesc->vSpawnPosition);
	m_pColliderCom->Update(ColliderMatrix);

	return S_OK;
}

HRESULT CLocation::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CLocation::Tick(_float fTimeDelta)
{
	vector<CGameObject*> isCollPlayer = m_pGameInstance->Find_Collision_Objects(m_pColliderCom, COLL_EVENTREACT_PLAYER);

	if (isCollPlayer.empty() == FALSE)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Player"));
		if (m_eLocation != pPlayer->Get_LocationID())
		{
			pPlayer->Set_LocationID(m_eLocation);
		}
	}

	return S_OK;
}

HRESULT CLocation::LateTick(_float fTimeDelta)
{
#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif

	return S_OK;
}

HRESULT CLocation::Render()
{
	return S_OK;
}

HRESULT CLocation::Add_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

CLocation* CLocation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLocation* pInstance = new CLocation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CSpawnBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLocation::Clone(void* pArg)
{
	CLocation* pInstance = new CLocation(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CLocation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLocation::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
