#include "Bounding_OBB.h"
#include "Bounding_AABB.h"
#include "Bounding_SPHERE.h"
#include "Bounding_Frustum.h"
#include "DebugDraw.h"

CBounding_SPHERE::CBounding_SPHERE()
	: CBounding()
{
}

HRESULT CBounding_SPHERE::Update(_fmatrix WorldMatrix)
{
	m_pBoundingDesc_Original->Transform(*m_pBoundingDesc, WorldMatrix);

	return S_OK;
}

HRESULT CBounding_SPHERE::Update_Desc(BOUNDING_DESC* pDesc)
{
	BOUNDING_SPHERE_DESC* pSphereDesc = (BOUNDING_SPHERE_DESC*)pDesc;

	m_pBoundingDesc_Original->Radius = pSphereDesc->fRadius;
	m_pBoundingDesc_Original->Center = pSphereDesc->vCenter;

	return S_OK;
}

_bool CBounding_SPHERE::Intersect(CCollider::COLLIDER_TYPE eType, CBounding* pTargetBounding)
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

_bool CBounding_SPHERE::Intersect(_float3 vOrigin, _float3 vDirection, _float& fDist)
{
	m_bColled = false;

	m_bColled = m_pBoundingDesc->Intersects(XMLoadFloat3(&vOrigin), XMLoadFloat3(&vDirection), fDist);

	return m_bColled;
}

HRESULT CBounding_SPHERE::Initialize(BOUNDING_DESC* pBoundingDesc)
{
	BOUNDING_SPHERE_DESC* pDesc = (BOUNDING_SPHERE_DESC*)pBoundingDesc;

	m_pBoundingDesc = new BoundingSphere(pDesc->vCenter, pDesc->fRadius);
	m_pBoundingDesc_Original = new BoundingSphere(*m_pBoundingDesc);

	return S_OK;
}

#ifdef _DEBUG
HRESULT CBounding_SPHERE::Render(PrimitiveBatch<DirectX::VertexPositionColor>* pBatch)
{
	DX::Draw(pBatch, *m_pBoundingDesc, Compute_Color());
	
	return S_OK;
}
#endif

CBounding_SPHERE* CBounding_SPHERE::Create(BOUNDING_DESC* pBoundingDesc)
{
	CBounding_SPHERE* pInstance = new CBounding_SPHERE();

	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX(TEXT("Failed to Create : CBounding_SPHERE"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_SPHERE::Free()
{
	__super::Free();

	Safe_Delete(m_pBoundingDesc);
	Safe_Delete(m_pBoundingDesc_Original);
}
