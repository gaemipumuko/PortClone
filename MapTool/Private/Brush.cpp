#include "Brush.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Renderer.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "Camera.h"

CBrush::CBrush()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CBrush::Initialize()
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Rotation(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), XMConvertToRadians(90.0f));

	return S_OK;
}

HRESULT CBrush::Render()
{
	if (true == m_bActivate)
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CBrush::Invalidate_Brush(_float4* pBrushPos)
{
	if (nullptr == pBrushPos)
		return E_FAIL;

	pBrushPos->y += 0.1f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(pBrushPos));

	return S_OK;
}

HRESULT CBrush::Resize_Brush(_float fScale)
{
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	_float3 vScale = _float3(fScale * 2.0f, fScale * 2.0f, 1.0f);

	m_pTransformCom->Set_Scale(XMLoadFloat3(&vScale));

	return S_OK;
}

HRESULT CBrush::Add_Components()
{
	/* For.Com_Transform */
	m_pTransformCom = (CTransform*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"));

	/* For.Com_Renderer */
	m_pRendererCom = (CRenderer*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"));

	/* For.Com_Shader */
	m_pShaderCom = (CShader*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"));

	/* For.Com_Texture */
	m_pTextureCom = (CTexture*)m_pGameInstance->Clone_Component_Prototype(LEVEL_EDIT, TEXT("Prototype_Component_Texture_Brush"));

	/* For.Com_VIBuffer */
	m_pVIBufferCom = (CVIBuffer_Rect*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"));

	return S_OK;
}

HRESULT CBrush::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	//_float fCamFar = static_cast<CCamera*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"))->Get_CamFar();

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fCamFar, sizeof(_float))))
	//	return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

CBrush* CBrush::Create()
{
	CBrush* pInstance = new CBrush();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CBrush"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBrush::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
