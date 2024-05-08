#include "stdafx.h"
#include "FlameSlash_Trail.h"

#include "GameInstance.h"

#include "Camera.h"

#include "Effect.h"

CFlameSlash_Trail::CFlameSlash_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDeciderObject(pDevice, pContext)
{
}

CFlameSlash_Trail::CFlameSlash_Trail(const CFlameSlash_Trail& rhs)
	: CDeciderObject(rhs)
{
}

HRESULT CFlameSlash_Trail::Add_Component(void* pArg)
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Slash_19_12"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	///* For.Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_Flame_Trail"),
		TEXT("Com_Texture"), (CComponent**)&m_pTrailTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFlameSlash_Trail::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pTrailTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	_float fCamFar = static_cast<CCamera*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"))->Get_CamFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fCamFar, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CFlameSlash_Trail::Create_Flame()
{
	for (_uint i = 0; i < 5; ++i)
	{
		if (m_iFlameNum < 22)
			m_iFlameNum++;
		else
			return S_OK;

		CEffect::EFFECT_DESC tDesc = {};

		_int iFlametype = m_iFlameNum % 3;
		switch (iFlametype)
		{
		case 0:
			tDesc.eEffectID = EFFECT_FLAME0;
			break;
		case 1:
			tDesc.eEffectID = EFFECT_FLAME1;
			break;
		case 2:
			tDesc.eEffectID = EFFECT_FLAME2;
			break;
		default:
			break;
		}

		tDesc.fLifeTime = 1.f;

		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vRight = XMVector3Normalize(m_pParentsTransformCom->Get_State(CTransform::STATE_RIGHT));
		_vector vLook = XMVector3Normalize(m_pParentsTransformCom->Get_State(CTransform::STATE_LOOK));
		_matrix RotationMatrix1 = {};

		if(m_bTrailReverse == FALSE)
			RotationMatrix1 = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(270.f + (_float)m_iFlameNum * 10.f));
		else
			RotationMatrix1 = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-270.f + (_float)m_iFlameNum * -10.f));


		_matrix RotationMatrix2 = XMMatrixRotationAxis(vLook, XMConvertToRadians(m_fAngle));
		_vector vDir = XMVector3Transform(vRight, RotationMatrix1);
		vDir = XMVector3Transform(vDir, RotationMatrix2) * 2.f;

		vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vDir;
		vPos.m128_f32[3] = 1.f;

		_float2 vRand = {};
		vRand.x = m_pGameInstance->Get_Float_Random(1.f, 1.2f);
		vRand.y = m_pGameInstance->Get_Float_Random(1.f, 1.2f);

		tDesc.vSpawnPos = vPos;
		tDesc.vScale = { vRand.x, vRand.y, 1.f, 0.f };

		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Trail_Flame", L"Layer_Effect", &tDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CFlameSlash_Trail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFlameSlash_Trail::Initialize_Clone(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	FLAMESLASH_DESC* pDesc = static_cast<FLAMESLASH_DESC*>(pArg);
	m_pParentsTransformCom = pDesc->pParentsTransform;
	m_fHeight = pDesc->fHeight;
	m_fAngle = pDesc->fAngle;
	m_fTrailSpeed = pDesc->fTrailSpeed;
	m_bTrailReverse = pDesc->bTrailReverse;

	_vector vPos = m_pParentsTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos.m128_f32[1] += m_fHeight;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	_vector vLook = m_pParentsTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vRight = XMVector3Cross(vUp, vLook);

	m_pTransformCom->Look_At_Direction(vLook);

	m_pTransformCom->Set_Scale({ 0.05f, 0.05f, 0.01f, 0.f });
	if (pDesc->bTrailReverse == TRUE)
		m_pTransformCom->RotationFromCurrentState(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	m_pTransformCom->RotationFromCurrentState(vRight, XMConvertToRadians(90.f));

	m_pTransformCom->RotationFromCurrentState(vLook, XMConvertToRadians(pDesc->fAngle));


	m_fLifeTime = 1.f;

	return S_OK;
}

HRESULT CFlameSlash_Trail::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CFlameSlash_Trail::Tick(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;
	if(m_fLifeTime <= 0.f)
		Set_Destroy(TRUE);

	m_fTimeAcc += fTimeDelta * m_fTrailSpeed;


	_vector vPos = m_pParentsTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos.m128_f32[1] += m_fHeight;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	CFlameSlash_Trail::Create_Flame();

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CFlameSlash_Trail::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_EFFECT, this);

	return S_OK;
}

HRESULT CFlameSlash_Trail::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;


		if (FAILED(m_pShaderCom->Begin(VTXMESH_DILUC_OUTER_FLAME_SLASH)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(VTXMESH_DILUC_FLAME_SLASH)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;


	}

	return S_OK;
}

CFlameSlash_Trail* CFlameSlash_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFlameSlash_Trail* pInstance = new CFlameSlash_Trail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CFlameSlash_Trail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFlameSlash_Trail::Clone(void* pArg)
{
	CFlameSlash_Trail* pInstance = new CFlameSlash_Trail(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CFlameSlash_Trail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFlameSlash_Trail::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTrailTextureCom);
}