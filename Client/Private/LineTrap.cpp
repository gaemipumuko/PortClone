#include "stdafx.h"
#include "LineTrap.h"
#include "Player.h"
#include "GameInstance.h"

CLineTrap::CLineTrap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CLineTrap::CLineTrap(const CLineTrap& rhs)
	: CEffect(rhs)
{
}

HRESULT CLineTrap::Add_Component(void* pArg)
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

	return S_OK;
}

HRESULT CLineTrap::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLineTrap::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLineTrap::Initialize_Clone(void* pArg)
{
	if (FAILED(CLineTrap::Add_Component(pArg)))
		return E_FAIL;

	if (pArg == nullptr)
		return E_FAIL;

	LINE_TRAP_DESC* pDesc = (LINE_TRAP_DESC*)pArg;
	
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
	m_pTransformCom->Set_Scale(XMVectorSet(fLength, 0.05f, 0.05f, 0.f));

	m_fMaxSpeed = pDesc->fSpeed;
	m_fAccel = pDesc->fAccel;
	m_fMaxMoveRange = pDesc->fMoveRange;
	XMStoreFloat3(&m_vDir, pDesc->vStartDir);

	/* For.Com_Collider */
	CBounding_OBB::BOUNDING_OBB_DESC tColliderDesc = {};
	tColliderDesc.vExtents = _float3(1.f, 1.f, 1.f);
	tColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), (CComponent**)&m_pCollider, &tColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLineTrap::PreviousTick(_float fTimeDelta)
{
	if (m_fMoveRange < m_fMaxMoveRange)
	{
		if (m_fAccel == 0.f)
			m_fSpeed = m_fMaxSpeed;
		else if (m_fSpeed < m_fMaxSpeed)
		{
			m_fSpeed += fTimeDelta * m_fAccel;
			if (m_fSpeed > m_fMaxSpeed)
				m_fSpeed = m_fMaxSpeed;
		}

		_vector vDir = XMLoadFloat3(&m_vDir);
		_vector vOverMove = {};
		_float	fSpeed = m_fSpeed;
		m_fMoveRange += fTimeDelta * fSpeed;
		if (m_fMoveRange >= m_fMaxMoveRange)
		{
			vOverMove = vDir * (m_fMoveRange - m_fMaxMoveRange) * -1.f;
			m_fMoveRange = 0.f;
			m_vDir.x *= -1.f;
			m_vDir.y *= -1.f;
			m_vDir.z *= -1.f;
			if(m_fAccel != 0.f)
				m_fSpeed = 0.f;
		}
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (vDir * fTimeDelta * fSpeed) + vOverMove;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	}

	return S_OK;
}

HRESULT CLineTrap::Tick(_float fTimeDelta)
{
	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	WorldMatrix.r[0] *= 0.5f;
	WorldMatrix.r[1] *= 0.5f;
	WorldMatrix.r[2] *= 0.5f;
	m_pCollider->Update(WorldMatrix);

	vector<CGameObject*> Hit = m_pGameInstance->Find_Collision_Objects(m_pCollider, COLL_EVENTREACT_PLAYER);
	if (Hit.empty() == FALSE)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Player"));
		_vector vPlayerPos = pPlayer->Get_Position();
		_vector vLinePos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float4 vDir = {};
		XMStoreFloat4(&vDir, XMVector4Normalize(vPlayerPos - vLinePos));
		pPlayer->Set_Damage(1557.f, ELEMENT_END, FSM_HIT_THROW, 3.f, 20.f, vDir, 0.5f);
	}

	return S_OK;
}

HRESULT CLineTrap::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	return S_OK;
}

HRESULT CLineTrap::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(VTXTEX_LINE_TRAP)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	m_pTransformCom->RotationFromCurrentState(vRight, XMConvertToRadians(90.f));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(VTXTEX_LINE_TRAP)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

CLineTrap* CLineTrap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLineTrap* pInstance = new CLineTrap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CLineTrap"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLineTrap::Clone(void* pArg)
{
	CLineTrap* pInstance = new CLineTrap(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CLineTrap"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLineTrap::Free()
{
	__super::Free();

	Safe_Release(m_pCollider);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
