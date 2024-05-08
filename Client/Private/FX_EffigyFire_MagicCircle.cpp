#include "stdafx.h"
#include "FX_EffigyFire_MagicCircle.h"

#include "DeciderObject.h"
#include "GameInstance.h"

CFX_EffigyFire_MagicCircle::CFX_EffigyFire_MagicCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAssetFX(pDevice, pContext)
{
}

CFX_EffigyFire_MagicCircle::CFX_EffigyFire_MagicCircle(const CFX_EffigyFire_MagicCircle& rhs)
	: CAssetFX(rhs)
{
}

HRESULT CFX_EffigyFire_MagicCircle::Add_Component(void* pArg)
{
	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_TexFX"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EffigyFire_MagicCircle"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFX_EffigyFire_MagicCircle::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

HRESULT CFX_EffigyFire_MagicCircle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFX_EffigyFire_MagicCircle::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(XMVectorSet(3.f, 3.f, 3.f, 0.f));

	ASFX_DESC* pAsfxDesc = static_cast<ASFX_DESC*>(pArg);
	if (pAsfxDesc->ePosType == ASFX_DESC::POS_FIX)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, pAsfxDesc->vInitPosition);

		return S_OK;
	}

	m_pRelativeTransform = pAsfxDesc->pTransform;
	Safe_AddRef(m_pRelativeTransform);


	return S_OK;
}

HRESULT CFX_EffigyFire_MagicCircle::PreviousTick(_float fTimeDelta)
{
	if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
	{
		if (FAILED(__super::PreviousTick(fTimeDelta)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CFX_EffigyFire_MagicCircle::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
	{
		m_fTimeAcc += fTimeDelta;

		if (m_pRelativeTransform != nullptr)
		{
			_matrix ParentMatrix = XMMatrixIdentity();
			ParentMatrix.r[3] = m_pRelativeTransform->Get_WorldMatrix().r[3];

			m_pTransformCom->Set_WorldMatrix(ParentMatrix);
			m_pTransformCom->Set_Scale(XMVectorSet(5.f, 5.f, 5.f, 0.f));
		}

		m_WorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		XMStoreFloat3(&m_vLook, XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 0.01f);
	}

	return S_OK;
}

HRESULT CFX_EffigyFire_MagicCircle::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	return S_OK;
}

HRESULT CFX_EffigyFire_MagicCircle::Render()
{
	if (FAILED(CFX_EffigyFire_MagicCircle::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;

	_bool bBlue = false;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bBlue", &bBlue, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(TEXFX_MAGICCIRCLE)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;


	//m_WorldMatrix._41 += m_vLook.x;
	//m_WorldMatrix._42 += m_vLook.y;
	//m_WorldMatrix._43 += m_vLook.z;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
		return E_FAIL;

	bBlue = true;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bBlue", &bBlue, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(TEXFX_MAGICCIRCLE)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

CFX_EffigyFire_MagicCircle* CFX_EffigyFire_MagicCircle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFX_EffigyFire_MagicCircle* pInstance = new CFX_EffigyFire_MagicCircle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CFX_EffigyFire_MagicCircle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFX_EffigyFire_MagicCircle::Clone(void* pArg)
{
	CFX_EffigyFire_MagicCircle* pInstance = new CFX_EffigyFire_MagicCircle(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CFX_EffigyFire_MagicCircle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFX_EffigyFire_MagicCircle::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}
