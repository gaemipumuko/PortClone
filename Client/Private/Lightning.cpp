#include "stdafx.h"
#include "Lightning.h"

#include "Camera.h"
#include "GameInstance.h"

CLightning::CLightning(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkillObj(pDevice, pContext)
{
}

CLightning::CLightning(const CLightning& rhs)
	: CSkillObj(rhs)
{
}

HRESULT CLightning::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Lightning"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLightning::Bind_ShaderResources()
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

	return S_OK;
}

HRESULT CLightning::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLightning::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(CLightning::Add_Component(pArg)))
		return E_FAIL;

	if (pArg == nullptr)
		return E_FAIL;

	SKILLOBJ_DESC* pDesc = (SKILLOBJ_DESC*)pArg;
	m_fLifeTime = pDesc->fLifeTime;
	
	_vector vScale = XMVectorSet(0.01f, 0.01f, 0.01f, 0.f) * pDesc->vScale;
	m_pTransformCom->Set_Scale(vScale);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vSpawnPos);
	
	m_pTransformCom->Look_At_Direction(pDesc->vDir);

	_float fLength = XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
	_float fVolume = min(0.25f / fLength, 0.25f);

	m_pGameInstance->PlaySoundW(L"Lightning1.mp3", fVolume);

	return S_OK;
}

HRESULT CLightning::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CLightning::Tick(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;
	m_fFrame += fTimeDelta * 40.f;
	m_fSoundDelay += fTimeDelta;

	if (m_fSoundDelay > 0.5f)
	{
		m_fSoundDelay = 0.f;
		_float fLength = XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
		_float fVolume = 1.f / fLength;
		if (fVolume > 1.f)
			fVolume = 1.f;
		m_pGameInstance->PlaySoundW(L"Lightning1.mp3", fVolume);
	}

	if (m_fLifeTime <= 0.f)
		Set_Destroy(TRUE);

	if (m_fFrame >= 1.f)
	{
		m_fFrame = 0.f;
		m_iMesIndex = m_pGameInstance->Get_Int_Random(0, 3);
	}

	return S_OK;
}

HRESULT CLightning::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	return S_OK;
}

HRESULT CLightning::Render()
{
	if (FAILED(CLightning::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(VTXMESH_LIGHTNING)))
		return E_FAIL;

	if (FAILED(m_pModelCom->Render(m_iMesIndex)))
		return E_FAIL;

	return S_OK;
}

CLightning* CLightning::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLightning* pInstance = new CLightning(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CLightning"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLightning::Clone(void* pArg)
{
	CLightning* pInstance = new CLightning(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CLightning"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLightning::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
