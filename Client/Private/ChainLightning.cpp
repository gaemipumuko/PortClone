#include "stdafx.h"
#include "ChainLightning.h"

#include "GameInstance.h"

CChainLightning::CChainLightning(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CChainLightning::CChainLightning(const CChainLightning& rhs)
	: CEffect(rhs)
{
}

HRESULT CChainLightning::Add_Component(void* pArg)
{
	__super::Add_Component(pArg);

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ChainLightning"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CChainLightning::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &m_iFrameIndex, sizeof(_int))))
		return E_FAIL;

	return S_OK;
}

HRESULT CChainLightning::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CChainLightning::Initialize_Clone(void* pArg)
{
	if (FAILED(CChainLightning::Add_Component(pArg)))
		return E_FAIL;

	if (pArg == nullptr)
		return E_FAIL;

	CHAIN_LIGHTNING_DESC* pDesc = (CHAIN_LIGHTNING_DESC*)pArg;

	_vector vPointA = pDesc->vPointA;
	_vector vPointB = pDesc->vPointB;

	_matrix Matrix = {};
	_vector vRight = XMVector3Normalize(vPointA - vPointB);
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vLook = XMVector3Cross(vUp, vRight);
	vUp = XMVector3Cross(vLook, vRight);

	Matrix.r[0] = vRight;
	Matrix.r[1] = vUp;
	Matrix.r[2] = vLook;
	Matrix.r[3] = (vPointA + vPointB) * 0.5f;
	m_pTransformCom->Set_WorldMatrix(Matrix);

	_float  fLength = XMVectorGetX(XMVector3Length(vPointA - vPointB));
	m_pTransformCom->Set_Scale(XMVectorSet(fLength, 1.f, 1.f, 0.f));

	m_iFrameIndex = m_pGameInstance->Get_Int_Random(0, 3);

	if(m_iFrameIndex <= 1)
		m_pGameInstance->PlaySoundW(L"Lightning1.mp3", 0.5f);
	else
		m_pGameInstance->PlaySoundW(L"Lightning2.mp3", 0.5f);

	return S_OK;
}

HRESULT CChainLightning::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CChainLightning::Tick(_float fTimeDelta)
{
	m_fLifeTime += fTimeDelta;
	m_fFrame += fTimeDelta * 40.f;
	if (m_fFrame >= 1.f)
	{
		m_fFrame = 0.f;
		m_iFrameIndex = m_pGameInstance->Get_Int_Random(0, 3);
	}
	if (m_fLifeTime > 0.3f)
		Set_Destroy(TRUE);

	return S_OK;
}

HRESULT CChainLightning::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	return S_OK;
}

HRESULT CChainLightning::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(VTXTEX_CHIN_LIGHTNING)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	m_pTransformCom->RotationFromCurrentState(vRight, XMConvertToRadians(90.f));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(VTXTEX_CHIN_LIGHTNING)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	m_pTransformCom->RotationFromCurrentState(vRight, XMConvertToRadians(-90.f));

	return S_OK;
}

CChainLightning* CChainLightning::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CChainLightning* pInstance = new CChainLightning(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CChainLightning"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CChainLightning::Clone(void* pArg)
{
	CChainLightning* pInstance = new CChainLightning(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CChainLightning"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChainLightning::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
