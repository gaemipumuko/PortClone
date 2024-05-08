#include "VIBuffer_Trail.h"

CVIBuffer_Trail::CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail& rhs)
	: CVIBuffer(rhs)
	, m_iNumTriCount(rhs.m_iNumTriCount)
{
}

void CVIBuffer_Trail::Set_PlayTrail(_bool bPlayTail)
{
	if ((m_bPlayTrail ^ bPlayTail) == TRUE)
		m_fCurrFrame = 0.f;

	if (m_bPlayTrail == FALSE && bPlayTail == TRUE)
		m_VertexList.clear();

	m_bPlayTrail = bPlayTail;
}

HRESULT CVIBuffer_Trail::Append_TrailVertex(_float fTimeDelta, _fvector vLocalLow, _fvector vLocalHigh)
{
	m_fCurrFrame += fTimeDelta;

	if (m_fCurrFrame > m_fCreateTime)
	{
		XMStoreFloat3(&m_vLastLow, vLocalLow);
		XMStoreFloat3(&m_vLastHigh, vLocalHigh);

		m_VertexList.push_back(vLocalLow);
		m_VertexList.push_back(vLocalHigh);

		m_iVertexCount = static_cast<_uint>(m_VertexList.size());

		if (m_iVertexCount > m_iNumVertices)
		{
			_uint iRemoveCount = m_iVertexCount / 30;
			if ((iRemoveCount & 1) == TRUE)
				iRemoveCount += 1;

			m_iVertexCount -= iRemoveCount;

			for (_uint iIndex = 0; iIndex < m_iVertexCount; iIndex += 2)
			{
				m_VertexList[iIndex] = m_VertexList[iRemoveCount + iIndex];
				m_VertexList[iIndex + 1] = m_VertexList[iRemoveCount + iIndex + 1];
			}

			m_VertexList.resize(m_iVertexCount);
		}
	}

	m_iVertexCount = static_cast<_uint>(m_VertexList.size());

	return S_OK;
}

HRESULT CVIBuffer_Trail::Invalidate_VertexBuff(_float fTimeDelta)
{
	if (m_bPlayTrail == TRUE)
		Invalidate_PlayVertexBuff(fTimeDelta);
	else
		Invalidate_StopVertexBuff(fTimeDelta);

	return S_OK;
}

