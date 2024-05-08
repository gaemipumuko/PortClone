#include "Bounding_OBB.h"
#include "Bounding_AABB.h"
#include "Bounding_SPHERE.h"
#include "Bounding_Frustum.h"
#include "DebugDraw.h"

CBounding_OBB::CBounding_OBB()
	: CBounding()
{
}

HRESULT CBounding_OBB::Update(_fmatrix WorldMatrix)
{
	m_pBoundingDesc_Original->Transform(*m_pBoundingDesc, WorldMatrix);

	return S_OK;
}

_bool CBounding_OBB::Intersect(CCollider::COLLIDER_TYPE eType, CBounding* pTargetBounding)
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

_bool CBounding_OBB::Intersect(_float3 vOrigin, _float3 vDirection, _float& fDist)
{
	m_bColled = false;

	m_bColled = m_pBoundingDesc->Intersects(XMLoadFloat3(&vOrigin), XMLoadFloat3(&vDirection), fDist);

	return m_bColled;
}

HRESULT CBounding_OBB::Initialize(BOUNDING_DESC* pBoundingDesc)
{
	BOUNDING_OBB_DESC* pDesc = static_cast<BOUNDING_OBB_DESC*>(pBoundingDesc);

	_float4	vRotation;
	XMStoreFloat4(&vRotation, XMQuaternionRotationRollPitchYaw(pDesc->vRadians.x, pDesc->vRadians.y, pDesc->vRadians.z));
	
	m_pBoundingDesc = new BoundingOrientedBox(pDesc->vCenter, pDesc->vExtents, vRotation);
	m_pBoundingDesc_Original = new BoundingOrientedBox(*m_pBoundingDesc);

	return S_OK;
}

#ifdef _DEBUG
HRESULT CBounding_OBB::Render(PrimitiveBatch<DirectX::VertexPositionColor>* pBatch)
{
	DX::Draw(pBatch, *m_pBoundingDesc, Compute_Color());

	return S_OK;
}
#endif

CBounding_OBB* CBounding_OBB::Create(BOUNDING_DESC* pBoundingDesc)
{
	CBounding_OBB* pInstance = new CBounding_OBB();

	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX(TEXT("Failed to Create : CBounding_OBB"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_OBB::Free()
{
	__super::Free();

	Safe_Delete(m_pBoundingDesc);
	Safe_Delete(m_pBoundingDesc_Original);
}
