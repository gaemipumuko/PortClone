#include "Edit_Spawner.h"
#include "Transform.h"
#include "Renderer.h"
#include "Shader.h"
#include "Collider.h"
#include "Bounding_Sphere.h"
#include "Edit_Model.h"
#include "GameInstance.h"
#include "Camera.h"

CEdit_Spawner::CEdit_Spawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEdit_Spawner::CEdit_Spawner(const CEdit_Spawner& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEdit_Spawner::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEdit_Spawner::Initialize_Clone(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	SPAWNER_DESC* pSpawnerDesc = (SPAWNER_DESC*)pArg;
	{
		m_iLevel = pSpawnerDesc->iLevel;
		m_vSpawnerPos = pSpawnerDesc->vSpawnerPos;
		m_fSpawnRange = pSpawnerDesc->fSpawnRange;
	}

	if (nullptr != pSpawnerDesc->pMonsterInfo) 
	{
		for (_int i = 0; i < pSpawnerDesc->pMonsterInfo->size(); i++) 
		{
			MONSTER_INFO tMonsterInfo = {};
			{
				tMonsterInfo.eCreatureType = (*pSpawnerDesc->pMonsterInfo)[i].eCreatureType;
				tMonsterInfo.vSpawnPosition = (*pSpawnerDesc->pMonsterInfo)[i].vSpawnPosition;
			}

			m_MonsterInfo.push_back(tMonsterInfo);
		}
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEdit_Spawner::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CEdit_Spawner::Tick(_float fTimeDelta)
{
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CEdit_Spawner::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);

	return S_OK;
}

HRESULT CEdit_Spawner::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEdit_Spawner::Add_MonsterInfo(MONSTER_INFO* pInfo)
{
	if (nullptr == pInfo)
		return E_FAIL;

	MONSTER_INFO tMonsterInfo = {};
	memcpy(&tMonsterInfo, pInfo, sizeof(MONSTER_INFO));

	m_MonsterInfo.push_back(tMonsterInfo);

	return S_OK;
}

HRESULT CEdit_Spawner::Add_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vSpawnerPos));

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Cube"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Texture_SpawnCube"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_SPHERE::BOUNDING_SPHERE_DESC tBounding_Desc = {};
	{
		tBounding_Desc.fRadius = m_fSpawnRange * 0.5f;
		tBounding_Desc.vCenter = _float3(0.0f, 0.0f, 0.0f);
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tBounding_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEdit_Spawner::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float fCamFar = static_cast<CCamera*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"))->Get_CamFar();

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fCamFar, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_CubeTexture")))
		return E_FAIL;

	return S_OK;
}

CEdit_Spawner* CEdit_Spawner::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEdit_Spawner* pInstance = new CEdit_Spawner(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEdit_Spawner"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEdit_Spawner::Clone(void* pArg)
{
	CEdit_Spawner* pInstance = new CEdit_Spawner(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEdit_Spawner"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEdit_Spawner::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);
}