HRESULT CVIBuffer_Trail::Invalidate_PlayVertexBuff(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource = {};

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint iIndex = 0; iIndex < m_iVertexCount; ++iIndex)
	{
		static_cast<VTXPOSTEX*>(SubResource.pData)[iIndex].vPosition =
			{ m_VertexList[iIndex].m128_f32[0], m_VertexList[iIndex].m128_f32[1], m_VertexList[iIndex].m128_f32[2] };
	}

	for (_uint iIndex = m_iVertexCount; iIndex < m_iNumVertices; iIndex += 2)
	{
		static_cast<VTXPOSTEX*>(SubResource.pData)[iIndex].vPosition = m_vLastLow;
		static_cast<VTXPOSTEX*>(SubResource.pData)[iIndex + 1].vPosition = m_vLastHigh;
	}

	/* CatmullRom */
	if (m_iVertexCount >= 2)
	{
		m_iEndIndex = m_iCatmullRomCount * 2 + m_iVertexCount;
		m_iCatmullRomIndex[2] = m_iEndIndex - 2;
		m_iCatmullRomIndex[3] = m_iEndIndex;

		static_cast<VTXPOSTEX*>(SubResource.pData)[m_iEndIndex - 2].vPosition = static_cast<VTXPOSTEX*>(SubResource.pData)[m_iVertexCount - 2].vPosition;
		static_cast<VTXPOSTEX*>(SubResource.pData)[m_iEndIndex - 1].vPosition = static_cast<VTXPOSTEX*>(SubResource.pData)[m_iVertexCount - 1].vPosition;

		static_cast<VTXPOSTEX*>(SubResource.pData)[m_iEndIndex].vPosition = m_vLastLow;
		static_cast<VTXPOSTEX*>(SubResource.pData)[m_iEndIndex + 1].vPosition = m_vLastHigh;

		for (_uint iIndex = 0; iIndex < m_iCatmullRomCount; ++iIndex)
		{
			_vector vCatRom = { };
			_uint iVtxIndex = iIndex * 2 + m_iVertexCount - 2;
			_float fWeight = _float(iIndex + 1) / (m_iCatmullRomCount + 1);

			vCatRom = XMVectorCatmullRom(
				XMLoadFloat3(&static_cast<VTXPOSTEX*>(SubResource.pData)[m_iCatmullRomIndex[0]].vPosition),
				XMLoadFloat3(&static_cast<VTXPOSTEX*>(SubResource.pData)[m_iCatmullRomIndex[1]].vPosition),
				XMLoadFloat3(&static_cast<VTXPOSTEX*>(SubResource.pData)[m_iCatmullRomIndex[2]].vPosition),
				XMLoadFloat3(&static_cast<VTXPOSTEX*>(SubResource.pData)[m_iCatmullRomIndex[3]].vPosition),
				fWeight
			);

			XMStoreFloat3(&static_cast<VTXPOSTEX*>(SubResource.pData)[iVtxIndex].vPosition, vCatRom);

			vCatRom = XMVectorCatmullRom(
				XMLoadFloat3(&static_cast<VTXPOSTEX*>(SubResource.pData)[m_iCatmullRomIndex[0] + 1].vPosition),
				XMLoadFloat3(&static_cast<VTXPOSTEX*>(SubResource.pData)[m_iCatmullRomIndex[1] + 1].vPosition),
				XMLoadFloat3(&static_cast<VTXPOSTEX*>(SubResource.pData)[m_iCatmullRomIndex[2] + 1].vPosition),
				XMLoadFloat3(&static_cast<VTXPOSTEX*>(SubResource.pData)[m_iCatmullRomIndex[3] + 1].vPosition),
				fWeight
			);

			XMStoreFloat3(&static_cast<VTXPOSTEX*>(SubResource.pData)[iVtxIndex + 1].vPosition, vCatRom);
		}
	}

	m_iVertexCount = m_iEndIndex + 2;

	m_iCatmullRomIndex[0] = m_iCatmullRomIndex[1];
	m_iCatmullRomIndex[1] = m_iCatmullRomIndex[2];

	for (_uint iIndex = 0; iIndex < m_iVertexCount; iIndex += 2)
	{
		static_cast<VTXPOSTEX*>(SubResource.pData)[iIndex].vTexcoord = { 1.f - (_float)iIndex / ((_float)m_iVertexCount - 2), 1.f };
		static_cast<VTXPOSTEX*>(SubResource.pData)[iIndex + 1].vTexcoord = { 1.f - (_float)iIndex / ((_float)m_iVertexCount - 2), 0.f };
	}

	m_pContext->Unmap(m_pVB, 0);

	return S_OK;
}

HRESULT CVIBuffer_Trail::Invalidate_StopVertexBuff(_float fTimeDelta)
{
	if (m_iVertexCount == 0)
		return S_OK;

	m_fCurrFrame += fTimeDelta;

	if (m_fCurrFrame > m_fRemoveTime)
	{
		m_iVertexCount = static_cast<_uint>(m_VertexList.size());

		_uint iRemoveCount = 2;

		m_iVertexCount -= iRemoveCount;

		for (_uint iIndex = 0; iIndex < m_iVertexCount; iIndex += 2)
		{
			m_VertexList[iIndex] = m_VertexList[iRemoveCount + iIndex];
			m_VertexList[iIndex + 1] = m_VertexList[iRemoveCount + iIndex + 1];
		}

		m_VertexList.resize(m_iVertexCount);
	}

	D3D11_MAPPED_SUBRESOURCE		SubResource = {};

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint iIndex = 0; iIndex < m_iVertexCount; ++iIndex)
	{
		static_cast<VTXPOSTEX*>(SubResource.pData)[iIndex].vPosition =
		{ m_VertexList[iIndex].m128_f32[0], m_VertexList[iIndex].m128_f32[1], m_VertexList[iIndex].m128_f32[2] };
	}

	for (_uint iIndex = m_iVertexCount; iIndex < m_iNumVertices; iIndex += 2)
	{
		static_cast<VTXPOSTEX*>(SubResource.pData)[iIndex].vPosition = m_vLastLow;
		static_cast<VTXPOSTEX*>(SubResource.pData)[iIndex + 1].vPosition = m_vLastHigh;
	}

	m_pContext->Unmap(m_pVB, 0);

	return S_OK;
}

