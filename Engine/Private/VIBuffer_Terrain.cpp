#include "VIBuffer_Terrain.h"
#include "GameInstance.h"
#include "QuadTree.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)

{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs)
	: CVIBuffer(rhs)
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
{
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(_uint iNumVerticesX, _uint iNumVerticesZ)
{
	if (nullptr != m_pVerticesPos) 
	{
		Safe_Release(m_pVB);
		Safe_Delete_Array(m_pVerticesPos);
	}

	if (nullptr != m_pIndices)
	{
		Safe_Release(m_pIB);
		Safe_Delete_Array(m_pIndices);
	}

	m_iNumVerticesX = iNumVerticesX;
	m_iNumVerticesZ = iNumVerticesZ;

	/* 정점 속성 */
	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(VTXNORTEX);
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	/* 인덱스 속성 */
	m_iIndexStride = 4;
	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;

	/* 정점 데이터를 이용하여 삼각형을 순차적으로 그림 */
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	/* 정점 정보를 담을 배열 */
	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	/* 정점의 위치를 멤버 변수로 저장하기 위한 배열 */
	m_pVerticesPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iNumVertices);

	m_pNormals = new _float3[m_iNumVertices];
	ZeroMemory(m_pNormals, sizeof(_float3) * m_iNumVertices);

	m_pTexCoord = new _float2[m_iNumVertices];
	ZeroMemory(m_pTexCoord, sizeof(_float2) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; i++)
	{
		for (_uint j = 0; j < m_iNumVerticesX; j++)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			m_pVerticesPos[iIndex] = pVertices[iIndex].vPosition = _float3((_float)j, 0.0f, (_float)i);
			m_pNormals[iIndex] = pVertices[iIndex].vNormal = _float3(0.0f, 0.f, 0.f);
			m_pTexCoord[iIndex] = pVertices[iIndex].vTexcoord = _float2((_float)j / (m_iNumVerticesX - 1.f), (_float)i / (m_iNumVerticesZ - 1.f));
		}
	}

	/* 인덱스 정보를 담을 배열 */
	_ulong* pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

	/* 인덱스를 멤버 변수로 저장하기 위한 배열 */
	m_pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(m_pIndices, sizeof(_ulong) * m_iNumIndices);

	_uint iNumIndices = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; j++) 
		{
			/* 삼각형이 그려질 순서를 정의하고 각 삼각형의 노말을 계산하여 저장 */
			_uint iIndex = i * m_iNumVerticesX + j;

			_uint iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_vector vSrc, vDst, vNormal;

			m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[0];
			m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[1];
			m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[2];

			vSrc = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDst = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal)) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[1]].vNormal)) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal)) + vNormal);


			m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[0];
			m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[2];
			m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[3];

			vSrc = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDst = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal)) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal)) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[3]].vNormal)) + vNormal);
		}
	}

	/* 계산한 노말을 단위 벡터로 변환하여 다시 저장 */
	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		XMStoreFloat3(&pVertices[i].vNormal,
			XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));

		m_pNormals[i] = pVertices[i].vNormal;
	}

	/* 정점 버퍼의 속성을 정의하고 생성 */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	{
		m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
		m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		m_BufferDesc.MiscFlags = 0;
		m_BufferDesc.StructureByteStride = m_iVertexStride;
	}
	
	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	/* 인덱스 버퍼의 속성을 정의하고 생성 */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	{
		m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
		m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		m_BufferDesc.MiscFlags = 0;
		m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;
	}
	
	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(_uint iNumVerticesX, _uint iNumVerticesZ, _float fGridSpacing)
{
	if (nullptr != m_pVerticesPos)
	{
		Safe_Release(m_pVB);
		Safe_Delete_Array(m_pVerticesPos);
	}

	if (nullptr != m_pIndices)
	{
		Safe_Release(m_pIB);
		Safe_Delete_Array(m_pIndices);
	}

	m_iNumVerticesX = iNumVerticesX;
	m_iNumVerticesZ = iNumVerticesZ;

	/* 정점 속성 */
	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(VTXNORTEX);
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	/* 인덱스 속성 */
	m_iIndexStride = 4;
	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;

	/* 정점 데이터를 이용하여 삼각형을 순차적으로 그림 */
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	/* 정점 정보를 담을 배열 */
	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	/* 정점의 위치를 멤버 변수로 저장하기 위한 배열 */
	m_pVerticesPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iNumVertices);

	m_pNormals = new _float3[m_iNumVertices];
	ZeroMemory(m_pNormals, sizeof(_float3) * m_iNumVertices);

	m_pTexCoord = new _float2[m_iNumVertices];
	ZeroMemory(m_pTexCoord, sizeof(_float2) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; i++)
	{
		for (_uint j = 0; j < m_iNumVerticesX; j++)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			m_pVerticesPos[iIndex] = pVertices[iIndex].vPosition = _float3((_float)j * fGridSpacing, 0.0f, (_float)i * fGridSpacing);
			m_pNormals[iIndex] = pVertices[iIndex].vNormal = _float3(0.0f, 0.f, 0.f);
			m_pTexCoord[iIndex] = pVertices[iIndex].vTexcoord = _float2((_float)j / (m_iNumVerticesX - 1.f), (_float)i / (m_iNumVerticesZ - 1.f));
		}
	}

	/* 인덱스 정보를 담을 배열 */
	_ulong* pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

	/* 인덱스를 멤버 변수로 저장하기 위한 배열 */
	m_pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(m_pIndices, sizeof(_ulong) * m_iNumIndices);

	_uint iNumIndices = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; j++)
		{
			/* 삼각형이 그려질 순서를 정의하고 각 삼각형의 노말을 계산하여 저장 */
			_uint iIndex = i * m_iNumVerticesX + j;

			_uint iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_vector vSrc, vDst, vNormal;

			m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[0];
			m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[1];
			m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[2];

			vSrc = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDst = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal)) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[1]].vNormal)) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal)) + vNormal);


			m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[0];
			m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[2];
			m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[3];

			vSrc = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDst = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal)) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal)) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[3]].vNormal)) + vNormal);
		}
	}

	/* 계산한 노말을 단위 벡터로 변환하여 다시 저장 */
	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		XMStoreFloat3(&pVertices[i].vNormal,
			XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));

		m_pNormals[i] = pVertices[i].vNormal;
	}

	/* 정점 버퍼의 속성을 정의하고 생성 */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	{
		m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
		m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		m_BufferDesc.MiscFlags = 0;
		m_BufferDesc.StructureByteStride = m_iVertexStride;
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	/* 인덱스 버퍼의 속성을 정의하고 생성 */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	{
		m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
		m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		m_BufferDesc.MiscFlags = 0;
		m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const wstring& strDataFilePath)
{
	if (true == strDataFilePath.empty())
		return E_FAIL;

	ifstream InFile(strDataFilePath, std::ios::binary);

	if (InFile.is_open())
	{
		if (nullptr != m_pVerticesPos)
		{
			Safe_Release(m_pVB);
			Safe_Delete_Array(m_pVerticesPos);
		}

		if (nullptr != m_pIndices)
		{
			Safe_Release(m_pIB);
			Safe_Delete_Array(m_pIndices);
		}

		InFile.read(reinterpret_cast<char*>(&m_iNumVerticesX), sizeof(_uint));
		InFile.read(reinterpret_cast<char*>(&m_iNumVerticesZ), sizeof(_uint));

		_float* pHeight = new _float[m_iNumVerticesX * m_iNumVerticesZ];
		ZeroMemory(pHeight, sizeof(_float) * m_iNumVerticesX * m_iNumVerticesZ);

		for (_uint i = 0; i < m_iNumVerticesX * m_iNumVerticesZ; i++)
		{
			_float fHeight;
			InFile.read(reinterpret_cast<char*>(&fHeight), sizeof(_float));
			pHeight[i] = fHeight;
		}

		/* 정점 속성 */
		m_iNumVertexBuffers = 1;
		m_iVertexStride = sizeof(VTXNORTEX);
		m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

		/* 인덱스 속성 */
		m_iIndexStride = 4;
		m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
		m_eIndexFormat = DXGI_FORMAT_R32_UINT;

		/* 정점 데이터를 이용하여 삼각형을 순차적으로 그림 */
		m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		/* 정점 정보를 담을 배열 */
		VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
		ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

		/* 정점의 위치를 멤버 변수로 저장하기 위한 배열 */
		m_pVerticesPos = new _float3[m_iNumVertices];
		ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iNumVertices);

		m_pNormals = new _float3[m_iNumVertices];
		ZeroMemory(m_pNormals, sizeof(_float3) * m_iNumVertices);

		m_pTexCoord = new _float2[m_iNumVertices];
		ZeroMemory(m_pTexCoord, sizeof(_float2) * m_iNumVertices);

		for (_uint i = 0; i < m_iNumVerticesZ; i++)
		{
			for (_uint j = 0; j < m_iNumVerticesX; j++)
			{
				_uint iIndex = i * m_iNumVerticesX + j;

				m_pVerticesPos[iIndex] = pVertices[iIndex].vPosition = _float3(static_cast<_float>(j), pHeight[iIndex], static_cast<_float>(i));
				m_pNormals[iIndex] = pVertices[iIndex].vNormal = _float3(0.0f, 0.f, 0.f);
				m_pTexCoord[iIndex] = pVertices[iIndex].vTexcoord = _float2((_float)j / (m_iNumVerticesX - 1.f), (_float)i / (m_iNumVerticesZ - 1.f));
			}
		}

		Safe_Delete_Array(pHeight);

		/* 인덱스 정보를 담을 배열 */
		_ulong* pIndices = new _ulong[m_iNumIndices];
		ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

		/* 인덱스를 멤버 변수로 저장하기 위한 배열 */
		m_pIndices = new _ulong[m_iNumIndices];
		ZeroMemory(m_pIndices, sizeof(_ulong) * m_iNumIndices);

		_uint iNumIndices = 0;

		for (_uint i = 0; i < m_iNumVerticesZ - 1; i++)
		{
			for (_uint j = 0; j < m_iNumVerticesX - 1; j++)
			{
				/* 삼각형이 그려질 순서를 정의하고 각 삼각형의 노말을 계산하여 저장 */
				_uint iIndex = i * m_iNumVerticesX + j;

				_uint iIndices[4] = {
					iIndex + m_iNumVerticesX,
					iIndex + m_iNumVerticesX + 1,
					iIndex + 1,
					iIndex
				};

				_vector vSrc, vDst, vNormal;

				m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[0];
				m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[1];
				m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[2];

				vSrc = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
				vDst = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
				vNormal = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

				XMStoreFloat3(&pVertices[iIndices[0]].vNormal,
					XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal)) + vNormal);
				XMStoreFloat3(&pVertices[iIndices[1]].vNormal,
					XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[1]].vNormal)) + vNormal);
				XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
					XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal)) + vNormal);


				m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[0];
				m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[2];
				m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[3];

				vSrc = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
				vDst = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
				vNormal = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

				XMStoreFloat3(&pVertices[iIndices[0]].vNormal,
					XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal)) + vNormal);
				XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
					XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal)) + vNormal);
				XMStoreFloat3(&pVertices[iIndices[3]].vNormal,
					XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[3]].vNormal)) + vNormal);
			}
		}

		/* 계산한 노말을 단위 벡터로 변환하여 다시 저장 */
		for (_uint i = 0; i < m_iNumVertices; i++)
		{
			XMStoreFloat3(&pVertices[i].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));

			m_pNormals[i] = pVertices[i].vNormal;
		}

		/* 정점 버퍼의 속성을 정의하고 생성 */
		ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
		{
			m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
			m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			m_BufferDesc.MiscFlags = 0;
			m_BufferDesc.StructureByteStride = m_iVertexStride;
		}

		ZeroMemory(&m_InitialData, sizeof m_InitialData);
		m_InitialData.pSysMem = pVertices;

		if (FAILED(__super::Create_Buffer(&m_pVB)))
			return E_FAIL;

		/* 인덱스 버퍼의 속성을 정의하고 생성 */
		ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
		{
			m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
			m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			m_BufferDesc.MiscFlags = 0;
			m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;
		}

		ZeroMemory(&m_InitialData, sizeof m_InitialData);
		m_InitialData.pSysMem = pIndices;

		if (FAILED(__super::Create_Buffer(&m_pIB)))
			return E_FAIL;

		Safe_Delete_Array(pVertices);
		Safe_Delete_Array(pIndices);
	}

	InFile.close();

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize_Clone(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize_QuadTree()
{
	if (nullptr != m_pQuadTree)
		Safe_Release(m_pQuadTree);

	m_pQuadTree = CQuadTree::Create(m_iNumVerticesX * m_iNumVerticesZ - m_iNumVerticesX,
		m_iNumVerticesX * m_iNumVerticesZ - 1,
		m_iNumVerticesX - 1,
		0);

	if (FAILED(m_pQuadTree->Make_Neighbors()))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_Terrain::Culling_Frustum(_fmatrix WorldMatrix)
{
	m_pGameInstance->Transform_LocalSpace(XMMatrixInverse(nullptr, WorldMatrix));

	D3D11_MAPPED_SUBRESOURCE		SubResource = {};
	_uint							iNumIndices = 0;

	m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
	_uint* pIndices = (_uint*)SubResource.pData;

	for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint		iIndex = static_cast<_uint>(i * m_iNumVerticesX + j);

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_bool		isIn[4] = {
				m_pGameInstance->PointInFrustum_InLocal(XMLoadFloat3(&m_pVerticesPos[iIndices[0]])),
				m_pGameInstance->PointInFrustum_InLocal(XMLoadFloat3(&m_pVerticesPos[iIndices[1]])),
				m_pGameInstance->PointInFrustum_InLocal(XMLoadFloat3(&m_pVerticesPos[iIndices[2]])),
				m_pGameInstance->PointInFrustum_InLocal(XMLoadFloat3(&m_pVerticesPos[iIndices[3]])),
			};

			if (true == isIn[0] &&
				true == isIn[1] &&
				true == isIn[2])
			{
				pIndices[iNumIndices++] = iIndices[0];
				pIndices[iNumIndices++] = iIndices[1];
				pIndices[iNumIndices++] = iIndices[2];
			}

			if (true == isIn[0] &&
				true == isIn[2] &&
				true == isIn[3])
			{
				pIndices[iNumIndices++] = iIndices[0];
				pIndices[iNumIndices++] = iIndices[2];
				pIndices[iNumIndices++] = iIndices[3];
			}
		}
	}

	m_pContext->Unmap(m_pIB, 0);

	m_iNumIndices = iNumIndices;
}

