#include "Bounding_OBB.h"
#include "Bounding_AABB.h"
#include "Bounding_SPHERE.h"
#include "Bounding_Frustum.h"
#include "DebugDraw.h"

CBounding_AABB::CBounding_AABB()
	: CBounding()
{
}

HRESULT CBounding_AABB::Update(_fmatrix WorldMatrix)
{
	_matrix		TransformMatrix = WorldMatrix;

	TransformMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[0]));
	TransformMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[1]));
	TransformMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[2]));

	m_pBoundingDesc_Original->Transform(*m_pBoundingDesc, TransformMatrix);

	return S_OK;
}

_bool CBounding_AABB::Intersect(CCollider::COLLIDER_TYPE eType, CBounding* pTargetBounding)
{
	m_bColled = FALSE;

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

_bool CBounding_AABB::Intersect(_float3 vOrigin, _float3 vDirection, _float& fDist)
{
	m_bColled = false;

	m_bColled = m_pBoundingDesc->Intersects(XMLoadFloat3(&vOrigin), XMLoadFloat3(&vDirection), fDist);

	return m_bColled;
}

HRESULT CBounding_AABB::Initialize(BOUNDING_DESC* pBoundingDesc)
{
	BOUNDING_AABB_DESC* pDesc = static_cast<BOUNDING_AABB_DESC*>(pBoundingDesc);

	m_pBoundingDesc = new BoundingBox(pDesc->vCenter, pDesc->vExtents);
	m_pBoundingDesc_Original = new BoundingBox(*m_pBoundingDesc);

	return S_OK;
}

#ifdef _DEBUG
HRESULT CBounding_AABB::Render(PrimitiveBatch<DirectX::VertexPositionColor>* pBatch)
{
	DX::Draw(pBatch, *m_pBoundingDesc, Compute_Color());

	return S_OK;
}
#endif

CBounding_AABB* CBounding_AABB::Create(BOUNDING_DESC* pBoundingDesc)
{
	CBounding_AABB* pInstance = new CBounding_AABB();

	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX(TEXT("Failed to Create : CBounding_AABB"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_AABB::Free()
{
	__super::Free();

	Safe_Delete(m_pBoundingDesc);
	Safe_Delete(m_pBoundingDesc_Original);
}
