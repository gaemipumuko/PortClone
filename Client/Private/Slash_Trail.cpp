#include "stdafx.h"
#include "Slash_Trail.h"

#include "GameInstance.h"

#include "Camera.h"
#include "Player.h"

CSlash_Trail::CSlash_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDeciderObject(pDevice, pContext)
{
}

CSlash_Trail::CSlash_Trail(const CSlash_Trail& rhs)
	: CDeciderObject(rhs)
{
}

HRESULT CSlash_Trail::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Slash_19_12"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	///* For.Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_Flame_Trail"),
		TEXT("Com_Texture"), (CComponent**)&m_pTrailTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSlash_Trail::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vTrailColor, sizeof(_vector))))
		return E_FAIL;

	if (FAILED(m_pTrailTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
		return E_FAIL;

	_float fCamFar = static_cast<CCamera*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"))->Get_CamFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fCamFar, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSlash_Trail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSlash_Trail::Initialize_Clone(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	SLASH_DESC* pDesc = static_cast<SLASH_DESC*>(pArg);
	m_pParentsTransformCom = pDesc->pParentsTransform;
	m_fHeight = pDesc->fHeight;
	m_fTrailSpeed = pDesc->fTrailSpeed;
	m_eTrailType = pDesc->eTrail_Type;
	m_vMoveLook = pDesc->vMoveLook;
	m_fLifeTime = pDesc->fLifeTime;

	if (m_fLifeTime == 0.f)
		m_fLifeTime = 1.f;
	else if(m_fLifeTime < 0.f)
		m_bReCycle = TRUE;

	if (m_fTrailSpeed == 0.f)
		m_fTrailSpeed = 1.f;

	switch (pDesc->eElementColor)
	{
	case Client::ELEMENT_FIRE:
		m_vTrailColor = { 253.f / 255.f,110.f / 255.f,77.f / 255.f ,1.f };
		break;
	case Client::ELEMENT_WATER:
		m_vTrailColor = { 43.f / 255.f,103.f / 255.f,242.f / 255.f ,1.f };
		break;
	case Client::ELEMENT_WIND:
		m_vTrailColor = { 42.f / 255.f,179.f / 255.f,154.f / 255.f ,1.f };
		break;
	case Client::ELEMENT_ELECTRIC:
		m_vTrailColor = { 161.f / 255.f,125.f / 255.f,1.f ,1.f };
		break;
	case Client::ELEMENT_FROST:
		m_vTrailColor = { 141.f / 255.f,219.f / 255.f,1.f ,1.f };
		break;
	case Client::ELEMENT_EARTH:
		m_vTrailColor = { 204.f / 255.f,188.f / 255.f,113.f / 255.f ,1.f };
		break;
	default:
		m_vTrailColor = { 1.f, 1.f, 0.55f, 1.f };
		break;
	}
	if (m_eTrailType == TRAIL_RANDOM)
	{
		_vector vPos = m_pParentsTransformCom->Get_State(CTransform::STATE_POSITION);
		vPos.m128_f32[1] += m_fHeight;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

		_vector vScale = pDesc->vScale;
		if (pDesc->vScale.m128_f32[0] <= 0.f || pDesc->vScale.m128_f32[1] <= 0.f || pDesc->vScale.m128_f32[2] <= 0.f)
			vScale = XMVectorSet(1.f, 1.f, 1.f, 0.f);
		m_pTransformCom->Set_Scale(XMVectorSet(0.05f, 0.05f, 0.01f, 0.f) * vScale);

		_vector vAxisX	= XMVectorSet(1.f, 0.f, 0.f, 0.f);
		_vector vAxisY	= XMVectorSet(0.f, 1.f, 0.f, 0.f);
		_vector vAxisZ	= XMVectorSet(0.f, 0.f, 1.f, 0.f);

		m_pTransformCom->RotationFromCurrentState(vAxisX, XMConvertToRadians(90.f));

		if((pDesc->iExceptAxis & (1 << 0)) == 0)
			m_pTransformCom->RotationFromCurrentState(vAxisX, XMConvertToRadians(m_pGameInstance->Get_Float_Random(0.f, 359.9f)));
		if ((pDesc->iExceptAxis & (1 << 1)) == 0)
			m_pTransformCom->RotationFromCurrentState(vAxisY, XMConvertToRadians(m_pGameInstance->Get_Float_Random(0.f, 359.9f)));
		if ((pDesc->iExceptAxis & (1 << 2)) == 0)
			m_pTransformCom->RotationFromCurrentState(vAxisZ, XMConvertToRadians(m_pGameInstance->Get_Float_Random(0.f, 359.9f)));

		m_pTransformCom->RotationFromCurrentState(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(m_pGameInstance->Get_Float_Random(0.f, 35.f)));

		return S_OK;
	}
	

	_vector vLook = m_pParentsTransformCom->Get_State(CTransform::STATE_LOOK);

	_vector vPos = m_pParentsTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos.m128_f32[1] += m_fHeight;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos + (vLook * m_vMoveLook));

	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vRight = XMVector3Cross(vUp, vLook);

	m_pTransformCom->Look_At_Direction(vLook);

	_vector vScale = pDesc->vScale;
	if (pDesc->vScale.m128_f32[0] <= 0.f || pDesc->vScale.m128_f32[1] <= 0.f || pDesc->vScale.m128_f32[2] <= 0.f)
		vScale = XMVectorSet(1.f, 1.f, 1.f, 0.f);
	m_pTransformCom->Set_Scale(XMVectorSet(0.05f, 0.05f, 0.01f, 0.f) * vScale);
	if (pDesc->bTrailReverse == TRUE)
		m_pTransformCom->RotationFromCurrentState(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	m_pTransformCom->RotationFromCurrentState(vRight,XMConvertToRadians(90.f));
	m_pTransformCom->RotationFromCurrentState(vLook,XMConvertToRadians(pDesc->fAngle));

	return S_OK;
}

HRESULT CSlash_Trail::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CSlash_Trail::Tick(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;
	if (m_fLifeTime <= 0.f)
		Set_Destroy(TRUE);

	m_fTimeAcc += fTimeDelta * m_fTrailSpeed;
	if (m_fTimeAcc >= 1.f && m_bReCycle == TRUE)
	{
		m_fTimeAcc = 0.f;
	}
	


	if (m_eTrailType == TRAIL_RANDOM)
	{
		_vector vPos = m_pParentsTransformCom->Get_State(CTransform::STATE_POSITION);
		vPos.m128_f32[1] += m_fHeight;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	}
	else
	{
		_vector vLook = m_pParentsTransformCom->Get_State(CTransform::STATE_LOOK);
		_vector vPos = m_pParentsTransformCom->Get_State(CTransform::STATE_POSITION);
		vPos.m128_f32[1] += m_fHeight;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos + (vLook * m_vMoveLook));

		//ELEMENT eElement = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")))->Get_Element();

		//switch (meElement)
		//{
		//case Client::ELEMENT_FIRE:
		//	m_vTrailColor = { 253.f / 255.f,110.f / 255.f,77.f / 255.f ,1.f };
		//	break;
		//case Client::ELEMENT_WATER:
		//	m_vTrailColor = { 43.f / 255.f,103.f / 255.f,242.f / 255.f ,1.f };
		//	break;
		//case Client::ELEMENT_WIND:
		//	m_vTrailColor = { 42.f / 255.f,179.f / 255.f,154.f / 255.f ,1.f };
		//	break;
		//case Client::ELEMENT_ELECTRIC:
		//	m_vTrailColor = { 161.f / 255.f,125.f / 255.f,1.f ,1.f };
		//	break;
		//case Client::ELEMENT_FROST:
		//	m_vTrailColor = { 141.f / 255.f,219.f / 255.f,1.f ,1.f };
		//	break;
		//case Client::ELEMENT_EARTH:
		//	m_vTrailColor = { 204.f / 255.f,188.f / 255.f,113.f / 255.f ,1.f };
		//	break;
		//default:
		//	m_vTrailColor = { 1.f, 1.f, 0.55f, 1.f };
		//	break;
		//}
	}

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CSlash_Trail::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	return S_OK;
}

HRESULT CSlash_Trail::Render()
{

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (m_eTrailType == TRAIL_RING)
		{
			if (FAILED(m_pShaderCom->Begin(VTXMESH_TRAIL_CIRCLE)))
				return E_FAIL;

		}
		else
		{
			
			if (FAILED(m_pShaderCom->Begin(VTXMESH_SLASH_DILUC)))
				return E_FAIL;
		}


		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

CSlash_Trail* CSlash_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSlash_Trail* pInstance = new CSlash_Trail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CSlash_Trail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSlash_Trail::Clone(void* pArg)
{
	CSlash_Trail* pInstance = new CSlash_Trail(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CSlash_Trail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSlash_Trail::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTrailTextureCom);
}