void CVIBuffer_Terrain::Culling_QuadTree(_fmatrix WorldMatrix)
{
	if (nullptr == m_pQuadTree)
		return;

	m_pGameInstance->Transform_LocalSpace(XMMatrixInverse(nullptr, WorldMatrix));

	D3D11_MAPPED_SUBRESOURCE		SubResource = {};
	_uint							iNumIndices = 0;

	m_pContext->Map(m_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
	_uint* pIndices = (_uint*)SubResource.pData;

	m_pQuadTree->Culling(m_pGameInstance, m_pVerticesPos, pIndices, &iNumIndices, WorldMatrix);

	m_pContext->Unmap(m_pIB, 0);

	m_iNumIndices = iNumIndices;
}

HRESULT CVIBuffer_Terrain::Resize_Terrain(_uint iNumVerticesX, _uint iNumVerticesZ)
{
	if (iNumVerticesX <= 0 || iNumVerticesZ <= 0)
		return E_FAIL;

	if (nullptr != m_pVerticesPos)
	{
		Safe_Release(m_pVB);
		Safe_Delete_Array(m_pVerticesPos);
	}

	if (nullptr != m_pIndices)
	{
		Safe_Release(m_pIB);
		Safe_Delete_Array(m_pIndices);
	}

	m_iNumVerticesX = iNumVerticesX;
	m_iNumVerticesZ = iNumVerticesZ;

	/* 정점 속성 */
	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(VTXNORTEX);
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	/* 인덱스 속성 */
	m_iIndexStride = 4;
	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;

	/* 정점 데이터를 이용하여 삼각형을 순차적으로 그림 */
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	/* 정점 정보를 담을 배열 */
	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	/* 정점의 위치를 멤버 변수로 저장하기 위한 배열 */
	m_pVerticesPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iNumVertices);

	m_pNormals = new _float3[m_iNumVertices];
	ZeroMemory(m_pNormals, sizeof(_float3) * m_iNumVertices);

	m_pTexCoord = new _float2[m_iNumVertices];
	ZeroMemory(m_pTexCoord, sizeof(_float2) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; i++)
	{
		for (_uint j = 0; j < m_iNumVerticesX; j++)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			m_pVerticesPos[iIndex] = pVertices[iIndex].vPosition = _float3((_float)j, 0.0f, (_float)i);
			m_pNormals[iIndex] = pVertices[iIndex].vNormal = _float3(0.0f, 0.f, 0.f);
			m_pTexCoord[iIndex] = pVertices[iIndex].vTexcoord = _float2((_float)j / (m_iNumVerticesX - 1.f), (_float)i / (m_iNumVerticesZ - 1.f));
		}
	}

	/* 인덱스 정보를 담을 배열 */
	_ulong* pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

	/* 인덱스를 멤버 변수로 저장하기 위한 배열 */
	m_pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(m_pIndices, sizeof(_ulong) * m_iNumIndices);

	_uint iNumIndices = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; j++)
		{
			/* 삼각형이 그려질 순서를 정의하고 각 삼각형의 노말을 계산하여 저장 */
			_uint iIndex = i * m_iNumVerticesX + j;

			_uint iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_vector vSrc, vDst, vNormal;

			m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[0];
			m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[1];
			m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[2];

			vSrc = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDst = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal)) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[1]].vNormal)) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal)) + vNormal);


			m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[0];
			m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[2];
			m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[3];

			vSrc = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDst = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal)) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal)) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[3]].vNormal)) + vNormal);
		}
	}

	/* 계산한 노말을 단위 벡터로 변환하여 다시 저장 */
	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		XMStoreFloat3(&pVertices[i].vNormal,
			XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));

		m_pNormals[i] = pVertices[i].vNormal;
	}

	/* 정점 버퍼의 속성을 정의하고 생성 */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	{
		m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
		m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		m_BufferDesc.MiscFlags = 0;
		m_BufferDesc.StructureByteStride = m_iVertexStride;
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	/* 인덱스 버퍼의 속성을 정의하고 생성 */
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	{
		m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		m_BufferDesc.CPUAccessFlags = 0;
		m_BufferDesc.MiscFlags = 0;
		m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Apply_HeightMap(const wstring& strDataFilePath)
{
	if (true == strDataFilePath.empty())
		return E_FAIL;

	ifstream InFile(strDataFilePath, std::ios::binary);

	if (InFile.is_open()) 
	{
		if (nullptr != m_pVerticesPos)
		{
			Safe_Release(m_pVB);
			Safe_Delete_Array(m_pVerticesPos);
			Safe_Delete_Array(m_pNormals);
			Safe_Delete_Array(m_pTexCoord);
		}

		if (nullptr != m_pIndices)
		{
			Safe_Release(m_pIB);
			Safe_Delete_Array(m_pIndices);
		}

		InFile.read(reinterpret_cast<char*>(&m_iNumVerticesX), sizeof(_uint));
		InFile.read(reinterpret_cast<char*>(&m_iNumVerticesZ), sizeof(_uint));

		_float* pHeight = new _float[m_iNumVerticesX * m_iNumVerticesZ];
		ZeroMemory(pHeight, sizeof(_float) * m_iNumVerticesX * m_iNumVerticesZ);
		
		for (_uint i = 0; i < m_iNumVerticesX * m_iNumVerticesZ; i++) 
		{
			_float fHeight;
			InFile.read(reinterpret_cast<char*>(&fHeight), sizeof(_float));
			pHeight[i] = fHeight;
		}

		/* 정점 속성 */
		m_iNumVertexBuffers = 1;
		m_iVertexStride = sizeof(VTXNORTEX);
		m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

		/* 인덱스 속성 */
		m_iIndexStride = 4;
		m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
		m_eIndexFormat = DXGI_FORMAT_R32_UINT;

		/* 정점 데이터를 이용하여 삼각형을 순차적으로 그림 */
		m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		/* 정점 정보를 담을 배열 */
		VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
		ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

		/* 정점의 위치를 멤버 변수로 저장하기 위한 배열 */
		m_pVerticesPos = new _float3[m_iNumVertices];
		ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iNumVertices);

		m_pNormals = new _float3[m_iNumVertices];
		ZeroMemory(m_pNormals, sizeof(_float3) * m_iNumVertices);

		m_pTexCoord = new _float2[m_iNumVertices];
		ZeroMemory(m_pTexCoord, sizeof(_float2) * m_iNumVertices);

		for (_uint i = 0; i < m_iNumVerticesZ; i++)
		{
			for (_uint j = 0; j < m_iNumVerticesX; j++)
			{
				_uint iIndex = i * m_iNumVerticesX + j;

				m_pVerticesPos[iIndex] = pVertices[iIndex].vPosition = _float3(static_cast<_float>(j), pHeight[iIndex], static_cast<_float>(i));
				m_pNormals[iIndex] = pVertices[iIndex].vNormal = _float3(0.0f, 0.f, 0.f);
				m_pTexCoord[iIndex] = pVertices[iIndex].vTexcoord = _float2((_float)j / (m_iNumVerticesX - 1.f), (_float)i / (m_iNumVerticesZ - 1.f));
			}
		}

		Safe_Delete_Array(pHeight);

		/* 인덱스 정보를 담을 배열 */
		_ulong* pIndices = new _ulong[m_iNumIndices];
		ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

		/* 인덱스를 멤버 변수로 저장하기 위한 배열 */
		m_pIndices = new _ulong[m_iNumIndices];
		ZeroMemory(m_pIndices, sizeof(_ulong) * m_iNumIndices);

		_uint iNumIndices = 0;

		for (_uint i = 0; i < m_iNumVerticesZ - 1; i++)
		{
			for (_uint j = 0; j < m_iNumVerticesX - 1; j++)
			{
				/* 삼각형이 그려질 순서를 정의하고 각 삼각형의 노말을 계산하여 저장 */
				_uint iIndex = i * m_iNumVerticesX + j;

				_uint iIndices[4] = {
					iIndex + m_iNumVerticesX,
					iIndex + m_iNumVerticesX + 1,
					iIndex + 1,
					iIndex
				};

				_vector vSrc, vDst, vNormal;

				m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[0];
				m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[1];
				m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[2];

				vSrc = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
				vDst = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
				vNormal = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

				XMStoreFloat3(&pVertices[iIndices[0]].vNormal,
					XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal)) + vNormal);
				XMStoreFloat3(&pVertices[iIndices[1]].vNormal,
					XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[1]].vNormal)) + vNormal);
				XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
					XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal)) + vNormal);


				m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[0];
				m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[2];
				m_pIndices[iNumIndices] = pIndices[iNumIndices++] = iIndices[3];

				vSrc = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
				vDst = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
				vNormal = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

				XMStoreFloat3(&pVertices[iIndices[0]].vNormal,
					XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal)) + vNormal);
				XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
					XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal)) + vNormal);
				XMStoreFloat3(&pVertices[iIndices[3]].vNormal,
					XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[3]].vNormal)) + vNormal);
			}
		}

		/* 계산한 노말을 단위 벡터로 변환하여 다시 저장 */
		for (_uint i = 0; i < m_iNumVertices; i++)
		{
			XMStoreFloat3(&pVertices[i].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));

			m_pNormals[i] = pVertices[i].vNormal;
		}

		/* 정점 버퍼의 속성을 정의하고 생성 */
		ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
		{
			m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
			m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			m_BufferDesc.MiscFlags = 0;
			m_BufferDesc.StructureByteStride = m_iVertexStride;
		}

		ZeroMemory(&m_InitialData, sizeof m_InitialData);
		m_InitialData.pSysMem = pVertices;

		if (FAILED(__super::Create_Buffer(&m_pVB)))
			return E_FAIL;

		/* 인덱스 버퍼의 속성을 정의하고 생성 */
		ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
		{
			m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
			m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
			m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			m_BufferDesc.CPUAccessFlags = 0;
			m_BufferDesc.MiscFlags = 0;
			m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;
		}

		ZeroMemory(&m_InitialData, sizeof m_InitialData);
		m_InitialData.pSysMem = pIndices;

		if (FAILED(__super::Create_Buffer(&m_pIB)))
			return E_FAIL;

		Safe_Delete_Array(pVertices);
		Safe_Delete_Array(pIndices);
	}

	InFile.close();

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Save_HeightMap(const wstring& strDataFilePath)
{
	if (true == strDataFilePath.empty())
		return E_FAIL;

	_float* pHeights = new _float[m_iNumVerticesX * m_iNumVerticesZ];
	ZeroMemory(pHeights, sizeof(_float) * m_iNumVerticesX * m_iNumVerticesZ);

	_int iSize = 0;

	for (_int i = 0; i < (_int)m_iNumVerticesZ; i++)
	{
		for (_int j = 0; j < (_int)m_iNumVerticesX; j++)
		{
			_uint iIndex = i * m_iNumVerticesX + j;
			pHeights[iSize++] = m_pVerticesPos[iIndex].y;
		}
	}

	ofstream OutFile(strDataFilePath, ios::binary);

	if (OutFile.is_open()) 
	{
		OutFile.write(reinterpret_cast<char*>(&m_iNumVerticesX), sizeof(_uint));
		OutFile.write(reinterpret_cast<char*>(&m_iNumVerticesZ), sizeof(_uint));

		for (_int i = 0; i < iSize; i++) 
		{
			_float fHeight = pHeights[i];
			OutFile.write(reinterpret_cast<char*>(&fHeight), sizeof(_float));
		}
	}

	OutFile.close();

	Safe_Delete_Array(pHeights);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Brushing_OnTerrain(_float& fBrushForce, _float3& vBrushPos, _float& fBrushSize, _float& fStrength, _float& fMaxHeight)
{
	_int iLeft = max(0, (_int)vBrushPos.x - (_int)fBrushSize);
	_int iTop = max(0, (_int)vBrushPos.z - (_int)fBrushSize);
	_int iRight = min(m_iNumVerticesX, (_uint)vBrushPos.x + (_uint)fBrushSize);
	_int iBottom = min(m_iNumVerticesZ, (_uint)vBrushPos.z + (_uint)fBrushSize);

	for (_int i = iTop; i < iBottom; i++) 
	{
		for (_int j = iLeft; j < iRight; j++) 
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			_float2 vCenter = _float2(vBrushPos.x, vBrushPos.z);
			_float2 vBrushRange = _float2(m_pVerticesPos[iIndex].x, m_pVerticesPos[iIndex].z);
			_float fDistance = XMVectorGetX(XMVector2Length(XMLoadFloat2(&vCenter) - XMLoadFloat2(&vBrushRange)));

			if (fDistance > fBrushSize)
				continue;

			_float3 vBrushPoint = _float3((_float)j, 0.0f, (_float)i);
			m_pVerticesPos[iIndex].y += fBrushForce + Compute_BrushInfluence(vBrushPoint, vBrushPos, fBrushSize, fStrength);

			if (fMaxHeight < m_pVerticesPos[iIndex].y)
				m_pVerticesPos[iIndex].y = fMaxHeight;
		}
	}

	VTXNORTEX* pVertices = nullptr;

	D3D11_MAPPED_SUBRESOURCE tMappedResource;
	ZeroMemory(&tMappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &tMappedResource);

	pVertices = (VTXNORTEX*)tMappedResource.pData;

	for (_uint i = 0; i < m_iNumVertices; i++) 
	{
		pVertices[i].vPosition = m_pVerticesPos[i];
		pVertices[i].vNormal = m_pNormals[i];
		pVertices[i].vTexcoord = m_pTexCoord[i];
	}

	m_pContext->Unmap(m_pVB, 0);

	return S_OK;
}

