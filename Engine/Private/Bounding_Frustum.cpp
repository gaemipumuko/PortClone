#include "Bounding_Frustum.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_SPHERE.h"
#include "DebugDraw.h"

CBounding_FRUSTUM::CBounding_FRUSTUM()
	: CBounding()
{
}

HRESULT CBounding_FRUSTUM::Update(_fmatrix WorldMatrix)
{
	m_pBoundingDesc_Original->Transform(*m_pBoundingDesc, WorldMatrix);

	return S_OK;
}

_bool CBounding_FRUSTUM::Intersect(CCollider::COLLIDER_TYPE eType, CBounding* pTargetBounding)
{
	m_bColled = false;

	switch (eType)
	{
	case CCollider::CTYPE_AABB:
		m_bColled = m_pBoundingDesc->Intersects(*static_cast<CBounding_AABB*>(pTargetBounding)->Get_BoundingDesc());
		break;
	case CCollider::CTYPE_OBB:
		m_bColled = m_pBoundingDesc->Intersects(*static_cast<CBounding_OBB*>(pTargetBounding)->Get_BoundingDesc());
		break;
	case CCollider::CTYPE_SPHERE:
		m_bColled = m_pBoundingDesc->Intersects(*static_cast<CBounding_SPHERE*>(pTargetBounding)->Get_BoundingDesc());
		break;
	case CCollider::CTYPE_FRUSTUM:
		m_bColled = m_pBoundingDesc->Intersects(*static_cast<CBounding_FRUSTUM*>(pTargetBounding)->Get_BoundingDesc());
		break;
	}

	return m_bColled;
}

_bool CBounding_FRUSTUM::Intersect(_float3 vOrigin, _float3 vDirection, _float& fDist)
{
	m_bColled = false;

	m_bColled = m_pBoundingDesc->Intersects(XMLoadFloat3(&vOrigin), XMLoadFloat3(&vDirection), fDist);

	return m_bColled;
}

HRESULT CBounding_FRUSTUM::Initialize(BOUNDING_DESC* pBoundingDesc)
{
	BOUNDING_FRUSTUM_DESC* pDesc = (BOUNDING_FRUSTUM_DESC*)pBoundingDesc;

	_float4x4 vMatrix;
	//XMStoreFloat4x4(&vMatrix, XMMatrixMultiply(XMLoadFloat4x4(&pDesc->vViewMatrix), XMLoadFloat4x4(&pDesc->vProjMatrix)));
	XMStoreFloat4x4(&vMatrix, XMLoadFloat4x4(&pDesc->vProjMatrix));

	m_pBoundingDesc = new BoundingFrustum(XMLoadFloat4x4(&vMatrix), false);
	m_pBoundingDesc_Original = new BoundingFrustum(*m_pBoundingDesc);

	return S_OK;
}

#ifdef _DEBUG
HRESULT CBounding_FRUSTUM::Render(PrimitiveBatch<DirectX::VertexPositionColor>* pBatch)
{
	DX::Draw(pBatch, *m_pBoundingDesc, Compute_Color());

	return S_OK;
}
#endif

CBounding_FRUSTUM* CBounding_FRUSTUM::Create(BOUNDING_DESC* pBoundingDesc)
{
	CBounding_FRUSTUM* pInstance = new CBounding_FRUSTUM();

	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX(TEXT("Failed to Create : CBounding_FRUSTUM"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_FRUSTUM::Free()
{
	__super::Free();

	Safe_Delete(m_pBoundingDesc);
	Safe_Delete(m_pBoundingDesc_Original);
}
