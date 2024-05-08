#include "Triangle.h"

#ifdef _DEBUG
#include "VIBuffer_Cell.h"
#endif

CTriangle::CTriangle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CTriangle::Initialize(const _float3* pPoints, _int iIndex)
{
	if (nullptr == pPoints)
		return E_FAIL;

	m_iIndex = iIndex;

	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);

	_vector	vLine;

	vLine = XMVector3Normalize(XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(&m_vPoints[POINT_A]));
	m_vNormals[LINE_AB] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

	vLine = XMVector3Normalize(XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(&m_vPoints[POINT_B]));
	m_vNormals[LINE_BC] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

	vLine = XMVector3Normalize(XMLoadFloat3(&m_vPoints[POINT_A]) - XMLoadFloat3(&m_vPoints[POINT_C]));
	m_vNormals[LINE_CA] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

#ifdef _DEBUG
	m_pVIBufferCom = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);

	if (nullptr == m_pVIBufferCom)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CTriangle::Initialize(const _float3* pPoints, const _int* pIndices, _int iIndex)
{
	m_iIndex = iIndex;

	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);
	memcpy(m_iNeighborIndices, pIndices, sizeof(_int) * 3);

	_vector	vLine;

	vLine = XMVector3Normalize(XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(&m_vPoints[POINT_A]));
	m_vNormals[LINE_AB] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

	vLine = XMVector3Normalize(XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(&m_vPoints[POINT_B]));
	m_vNormals[LINE_BC] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

	vLine = XMVector3Normalize(XMLoadFloat3(&m_vPoints[POINT_A]) - XMLoadFloat3(&m_vPoints[POINT_C]));
	m_vNormals[LINE_CA] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));

#ifdef _DEBUG
	m_pVIBufferCom = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);

	if (nullptr == m_pVIBufferCom)
		return E_FAIL;
#endif

	return S_OK;
}

_bool CTriangle::Compare_Points(_fvector vPoint_A, _fvector vPoint_B)
{
	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vPoint_A))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vPoint_B))
			return true;

		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vPoint_B))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vPoint_A))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vPoint_B))
			return true;

		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vPoint_B))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vPoint_A))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vPoint_B))
			return true;

		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vPoint_B))
			return true;
	}

	return false;
}

_bool CTriangle::isIn(_fvector vPosition, _int* pNeighborIndex)
{
	for (size_t i = 0; i < LINE_END; i++)
	{
		_vector	vDir = vPosition - XMLoadFloat3(&m_vPoints[i]);

		if (0 < XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDir), XMLoadFloat3(&m_vNormals[i]))))
		{
			*pNeighborIndex = m_iNeighborIndices[i];
			return false;
		}
	}

	return true;
}

#ifdef _DEBUG

HRESULT CTriangle::Render()
{
	m_pVIBufferCom->Render();

	return S_OK;
}

#endif

CTriangle* CTriangle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex)
{
	CTriangle* pInstance = new CTriangle(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX(TEXT("Failed to Created : CTriangle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CTriangle* CTriangle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, const _int* pIndices, _int iIndex)
{
	CTriangle* pInstance = new CTriangle(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, pIndices, iIndex)))
	{
		MSG_BOX(TEXT("Failed to Created : CTriangle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTriangle::Free()
{
#ifdef _DEBUG
	Safe_Release(m_pVIBufferCom);
#endif

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
