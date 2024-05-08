#include "VIBuffer_Rect.h"

CVIBuffer_Rect::CVIBuffer_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Rect::CVIBuffer_Rect(const CVIBuffer_Rect& rhs)
    : CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Rect::Initialize_Prototype()
{
	m_iNumVertexBuffers	 = 1;

	m_iVertexStride		 = sizeof(VTXPOSTEX);
	m_iIndexStride		 = 2;
						 
	m_iNumVertices		 = 4; /* 정점 4개 */
	m_iNumIndices		 = 6; /* 인덱스 6개 */
						 
	m_eIndexFormat		 = DXGI_FORMAT_R16_UINT; /* 16비트(2바이트) 부호 없는 정수 */
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST; /* 꼭지점 데이터를 삼각형 목록으로 해석 */

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	ZeroMemory(&m_InitialData, sizeof m_InitialData);

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;	/* 버퍼의 총 크기 */
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;					/* 정적(DEFAULT) or 동적 버퍼(DYNAMIC) */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;			/* 무슨 용도로 장치에 바인딩할 것인지 */
	m_BufferDesc.CPUAccessFlags = 0;							/* 동적 버퍼일 경우 CPU가 값을 읽거나 쓸 수 있을 것인지 (정적 : 0) */
	m_BufferDesc.MiscFlags = 0;									/* (정적 : 0) */
	m_BufferDesc.StructureByteStride = m_iVertexStride;			/* 정점 하나의 크기 */

	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexcoord = _float2(0.f, 0.f);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexcoord = _float2(1.f, 0.f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexcoord = _float2(1.f, 1.f);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexcoord = _float2(0.f, 1.f);

	/* SubResource의 Pitch는 이미지 가로 한줄의 Byte 크기, 현재는 정점 버퍼 할당이기 때문에 필요하지 않음*/
	m_InitialData.pSysMem = pVertices;							/* 공간을 할당할 때 해당 주소에 접근해서 값을 복사해서 할당 (memcpy) */

	/* DX11은 정적 버퍼로 할당하면 더이상 이 이후로 값을 바꿀 수 없음 */
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);								/* 더이상 필요 없는 값이기 때문에 파괴 */
#pragma endregion

#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	ZeroMemory(&m_InitialData, sizeof m_InitialData);

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iIndexStride;
	
	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;

	pIndices[3] = 0;
	pIndices[4] = 2;
	pIndices[5] = 3;

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion

    return S_OK;
}

HRESULT CVIBuffer_Rect::Initialize_Clone(void* pArg)
{
    return S_OK;
}

CVIBuffer_Rect* CVIBuffer_Rect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Rect* pInstance = new CVIBuffer_Rect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CVIBuffer_Rect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Rect::Clone(void* pArg)
{
	CVIBuffer_Rect* pInstance = new CVIBuffer_Rect(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CVIBuffer_Rect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Rect::Free()
{
	__super::Free();
}
