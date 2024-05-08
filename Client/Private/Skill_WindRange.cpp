#include "stdafx.h"
#include "Skill_WindRange.h"

#include "GameInstance.h"

#include "Andrius.h"
#include "Camera.h"

#include "Effect.h"

CSkill_WindRange::CSkill_WindRange(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkillObj(pDevice, pContext)
{
}

CSkill_WindRange::CSkill_WindRange(const CSkill_WindRange& rhs)
	: CSkillObj(rhs)
{
}

HRESULT CSkill_WindRange::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_WindRange"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkill_WindRange::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
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

HRESULT CSkill_WindRange::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkill_WindRange::Initialize_Clone(void* pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		SKILLOBJ_DESC* pDesc = static_cast<SKILLOBJ_DESC*>(pArg);

		m_eSkill = pDesc->eSkillObjID;
		m_vColor = pDesc->vColor;

		m_fLifeTime = pDesc->fLifeTime;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vSpawnPos);
		m_pTransformCom->Set_Scale(pDesc->vScale);
	}
	else
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	if(m_vColor.m128_f32[3]==0.f)
		m_vColor = { 0.59215686274f, 0.82352941176f, 0.94117647058f, 0.f };

	return S_OK;
}

HRESULT CSkill_WindRange::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CSkill_WindRange::Tick(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;


	switch (m_eSkill)
	{
	case SKILLOBJ_WINDRANGE_ICECONE:
	{
		m_vScaleUV = { 3.f, 1.f };
		m_vSpinUV.x -= fTimeDelta * 1.5f;
		m_vSpinUV.y += fTimeDelta / 3;

		_vector vSelfScale = m_pTransformCom->Get_Scale();
		vSelfScale.m128_f32[0] += fTimeDelta / 60.f;
		//vSelfScale.m128_f32[1] *= 1.0f;
		vSelfScale.m128_f32[2] += fTimeDelta / 60.f;
		m_pTransformCom->Set_Scale(vSelfScale);
		m_pTransformCom->Go_Down(fTimeDelta * 0.089f);
	}
		break;
	case SKILLOBJ_WINDRANGE_SWEEP:
	{
		m_vScaleUV = { 3.f, 1.f };
		m_vSpinUV.x -= fTimeDelta * 1.5f;
		//m_vSpinUV.y += fTimeDelta * 1.f;

		_vector vSelfScale = m_pTransformCom->Get_Scale();
		vSelfScale.m128_f32[0] += fTimeDelta / 15.f;
		//vSelfScale.m128_f32[1] *= fTimeDelta / 10.f;
		vSelfScale.m128_f32[2] += fTimeDelta / 15.f;
		m_pTransformCom->Set_Scale(vSelfScale);
		m_pTransformCom->Go_Down(fTimeDelta * 0.08f);
	}
	break;
	case SKILLOBJ_WINDRANGE_JUMP:
	{
		m_vScaleUV = { 3.f, 1.f };
		m_vSpinUV.x -= fTimeDelta * 1.5f;
		//m_vSpinUV.y += fTimeDelta * 1.f;

		_vector vSelfScale = m_pTransformCom->Get_Scale(); 
		vSelfScale.m128_f32[0] += fTimeDelta / 15.f;
		//vSelfScale.m128_f32[1] *= fTimeDelta / 10.f;
		vSelfScale.m128_f32[2] += fTimeDelta / 15.f;
		m_pTransformCom->Set_Scale(vSelfScale);
		m_pTransformCom->Go_Down(fTimeDelta * 0.08f);
	}
	break;
	case SKILLOBJ_WINDRANGE_JUMP_CENTER:
	{
		m_vScaleUV = { 3.f, 1.f };
		m_vSpinUV.x -= fTimeDelta * 1.5f;
		//m_vSpinUV.y += fTimeDelta * 1.f;

		_vector vSelfScale = m_pTransformCom->Get_Scale(); 
		vSelfScale.m128_f32[0] += fTimeDelta / 15.f;
		//vSelfScale.m128_f32[1] *= fTimeDelta / 10.f;
		vSelfScale.m128_f32[2] += fTimeDelta / 15.f;
		m_pTransformCom->Set_Scale(vSelfScale);
		m_pTransformCom->Go_Down(fTimeDelta * 0.03f);
	}
	break;
	case SKILLOBJ_WINDRANGE_HOWL:
	{
		m_vScaleUV = { 3.f, 1.f };
		m_vSpinUV.x -= fTimeDelta * 1.5f;
		//m_vSpinUV.y += fTimeDelta * 1.f;

		_vector vSelfScale = m_pTransformCom->Get_Scale();
		vSelfScale.m128_f32[0] += fTimeDelta / 50.f;
		vSelfScale.m128_f32[1] += fTimeDelta / 50.f;
		vSelfScale.m128_f32[2] += fTimeDelta / 50.f;
		m_pTransformCom->Set_Scale(vSelfScale);
		m_pTransformCom->Go_Down(fTimeDelta * 0.05f);
	}
	break;
	case SKILLOBJ_WINDRANGE_HAIL:
	{
		m_vScaleUV = { 3.f, 1.f };
		m_vSpinUV.x -= fTimeDelta * 1.5f;
		//m_vSpinUV.y += fTimeDelta * 1.f;

		if (m_fLifeTime <= 3.f)
		{
			m_pTransformCom->Go_Down(fTimeDelta * 0.08f);
		}
		else
		{
			_vector vSelfScale = m_pTransformCom->Get_Scale();
			vSelfScale.m128_f32[0] += fTimeDelta / 50.f;
			vSelfScale.m128_f32[2] += fTimeDelta / 50.f;
			m_pTransformCom->Set_Scale(vSelfScale);
		}
	}
	break;
	case SKILLOBJ_WIND_DEFINED:
	{
		m_vScaleUV = { 3.f + fTimeDelta*4.f, 1.f };
		m_vSpinUV.x -= fTimeDelta * 1.5f;
		//m_vSpinUV.y += fTimeDelta * 1.f;

			m_pTransformCom->Go_Down(fTimeDelta * 0.3f);
			_vector vSelfScale = m_pTransformCom->Get_Scale();
			vSelfScale.m128_f32[0] += fTimeDelta / 3.f;
			vSelfScale.m128_f32[2] += fTimeDelta / 3.f;
			m_pTransformCom->Set_Scale(vSelfScale);

	}
	break;
	default:
		break;
	}
	
	if(m_fLifeTime <= 0.f)
		Set_Destroy(TRUE);


	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CSkill_WindRange::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	return S_OK;
}

HRESULT CSkill_WindRange::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVScaleRate", &m_vScaleUV, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVSpinRate", &m_vSpinUV, sizeof(_float2))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;
		
			if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_vector))))
				return E_FAIL;

		/*if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;*/

		if (FAILED(m_pShaderCom->Begin(VTXMESH_WIND_RANGE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

CSkill_WindRange* CSkill_WindRange::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_WindRange* pInstance = new CSkill_WindRange(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CSkill_WindRange"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_WindRange::Clone(void* pArg)
{
	CSkill_WindRange* pInstance = new CSkill_WindRange(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CSkill_WindRange"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_WindRange::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}