#include "Ray.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Transform.h"
#include "VIBuffer_Terrain.h"

CRay::CRay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRay::Initialize(_float3 vRayPos, _float3 vRayDir)
{
	m_vRayPos = vRayPos;
	m_vRayDir = vRayDir;

	return S_OK;
}

CRay* CRay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _float3 vRayPos, _float3 vRayDir)
{
	CRay* pInstance = new CRay(pDevice, pContext);

	if (FAILED(pInstance->Initialize(vRayPos, vRayDir)))
	{
		MSG_BOX(TEXT("Failed to Created : CRay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRay* CRay::ScreenPointToRay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _float2 vScreenPoint)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_uint iNumViewPorts = 1;
	D3D11_VIEWPORT ViewPortDesc = {};

	_float4x4 vViewMatrixInv = pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_VIEW);
	_float4x4 vProjMatrixInv = pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_PROJ);

	_float3 vRayPos_InWorld;
	_float3 vRayDir_InWorld;

	pContext->RSGetViewports(&iNumViewPorts, &ViewPortDesc);

	_float3 vOrigin = _float3(0.0f, 0.0f, 0.0f);
	_float3 vPoint = _float3(vScreenPoint.x / (ViewPortDesc.Width * 0.5f) - 1.f, vScreenPoint.y / (ViewPortDesc.Height * -0.5f) + 1.f, 0.0f);

	XMStoreFloat3(&vPoint, XMVector3TransformCoord(XMLoadFloat3(&vPoint), XMLoadFloat4x4(&vProjMatrixInv)));

	vRayDir_InWorld.x = vPoint.x - vOrigin.x;
	vRayDir_InWorld.y = vPoint.y - vOrigin.y;
	vRayDir_InWorld.z = vPoint.z - vOrigin.z;

	XMStoreFloat3(&vRayPos_InWorld, XMVector3TransformCoord(XMLoadFloat3(&vOrigin), XMLoadFloat4x4(&vViewMatrixInv)));
	XMStoreFloat3(&vRayDir_InWorld, XMVector3TransformNormal(XMLoadFloat3(&vRayDir_InWorld), XMLoadFloat4x4(&vViewMatrixInv)));
	XMStoreFloat3(&vRayDir_InWorld, XMVector3Normalize(XMLoadFloat3(&vRayDir_InWorld)));

	CRay* pRay = CRay::Create(pDevice, pContext, vRayPos_InWorld, vRayDir_InWorld);

	if (nullptr == pRay)
	{
		RELEASE_INSTANCE(CGameInstance);
		return nullptr;
	}

	RELEASE_INSTANCE(CGameInstance);

	return pRay;
}

_bool CRay::RayCast(_uint iLevel, const wstring& strLayerTag, _float3 vRayPos, _float3 vRayDir, CGameObject** pHitObject, _float& fDist)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	list<CGameObject*>* pGameObjects = pGameInstance->Find_Objects(iLevel, strLayerTag);

	if (nullptr == pGameObjects)
	{
		RELEASE_INSTANCE(CGameInstance);
		return false;
	}

	for (auto pGameObject : *pGameObjects)
	{
		CCollider* pCollider = (CCollider*)pGameObject->Find_Component(TEXT("Com_Collider"));

		if (nullptr != pCollider)
		{
			_bool bisColl = pCollider->Intersect(vRayPos, vRayDir, fDist);

			if (true == bisColl)
			{
				*pHitObject = pGameObject;

				RELEASE_INSTANCE(CGameInstance);

				return true;
			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	return false;
}

_bool CRay::RayCast(_float3 vRayPos, _float3 vRayDir, CGameObject* pTerrain, _float3* pPosition_InTerrain)
{
	if (nullptr == pTerrain)
		return false;

	_float3 vOrigin = *pPosition_InTerrain;

	CTransform* pTransformCom = (CTransform*)pTerrain->Find_Component(TEXT("Com_Transform"));
	CVIBuffer_Terrain* pVIBufferCom = (CVIBuffer_Terrain*)pTerrain->Find_Component(TEXT("Com_VIBuffer"));

	if (nullptr == pTransformCom ||
		nullptr == pVIBufferCom)
		return false;

	_matrix WorldMatrix = pTransformCom->Get_WorldMatrix();

	*pPosition_InTerrain = pVIBufferCom->Compute_RayCast(vRayPos, vRayDir, WorldMatrix);

	if (XMVector3Equal(XMLoadFloat3(&vOrigin), XMLoadFloat3(pPosition_InTerrain)))
		return false;
	else
		return true;
}

void CRay::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
