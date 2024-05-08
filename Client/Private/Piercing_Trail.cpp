#include "stdafx.h"
#include "Piercing_Trail.h"

#include "GameInstance.h"

CPiercing_Trail::CPiercing_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDeciderObject(pDevice, pContext)
{
}

CPiercing_Trail::CPiercing_Trail(const CPiercing_Trail& rhs)
	: CDeciderObject(rhs)
{
}

HRESULT CPiercing_Trail::Add_Component(void* pArg)
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

HRESULT CPiercing_Trail::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPiercing_Trail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPiercing_Trail::Initialize_Clone(void* pArg)
{
	if (FAILED(CPiercing_Trail::Add_Component(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		PIERCING_DESC* pDesc = static_cast<PIERCING_DESC*>(pArg);

		m_pParentModelCom = pDesc->pModelCom;
		m_pParentCombineWorldMatrix = pDesc->pCombineWorldMatrix;
		m_fTrailSpeed = pDesc->fTrailSpeed;
		m_vScale = pDesc->vScale;

		_matrix WorldMatrix = XMLoadFloat4x4(m_pParentCombineWorldMatrix);
		WorldMatrix.r[0] = XMVector3Normalize(WorldMatrix.r[0]);
		WorldMatrix.r[1] = XMVector3Normalize(WorldMatrix.r[1]);
		WorldMatrix.r[2] = XMVector3Normalize(WorldMatrix.r[2]);
		_matrix vPos = XMLoadFloat4x4(m_pParentModelCom->Get_CombinedMatrix("RootNode")) * XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) * WorldMatrix;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos.r[3]);
		m_pTransformCom->Set_Scale(XMLoadFloat3(&pDesc->vScale));

		_matrix HighBone = 
			XMLoadFloat4x4(m_pParentModelCom->Get_CombinedMatrix("Trail_High")) * 
			XMMatrixRotationAxis(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(180.f)) * 
			WorldMatrix;
		_matrix LowBone = 
			XMLoadFloat4x4(m_pParentModelCom->Get_CombinedMatrix("Trail_Low")) * 
			XMMatrixRotationAxis(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(180.f)) *
			WorldMatrix;
		_vector vDir = XMVector3Normalize(HighBone.r[3] - LowBone.r[3]);

		m_pTransformCom->Look_At_Direction(vDir);

		_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		m_pTransformCom->RotationFromCurrentState(vRight, XMConvertToRadians(90.f));

		if (pDesc->bRotation == TRUE)
		{
			_vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
			m_pTransformCom->RotationFromCurrentState(vUp, XMConvertToRadians(90.f));
		}
	}
	else
		return E_FAIL;


	m_fLifeTime = 2.f;



	return S_OK;
}

HRESULT CPiercing_Trail::PreviousTick(_float fTimeDelta)
{

	return S_OK;
}

HRESULT CPiercing_Trail::Tick(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;
	if (m_fLifeTime <= 0.f)
	{
		Set_Destroy(TRUE);
	}

	/*_matrix WorldMatrix = XMLoadFloat4x4(m_pCombineWorldMatrix);
	_vector vPos = WorldMatrix.r[3];
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);*/

	/*m_pTransformCom->Rotation({ 0.f,1.f,0.f,0.f }, XMConvertToRadians(90.f));
	m_pTransformCom->Rotation({ 0.f,0.f,1.f,0.f }, XMConvertToRadians(45.f));*/

	m_fTimeAcc += fTimeDelta * m_fTrailSpeed;
	//m_fColorRate0 += log(m_fTimeAcc / 10 + 1.f) * 4.f;

	//XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CPiercing_Trail::LateTick(_float fTimeDelta)
{
	/*_matrix WorldMatrix = XMLoadFloat4x4(m_pParentCombineWorldMatrix);
	_vector vPos = WorldMatrix.r[3];
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);*/

	_matrix WorldMatrix = XMLoadFloat4x4(m_pParentCombineWorldMatrix);
	WorldMatrix.r[0] = XMVector3Normalize(WorldMatrix.r[0]);
	WorldMatrix.r[1] = XMVector3Normalize(WorldMatrix.r[1]);
	WorldMatrix.r[2] = XMVector3Normalize(WorldMatrix.r[2]);
	_matrix vPos = XMLoadFloat4x4(m_pParentModelCom->Get_CombinedMatrix("RootNode")) * XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) * WorldMatrix;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos.r[3]);

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	return S_OK;
}

HRESULT CPiercing_Trail::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	//_vector vSelfScale = m_pTransformCom->Get_Scale();
	//vSelfScale.m128_f32[0] += 0.01f;
	//vSelfScale.m128_f32[1] += 0.01f;
	//m_pTransformCom->Set_Scale(vSelfScale);


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

CPiercing_Trail* CPiercing_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPiercing_Trail* pInstance = new CPiercing_Trail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CPiercing_Trail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPiercing_Trail::Clone(void* pArg)
{
	CPiercing_Trail* pInstance = new CPiercing_Trail(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CPiercing_Trail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPiercing_Trail::Free()
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