_float3 CVIBuffer_Terrain::Compute_RayCast(const _float3 vRayPos_InWorld, const _float3 vRayDir_InWorld, const _matrix WorldMatrix)
{
	_float3 vRayPos_InLocal;
	_float3 vRayDir_InLocal;

	_matrix WorldMatrixInv = XMMatrixInverse(nullptr, WorldMatrix);

	_float3 vPickPos = _float3(0.0f, 0.0f, 0.0f);

	_bool bisColl = false;

	XMStoreFloat3(&vRayPos_InLocal, XMVector3TransformCoord(XMLoadFloat3(&vRayPos_InWorld), WorldMatrixInv));
	XMStoreFloat3(&vRayDir_InLocal, XMVector3TransformCoord(XMLoadFloat3(&vRayDir_InWorld), WorldMatrixInv));
	XMStoreFloat3(&vRayDir_InLocal, XMVector3Normalize(XMLoadFloat3(&vRayDir_InLocal)));

	for (_uint i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			_uint iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			bisColl = RayCast_InTerrain(
				vRayPos_InLocal,
				vRayDir_InLocal,
				m_pVerticesPos[iIndices[0]],
				m_pVerticesPos[iIndices[1]],
				m_pVerticesPos[iIndices[2]],
				&vPickPos);

			if (true == bisColl)
				break;

			bisColl = RayCast_InTerrain(
				vRayPos_InLocal,
				vRayDir_InLocal,
				m_pVerticesPos[iIndices[0]],
				m_pVerticesPos[iIndices[2]],
				m_pVerticesPos[iIndices[3]],
				&vPickPos);

			if (true == bisColl)
				break;

			if (true == bisColl)
				break;
		}

		if (true == bisColl)
			break;
	}

	XMStoreFloat3(&vPickPos, XMVector3TransformCoord(XMLoadFloat3(&vPickPos), WorldMatrix));

	return vPickPos;
}

