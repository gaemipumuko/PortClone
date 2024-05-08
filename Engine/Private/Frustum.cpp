#include "Frustum.h"

#include "GameInstance.h"

CFrustum::CFrustum()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CFrustum::Make_Plane(const _float3* pPoints, _Inout_ _float4* pPlanes)
{
	XMStoreFloat4(&pPlanes[0], XMPlaneFromPoints(XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[6])));
	XMStoreFloat4(&pPlanes[1], XMPlaneFromPoints(XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[0]), XMLoadFloat3(&pPoints[3])));
	XMStoreFloat4(&pPlanes[2], XMPlaneFromPoints(XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[1])));
	XMStoreFloat4(&pPlanes[3], XMPlaneFromPoints(XMLoadFloat3(&pPoints[3]), XMLoadFloat3(&pPoints[2]), XMLoadFloat3(&pPoints[6])));
	XMStoreFloat4(&pPlanes[4], XMPlaneFromPoints(XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[7])));
	XMStoreFloat4(&pPlanes[5], XMPlaneFromPoints(XMLoadFloat3(&pPoints[0]), XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&pPoints[2])));

	return S_OK;
}

_bool CFrustum::PointInFrustum_InLocal(_fvector vLocalPos, _float fRange)
{
	for (_int iIndex = 0; iIndex < 6; ++iIndex)
	{
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vLocalPlanes[iIndex]), vLocalPos)))
			return FALSE;
	}

	return TRUE;
}

_bool CFrustum::PointInFrustum_InWorld(_fvector vWorldPos, _float fRange)
{
	for (_int iIndex = 0; iIndex < 6; ++iIndex)
	{
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vWorldPlanes[iIndex]), vWorldPos)))
			return FALSE;
	}

	return TRUE;
}

void CFrustum::Transform_LocalSpace(_fmatrix WorldMatrixInv)
{
	_float3			vLocalPoints[8];

	for (size_t i = 0; i < 8; i++)
		XMStoreFloat3(&vLocalPoints[i], XMVector3TransformCoord(XMLoadFloat3(&m_vWorldPoints[i]), WorldMatrixInv));

	if (FAILED(Make_Plane(vLocalPoints, m_vLocalPlanes)))
		return;
}

HRESULT CFrustum::Initialize()
{
	m_vPoints[0] = _float3(-1.f, 1.f, 0.f);
	m_vPoints[1] = _float3(1.f, 1.f, 0.f);
	m_vPoints[2] = _float3(1.f, -1.f, 0.f);
	m_vPoints[3] = _float3(-1.f, -1.f, 0.f);

	m_vPoints[4] = _float3(-1.f, 1.f, 1.f);
	m_vPoints[5] = _float3(1.f, 1.f, 1.f);
	m_vPoints[6] = _float3(1.f, -1.f, 1.f);
	m_vPoints[7] = _float3(-1.f, -1.f, 1.f);

	return S_OK;
}

HRESULT CFrustum::Tick(_float fTimeDelta)
{
	for (size_t i = 0; i < 8; i++)
	{
		/* 뷰스페이스로의 변환. */
		XMStoreFloat3(&m_vWorldPoints[i], XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[i]), m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_PROJ)));
		/* 월드스페이스로의 변환. */
		XMStoreFloat3(&m_vWorldPoints[i], XMVector3TransformCoord(XMLoadFloat3(&m_vWorldPoints[i]), m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_VIEW)));
	}

	if (FAILED(Make_Plane(m_vWorldPoints, m_vWorldPlanes)))
		return E_FAIL;

	return S_OK;
}

CFrustum* CFrustum::Create()
{
	CFrustum* pInstance = new CFrustum();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CFrustum"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFrustum::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
