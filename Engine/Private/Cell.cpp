#include "Cell.h"

#ifdef _DEBUG
#include "VIBuffer_Cell.h"
#endif

#include "GameInstance.h"

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_vector CCell::Get_SlidingVector(_fvector vLocalPos, _fvector vMoveDir)
{
	_int iOutCellIndex = -1;

	for (size_t iCellIndex = 0; iCellIndex < LINE_END; ++iCellIndex)
	{
		_vector	vDir = vLocalPos - XMLoadFloat3(&m_vPoints[iCellIndex]);

		if (0.f < XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDir), XMLoadFloat3(&m_vNormals[iCellIndex]))))
		{
			iOutCellIndex = static_cast<_int>(iCellIndex);
		}
	}

	if (iOutCellIndex == -1)
		return vMoveDir;
	else
	{
		_vector vSliding = 
			vMoveDir -
			XMLoadFloat3(&m_vNormals[iOutCellIndex]) * (XMVector3Dot(vMoveDir, XMLoadFloat3(&m_vNormals[iOutCellIndex])));

		return vSliding;
	}
}

_bool CCell::isIn(_fvector vLocalPos, _int* pNeighborIndex)
{
	for (size_t iCellIndex = 0; iCellIndex < LINE_END; ++iCellIndex)
	{
		_vector	vDir = vLocalPos - XMLoadFloat3(&m_vPoints[iCellIndex]);

		if (0.f < XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDir), XMLoadFloat3(&m_vNormals[iCellIndex]))))
		{
			*pNeighborIndex = m_iNeighborIndices[iCellIndex];
			return FALSE;
		}
	}

	return TRUE;
}

HRESULT CCell::Initialize(const _float3* pPoints, const _int* pIndices, _int iIndex)
{
	m_iIndex = iIndex;

	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);
	memcpy(m_iNeighborIndices, pIndices, sizeof(_int) * 3);

	_vector		vLine;

	/* 법선 벡터 저장 */
	vLine = XMVector3Normalize(XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(&m_vPoints[POINT_A]));
	m_vNormals[LINE_AB] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

	vLine = XMVector3Normalize(XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(&m_vPoints[POINT_B]));
	m_vNormals[LINE_BC] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

	vLine = XMVector3Normalize(XMLoadFloat3(&m_vPoints[POINT_A]) - XMLoadFloat3(&m_vPoints[POINT_C]));
	m_vNormals[LINE_CA] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;
#endif

	return S_OK;
}

#ifdef _DEBUG
HRESULT CCell::Render()
{
	m_pVIBuffer->Render();

	return S_OK;
}
#endif

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, const _int* pIndices, _int iIndex)
{
	CCell* pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, pIndices, iIndex)))
	{
		MSG_BOX(TEXT("Failed to Create : CCell"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCell::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