_bool CVIBuffer_Terrain::RayCast_InTerrain(const _float3 vRayPos_InLocal, const _float3 vRayDir_InLocal, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* vPickPos)
{
	_bool bisColl = false;

	_float fDist = 0.0f;

	bisColl = TriangleTests::Intersects(XMLoadFloat3(&vRayPos_InLocal), XMLoadFloat3(&vRayDir_InLocal), XMLoadFloat3(&vPointA), XMLoadFloat3(&vPointB), XMLoadFloat3(&vPointC), fDist);

	XMStoreFloat3(vPickPos, XMLoadFloat3(&vRayDir_InLocal) * fDist + XMLoadFloat3(&vRayPos_InLocal));

	return bisColl;
}

_float CVIBuffer_Terrain::Compute_BrushInfluence(_float3& vBrushPoint, _float3& vBrushPos, _float& fBrushSize, _float& fStrength)
{
	
	_float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vBrushPos) - XMLoadFloat3(&vBrushPoint)));
	_float finfluence = max(0.0f, 1.0f - fDistance / fBrushSize);

	return finfluence * fStrength;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumVerticesX, _uint iNumVerticesZ)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iNumVerticesX, iNumVerticesZ)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Terrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumVerticesX, _uint iNumVerticesZ, _float fGridSpacing)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iNumVerticesX, iNumVerticesZ, fGridSpacing)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Terrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strDataFilePath)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strDataFilePath)))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Terrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Terrain::Clone(void* pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Terrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();

	if (false == m_bCloned)
		Safe_Delete_Array(m_pVerticesPos);

	if (false == m_bCloned)
		Safe_Delete_Array(m_pIndices);

	if (false == m_bCloned)
		Safe_Delete_Array(m_pNormals);

	if (false == m_bCloned)
		Safe_Delete_Array(m_pTexCoord);

	if (nullptr != m_pQuadTree)
		Safe_Release(m_pQuadTree);
}