HRESULT CVIBuffer_Trail::Initialize_Prototype(_uint iNumTriCnt)
{
	m_iNumTriCount = iNumTriCnt;

	m_iNumVertexBuffers = 1;

	m_iVertexStride = sizeof(VTXPOSTEX);
	m_iIndexStride = 4;

	m_iNumVertices = iNumTriCnt + 2;
	m_iNumIndices = iNumTriCnt * 3;

	m_eIndexFormat = DXGI_FORMAT_R32_UINT;						/* 32비트(4바이트) 부호 없는 정수 */
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST; /* 꼭지점 데이터를 삼각형 목록으로 해석 */

	return S_OK;
}

HRESULT CVIBuffer_Trail::Initialize_Clone(void* pArg)
{
#pragma region VERTEX_BUFFER
	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	for (_uint iIndex = 0; iIndex < m_iNumVertices; ++iIndex)
	{
		pVertices[iIndex].vPosition = _float3(0.f, 0.f, 0.f);
		pVertices[iIndex].vTexcoord = _float2(0.f, 0.f);
	}
#pragma endregion

#pragma region CREATE_BUFFER
	/* Vertex Buffer */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	ZeroMemory(&m_InitialData, sizeof m_InitialData);

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;	/* 버퍼의 총 크기 */
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;					/* 정적(DEFAULT) or 동적 버퍼(DYNAMIC) */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;			/* 무슨 용도로 장치에 바인딩할 것인지 */
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;		/* 동적 버퍼일 경우 CPU가 값을 읽거나 쓸 수 있을 것인지 (정적 : 0) */
	m_BufferDesc.MiscFlags = 0;									/* (정적 : 0) */
	m_BufferDesc.StructureByteStride = m_iVertexStride;			/* 정점 하나의 크기 */

	m_InitialData.pSysMem = pVertices;							/* 공간을 할당할 때 해당 주소에 접근해서 값을 복사해서 할당 (memcpy) */

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;
#pragma endregion

#pragma region INDEX_BUFFER
	_ulong* pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

	_uint		iNumIndices = 0;
	for (_uint iIndex = 0; iIndex < m_iNumTriCount; iIndex += 2)
	{
		pIndices[iNumIndices++] = iIndex + 3;
		pIndices[iNumIndices++] = iIndex + 1;
		pIndices[iNumIndices++] = iIndex;

		pIndices[iNumIndices++] = iIndex + 2;
		pIndices[iNumIndices++] = iIndex + 3;
		pIndices[iNumIndices++] = iIndex;
	}
#pragma endregion

#pragma region CREATE_BUFFER
	/* Index Buffer */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	ZeroMemory(&m_InitialData, sizeof m_InitialData);

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;
#pragma endregion

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

	return S_OK;
}

CVIBuffer_Trail* CVIBuffer_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumTriCnt)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iNumTriCnt)))
	{
		MSG_BOX(TEXT("Failed to Create : CVIBuffer_Trail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Trail::Clone(void* pArg)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CVIBuffer_Trail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CVIBuffer_Trail::Render()
{
	if ((m_bPlayTrail == FALSE) && (m_iVertexCount < m_iNumVertices * 0.1f))
		return S_OK;

	/* 인스턴싱을 위한 배열들 */
	ID3D11Buffer* pVertexBuffer[] = {
		m_pVB,
	};

	_uint				iVertexStrides[] = {
		m_iVertexStride,
	};

	_uint				iOffsets[] = {
		0,
	};

	/* 정점 버퍼들을 장치에 바인딩 (장치에 여러 개의 정점 버퍼를 동시에 바인딩할 수 있음) */
	/* 바인딩 시작 슬롯, 정점 버퍼의 개수, 정점 버퍼 배열(이중포인터), 정점 하나 크기의 배열(포인터), VB의 메모리 시작 바이트 배열(포인터) */
	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffer, iVertexStrides, iOffsets);

	/* 인덱스 버퍼를 장치에 바인딩 (인덱스 버퍼는 하나만 바인딩) */
	/* 인덱스 버퍼, 인덱스 하나 크기, IB의 메모리 시작 바이트 */
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	/* 인덱스로 어떤 방식으로 그릴 것인지 (TriangleList) */
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);
	//m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	/* 인덱스 버퍼를 이용해서 그림 (0번째 IB, VB부터 그림) */
	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

	return S_OK;
}

void CVIBuffer_Trail::Free()
{
	__super::Free();
}
