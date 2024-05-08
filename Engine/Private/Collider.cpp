#include "Collider.h"

#include "Bounding_OBB.h"
#include "Bounding_AABB.h"
#include "Bounding_SPHERE.h"
#include "Bounding_Frustum.h"

#include "GameInstance.h"

#ifdef _DEBUG
_bool CCollider::m_bDebugRender = { TRUE };
#endif

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CCollider::CCollider(const CCollider& rhs)
	: CComponent(rhs)
	, m_eType(rhs.m_eType)
#ifdef _DEBUG
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif
{
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif
}

_bool CCollider::Intersect(const CCollider* pTargetCollider)
{
	return m_pBounding->Intersect(pTargetCollider->m_eType, pTargetCollider->m_pBounding);
}

_bool CCollider::Intersect(_float3 vOrigin, _float3 vDirection, _float& fDist)
{
	return m_pBounding->Intersect(vOrigin, vDirection, fDist);
}

void CCollider::Update(_fmatrix WorldMatrix)
{
	m_pBounding->Update(WorldMatrix);
}

void CCollider::Update_Desc(void* pBoundingDesc)
{
	CBounding::BOUNDING_DESC* pDesc = static_cast<CBounding::BOUNDING_DESC*>(pBoundingDesc);
	m_pBounding->Update_Desc(pDesc);
}

HRESULT CCollider::Initialize_Prototype(COLLIDER_TYPE eType)
{
	m_eType = eType;

#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<DirectX::VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderBytecode = { nullptr };
	size_t		iLength = { 0 };

	m_pEffect->GetVertexShaderBytecode(&pShaderBytecode, &iLength);

	m_pDevice->CreateInputLayout(DirectX::VertexPositionColor::InputElements, DirectX::VertexPositionColor::InputElementCount, pShaderBytecode, iLength, &m_pInputLayout);
#endif

	return S_OK;
}

HRESULT CCollider::Initialize_Clone(void* pArg)
{
	CBounding::BOUNDING_DESC* pBoundingDesc = (CBounding::BOUNDING_DESC*)pArg;

	switch (m_eType)
	{
	case CTYPE_AABB:
		m_pBounding = CBounding_AABB::Create(pBoundingDesc);
		break;

	case CTYPE_OBB:
		m_pBounding = CBounding_OBB::Create(pBoundingDesc);
		break;

	case CTYPE_SPHERE:
		m_pBounding = CBounding_SPHERE::Create(pBoundingDesc);
		break;
	case CTYPE_FRUSTUM:
		m_pBounding = CBounding_FRUSTUM::Create(pBoundingDesc);
		break;
	}

	return S_OK;
}

#ifdef _DEBUG
HRESULT CCollider::Render()
{
	if (m_bDebugRender == TRUE)
	{
		m_pEffect->SetWorld(XMMatrixIdentity());
		m_pEffect->SetView(m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW));
		m_pEffect->SetProjection(m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ));

		m_pContext->GSSetShader(nullptr, nullptr, 0);

		m_pBatch->Begin();

		m_pContext->IASetInputLayout(m_pInputLayout);
		m_pEffect->Apply(m_pContext);

		m_pBounding->Render(m_pBatch);

		m_pBatch->End();
	}

	return S_OK;
}
#endif

CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER_TYPE eType)
{
	CCollider* pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType)))
	{
		MSG_BOX(TEXT("Failed to Create : CCollider"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCollider::Clone(void* pArg)
{
	CCollider* pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CCollider"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

	Safe_Release(m_pBounding);

#ifdef _DEBUG
	Safe_Release(m_pInputLayout);

	if (m_bCloned == FALSE)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}
#endif
}
