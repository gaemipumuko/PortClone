#include "stdafx.h"
#include "FallingSpearTrail.h"

#include "GameInstance.h"

CFallingSpearTrail::CFallingSpearTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDeciderObject(pDevice, pContext)
{
}

CFallingSpearTrail::CFallingSpearTrail(const CFallingSpearTrail& rhs)
	: CDeciderObject(rhs)
{
}

HRESULT CFallingSpearTrail::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Piercing_Trail"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFallingSpearTrail::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CFallingSpearTrail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFallingSpearTrail::Initialize_Clone(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	if (FAILED(CFallingSpearTrail::Add_Component(pArg)))
		return E_FAIL;

	//PIERCING_DESC
	FALLINGSPEARTRAIL_DESC* pDesc = (FALLINGSPEARTRAIL_DESC*)pArg;
	m_pParentTransformCom	= pDesc->pParentTransformCom;
	m_fHeightCorrection		= pDesc->fHeightCorrection;
	m_fTrailSpeed			= pDesc->fTrailSpeed;
	m_vScale				= pDesc->vScale;

	m_pTransformCom->Set_Scale(XMLoadFloat3(&pDesc->vScale));
	m_pTransformCom->RotationFromCurrentState({ 1.f, 0.f, 0.f, 0.f }, XMConvertToRadians(180.f));
	if (pDesc->bRotation)
		m_pTransformCom->RotationFromCurrentState({0.f, 1.f, 0.f, 0.f}, XMConvertToRadians(90.f));

	_vector vPos = m_pParentTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, m_fHeightCorrection, 0.f, 0.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_fLifeTime = 2.f;

	return S_OK;
}

HRESULT CFallingSpearTrail::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CFallingSpearTrail::Tick(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;
	if (m_fLifeTime <= 0.f)
	{
		Set_Destroy(TRUE);
	}

	m_fTimeAcc += fTimeDelta * m_fTrailSpeed;

	return S_OK;
}

HRESULT CFallingSpearTrail::LateTick(_float fTimeDelta)
{
	_vector vPos = m_pParentTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, m_fHeightCorrection, 0.f, 0.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	return S_OK;
}

HRESULT CFallingSpearTrail::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(VTXTEX_PIERCING_TRAIL)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

CFallingSpearTrail* CFallingSpearTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFallingSpearTrail* pInstance = new CFallingSpearTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CFallingSpearTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFallingSpearTrail::Clone(void* pArg)
{
	CFallingSpearTrail* pInstance = new CFallingSpearTrail(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CPiercing_Trail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFallingSpearTrail::Free()
{
	__super::Free();

	if (m_bCloned == true)
	{

		Safe_Release(m_pShaderCom);
		Safe_Release(m_pTextureCom);
		Safe_Release(m_pVIBufferCom);
		Safe_Release(m_pRendererCom);
		Safe_Release(m_pTransformCom);

	}
}
