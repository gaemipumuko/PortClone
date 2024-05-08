#include "stdafx.h"
#include "ItemGlow.h"

#include "GameInstance.h"
#include "Ground_Item.h"

CItem_Glow::CItem_Glow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CItem_Glow::CItem_Glow(const CItem_Glow& rhs)
	: CEffect(rhs)
{
}

HRESULT CItem_Glow::Add_Component(void* pArg)
{
	__super::Add_Component(pArg);

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_Item_Glow"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CItem_Glow::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem_Glow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItem_Glow::Initialize_Clone(void* pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);

		m_EffectDesc.eEffectID = pDesc->eEffectID;
		m_EffectDesc.fLifeTime = pDesc->fLifeTime;
		m_EffectDesc.vSpawnPos = pDesc->vSpawnPos;
		m_EffectDesc.vScale	= pDesc->vScale;
		m_EffectDesc.vBaseColor = pDesc->vBaseColor;
		m_EffectDesc.pParent = pDesc->pParent;

		m_fLifeTime = m_EffectDesc.fLifeTime;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vSpawnPos);
		m_pTransformCom->Set_Scale(m_EffectDesc.vScale);

	}
	else
		return E_FAIL;

	//m_pTransformCom->Rotation({ 0.f,0.f,1.f,0.f }, XMConvertToRadians(90.f));

	return S_OK;
}

HRESULT CItem_Glow::PreviousTick(_float fTimeDelta)
{

	return S_OK;
}

HRESULT CItem_Glow::Tick(_float fTimeDelta)
{

	m_fTimeAcc += fTimeDelta;

	_vector vScale = m_EffectDesc.vScale * (1+ (cos(m_fTimeAcc*2.f) * 0.2f));

	m_pTransformCom->Set_Scale(vScale);

	CTransform* pParentsTransform = (CTransform*)(m_EffectDesc.pParent->Find_Component(TEXT("Com_Transform")));
	_vector vLook = pParentsTransform->Get_State(CTransform::STATE_LOOK);
	_vector vPos = pParentsTransform->Get_State(CTransform::STATE_POSITION);
	_vector vUp = pParentsTransform->Get_State(CTransform::STATE_UP);
	
	vLook = XMVector3Normalize(vLook);
	vUp = XMVector3Normalize(vUp);
	vPos = vPos + vUp * 0.09f;

	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	

	//if(m_pGameInstance->Key_Down('O'))
	//{
	//	m_iGrade++;
	//	if (m_iGrade > 4)
	//		m_iGrade = 0;
	//}

	//switch (m_iGrade)
	//{
	//case 0:
	//m_EffectDesc.vBaseColor = COLOR_COMMON;
	//	break;
	//case 1:
	//m_EffectDesc.vBaseColor = COLOR_UNCOMMON;
	//	break;
	//case 2:
	//m_EffectDesc.vBaseColor = COLOR_RARE;
	//	break;
	//case 3:
	//m_EffectDesc.vBaseColor = COLOR_EPIC;
	//	break;
	//case 4:
	//m_EffectDesc.vBaseColor = COLOR_LEGENDARY;
	//	break;
	//default:
	//	break;
	//}

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CItem_Glow::LateTick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix());


	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	return S_OK;
}

HRESULT CItem_Glow::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	//_vector vSelfScale = m_pTransformCom->Get_Scale();
	//vSelfScale.m128_f32[0] += 0.01f;
	//vSelfScale.m128_f32[1] += 0.01f;
	//m_pTransformCom->Set_Scale(vSelfScale);


	m_pTransformCom->Rotation({ 0.f,1.f,0.f,0.f }, XMConvertToRadians(0.f));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture",0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vGradeColor", &m_EffectDesc.vBaseColor, sizeof(_vector))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(VTXTEX_ITEM_GLOW)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pTransformCom->Rotation({ 0.f,1.f,0.f,0.f }, XMConvertToRadians(90.f));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(VTXTEX_ITEM_GLOW)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	return S_OK;
}

void CItem_Glow::Set_Matrix(_matrix vWorldMat)
{
	m_pTransformCom->Set_WorldMatrix(vWorldMat);
}

CItem_Glow* CItem_Glow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItem_Glow* pInstance = new CItem_Glow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CItem_Glow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItem_Glow::Clone(void* pArg)
{
	CItem_Glow* pInstance = new CItem_Glow(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CItem_Glow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem_Glow::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}