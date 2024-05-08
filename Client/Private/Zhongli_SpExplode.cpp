#include "stdafx.h"
#include "Zhongli_SpExplode.h"

#include "GameInstance.h"

#include "Camera.h"

CZhongli_SpExplod::CZhongli_SpExplod(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDeciderObject(pDevice, pContext)
{
}

CZhongli_SpExplod::CZhongli_SpExplod(const CZhongli_SpExplod& rhs)
	: CDeciderObject(rhs)
{
}

HRESULT CZhongli_SpExplod::Add_Component(void* pArg)
{
	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Sphere"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CZhongli_SpExplod::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;

	_float fCamFar = static_cast<CCamera*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"))->Get_CamFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fCamFar, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CZhongli_SpExplod::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CZhongli_SpExplod::Initialize_Clone(void* pArg)
{
	//if (FAILED(__super::Initia	lize_Clone(pArg)))
	//	return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		_vector* vSpawnPos = (_vector*)pArg;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, *vSpawnPos);
	}
	else
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_pTransformCom->Set_Scale({ 0.001f, 0.001f, 0.001f, 0.f });

	m_pGameInstance->Play_Particle(TEXT("LightShard"), Get_ParticleTag(TEXT("LightShard")), PARTICLE_SHARD, TEX_PARTICLE_ATLAS, nullptr, m_pTransformCom, 1.f);

	m_fLifeTime = 0.3f;

	return S_OK;
}

HRESULT CZhongli_SpExplod::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CZhongli_SpExplod::Tick(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;
	if(m_fLifeTime <= 0.f)
		Set_Destroy(TRUE);

	m_fTimeAcc += fTimeDelta/2.f;
	m_pTransformCom->Set_Scale({ 0.001f+ m_fTimeAcc, 0.001f + m_fTimeAcc, 0.001f + m_fTimeAcc, 0.f });



	// ------------------------------------------------------------- 임시
	//CTransform* pMonsterTransform = static_cast<CTransform*>(
	//	m_pGameInstance->Find_Layer_Component(LEVEL_WORLD, TEXT("Layer_Monster"), TEXT("Com_Transform")));
	//_vector vLook = pMonsterTransform->Get_State(CTransform::STATE_LOOK);
	//m_pTransformCom->Look_At_Direction(vLook);
	//m_pTransformCom->Go_Straight(fTimeDelta);
	// -------------------------------------------------------------



	//m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CZhongli_SpExplod::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	return S_OK;
}


HRESULT CZhongli_SpExplod::Render()
{

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(VTXMESH_SP_EXPLODE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}


	return S_OK;
}

CZhongli_SpExplod* CZhongli_SpExplod::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CZhongli_SpExplod* pInstance = new CZhongli_SpExplod(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CZhongli_SpExplod"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CZhongli_SpExplod::Clone(void* pArg)
{
	CZhongli_SpExplod* pInstance = new CZhongli_SpExplod(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CZhongli_SpExplod"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CZhongli_SpExplod::Free()
{
	__super::Free();

	if (m_bCloned)
	{
		Safe_Release(m_pTransformCom);
		Safe_Release(m_pColliderCom);
		Safe_Release(m_pModelCom);
		Safe_Release(m_pShaderCom);
		Safe_Release(m_pRendererCom);
	}
} 