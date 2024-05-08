#include "VIBuffer_Model_Instancing.h"
#include "GameInstance.h"
#include "Texture.h"

CVIBuffer_Model_Instancing::CVIBuffer_Model_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CVIBuffer_Model_Instancing::CVIBuffer_Model_Instancing(const CVIBuffer_Model_Instancing& rhs)
	: CComponent(rhs)
{
}

HRESULT CVIBuffer_Model_Instancing::Initialize_Prototype(const wstring& strModelFilePath)
{
#pragma region Instance Buffer
	m_iInstanceStride = sizeof(_float4x4);
	m_iNumInstance = 1;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	D3D11_BUFFER_DESC tInstanceBufferDesc = {};
	ZeroMemory(&tInstanceBufferDesc, sizeof(D3D11_BUFFER_DESC));
	{
		tInstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumMaxInstance;
		tInstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		tInstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		tInstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		tInstanceBufferDesc.MiscFlags = 0;
		tInstanceBufferDesc.StructureByteStride = m_iInstanceStride;
	}

	m_vInstanceMatrices.reserve(m_iNumMaxInstance);

	_float4x4 vInstanceMatrix;
	XMStoreFloat4x4(&vInstanceMatrix, XMMatrixIdentity());

	m_vInstanceMatrices.push_back(vInstanceMatrix);

	D3D11_SUBRESOURCE_DATA tInstanceInitialData = {};
	ZeroMemory(&tInstanceInitialData, sizeof(D3D11_SUBRESOURCE_DATA));
	tInstanceInitialData.pSysMem = m_vInstanceMatrices.data();

	if (FAILED(m_pDevice->CreateBuffer(&tInstanceBufferDesc, &tInstanceInitialData, &m_pInstanceBuffer)))
		return E_FAIL;
#pragma endregion

	ifstream InFile(strModelFilePath.c_str(), ios::binary | ios::in);

	if (InFile.is_open() == FALSE)
		return E_FAIL;

	InFile.read(reinterpret_cast<char*>(&m_eInstanceType), sizeof _int);

	InFile.read(reinterpret_cast<char*>(&m_vInstancePivot.m[0]), sizeof _float4);
	InFile.read(reinterpret_cast<char*>(&m_vInstancePivot.m[1]), sizeof _float4);
	InFile.read(reinterpret_cast<char*>(&m_vInstancePivot.m[2]), sizeof _float4);
	InFile.read(reinterpret_cast<char*>(&m_vInstancePivot.m[3]), sizeof _float4);

	m_iNumVertexBuffers = 2;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;

	HRESULT hr = m_eInstanceType == INSTANCE_STATIC ? Ready_InstanceData_Static(InFile) : Ready_InstanceData_Dynamic(InFile);

	if (FAILED(hr))
		return E_FAIL;

	if (FAILED(Ready_InstanceMaterial(InFile, strModelFilePath)))
		return E_FAIL;

	InFile.close();

	m_Area.reserve(m_iNumMaxInstance);

	return S_OK;
}

HRESULT CVIBuffer_Model_Instancing::Initialize_Clone(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Model_Instancing::Render(_uint iNumInstanceMesh)
{
	if (nullptr == m_pInstanceBuffer)
		return S_OK;

	ID3D11Buffer* pVertexBuffer[] = {
		m_pVertexBuffers[iNumInstanceMesh],
		m_pInstanceBuffer,
	};

	_uint iVertexStrides[] = {
		m_iVertexStride,
		m_iInstanceStride,
	};

	_uint iOffsets[] = {
		0,
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffer, iVertexStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIndexBuffers[iNumInstanceMesh], m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);
	m_pContext->DrawIndexedInstanced(m_iNumIndices[iNumInstanceMesh], m_iNumInstance, 0, 0, 0);
	
	return S_OK;
}
//m_pContext->DrawInstanced(m_iNumVertices[iNumInstanceMesh], m_iNumInstance, 0, 0);
HRESULT CVIBuffer_Model_Instancing::Add_Instance(_float4x4 vInstanceWorld)
{
	m_vInstanceMatrices.push_back(vInstanceWorld);

	m_iNumInstance = (_uint)m_vInstanceMatrices.size();

	D3D11_MAPPED_SUBRESOURCE tInstanceMSR;
	ZeroMemory(&tInstanceMSR, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &tInstanceMSR);

	memcpy(tInstanceMSR.pData, m_vInstanceMatrices.data(), sizeof(_float4x4) * m_iNumInstance);
	
	m_pContext->Unmap(m_pInstanceBuffer, 0);

	return S_OK;
}

HRESULT CVIBuffer_Model_Instancing::Load_InstanceData(const wstring& strInstanceDataPath)
{
	ifstream InFile(strInstanceDataPath, ios::binary);

	if (false == InFile.is_open())
		return E_FAIL;

	m_vInstanceMatrices.clear();

	InFile.read(reinterpret_cast<_char*>(&m_iNumInstance), sizeof(_uint));

	for (_uint i = 0; i < m_iNumInstance; i++)
	{
		_float4x4 vInstanceData;
		InFile.read(reinterpret_cast<_char*>(&vInstanceData), sizeof(_float4x4));

		m_vInstanceMatrices.push_back(vInstanceData);
	}

	D3D11_MAPPED_SUBRESOURCE tInstanceMSR;
	ZeroMemory(&tInstanceMSR, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &tInstanceMSR);

	memcpy(tInstanceMSR.pData, m_vInstanceMatrices.data(), sizeof(_float4x4) * m_iNumInstance);

	m_pContext->Unmap(m_pInstanceBuffer, 0);

	for (_uint fZ = 0; fZ < 16; fZ++) 
	{
		for (_uint fX = 0; fX < 16; fX++) 
			Compute_Area(fZ, fX);
	}

	return S_OK;
}

HRESULT CVIBuffer_Model_Instancing::Save_InstanceData(const wstring& strInstanceDataPath)
{
	ofstream OutFile(strInstanceDataPath, ios::binary);

	if (false == OutFile.is_open())
		return E_FAIL;

	OutFile.write(reinterpret_cast<_char*>(&m_iNumInstance), sizeof(_uint));

	for (_uint i = 0; i < m_iNumInstance; i++)
	{
		_float4x4 vInstanceData = m_vInstanceMatrices[i];
		OutFile.write(reinterpret_cast<_char*>(&vInstanceData), sizeof(_float4x4));
	}

	return S_OK;
}

HRESULT CVIBuffer_Model_Instancing::Clear_InstanceData()
{
	m_vInstanceMatrices.clear();
	
	_float4x4 vIdentity;
	XMStoreFloat4x4(&vIdentity, XMMatrixIdentity());

	m_vInstanceMatrices.push_back(vIdentity);

	m_iNumInstance = m_vInstanceMatrices.size();

	D3D11_MAPPED_SUBRESOURCE tInstanceMSR;
	ZeroMemory(&tInstanceMSR, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &tInstanceMSR);

	memcpy(tInstanceMSR.pData, m_vInstanceMatrices.data(), sizeof(_float4x4) * m_iNumInstance);

	m_pContext->Unmap(m_pInstanceBuffer, 0);
	
	return S_OK;
}

HRESULT CVIBuffer_Model_Instancing::Bind_Material(CShader* pShader, const _char* pConstantName, _uint iNumInstanceMesh, TEXTURETYPE eMaterialType)
{
	if (m_tMaterials[m_iMaterialIndex[iNumInstanceMesh]].pMaterials[eMaterialType] == nullptr)
		return E_NOINTERFACE;

	return m_tMaterials[m_iMaterialIndex[iNumInstanceMesh]].pMaterials[eMaterialType]->Bind_ShaderResource(pShader, pConstantName, 0);
}

HRESULT CVIBuffer_Model_Instancing::Update_InstanceData(_float4* pPosition)
{
	if (nullptr == pPosition)
		return E_FAIL;

	if (pPosition->x < 0 || pPosition->x>1024 || pPosition->z < 0 || pPosition->z>1024)
		return S_OK;

	_uint iRow, iCol;

	iRow = _uint(pPosition->z / 64);
	iCol = _uint(pPosition->x / 64);

	_uint iIndex = iRow * 16 + iCol;

	if (m_iAreaIndex != iIndex)
	{
		m_iAreaIndex = iIndex;
		m_Area.clear();

		_float4x4 vIdentityMatrix;
		XMStoreFloat4x4(&vIdentityMatrix, XMMatrixIdentity());

		m_Area.push_back(vIdentityMatrix);

		// 왼쪽 위
		if (0 == iCol && 15 == iRow)
		{
			// 현재 위치
			for (size_t i = 0; i < m_Areas[iIndex].size(); i++)
				m_Area.push_back(m_Areas[iIndex].data()[i]);

			// 오른쪽
			for (size_t i = 0; i < m_Areas[iIndex + 1].size(); i++)
				m_Area.push_back(m_Areas[iIndex + 1].data()[i]);

			// 아래
			for (size_t i = 0; i < m_Areas[iIndex - 16].size(); i++)
				m_Area.push_back(m_Areas[iIndex - 16].data()[i]);

			// 아래 오른쪽
			for (size_t i = 0; i < m_Areas[iIndex - 16 + 1].size(); i++)
				m_Area.push_back(m_Areas[iIndex - 16 + 1].data()[i]);
		}

		// 왼쪽 아래
		else if (0 == iCol && 0 == iRow)
		{
			// 현재 위치
			for (size_t i = 0; i < m_Areas[iIndex].size(); i++)
				m_Area.push_back(m_Areas[iIndex].data()[i]);

			// 위
			for (size_t i = 0; i < m_Areas[iIndex + 16].size(); i++)
				m_Area.push_back(m_Areas[iIndex + 16].data()[i]);

			// 위 오른쪽
			for (size_t i = 0; i < m_Areas[iIndex + 16 + 1].size(); i++)
				m_Area.push_back(m_Areas[iIndex + 16 + 1].data()[i]);

			// 오른쪽
			for (size_t i = 0; i < m_Areas[iIndex + 1].size(); i++)
				m_Area.push_back(m_Areas[iIndex + 1].data()[i]);
		}

		// 오른쪽 아래
		else if (15 == iCol && 0 == iRow)
		{
			// 현재 위치
			for (size_t i = 0; i < m_Areas[iIndex].size(); i++)
				m_Area.push_back(m_Areas[iIndex].data()[i]);

			// 위
			for (size_t i = 0; i < m_Areas[iIndex + 16].size(); i++)
				m_Area.push_back(m_Areas[iIndex + 16].data()[i]);

			// 위 왼쪽
			for (size_t i = 0; i < m_Areas[iIndex + 16 - 1].size(); i++)
				m_Area.push_back(m_Areas[iIndex + 16 - 1].data()[i]);

			// 왼쪽
			for (size_t i = 0; i < m_Areas[iIndex - 1].size(); i++)
				m_Area.push_back(m_Areas[iIndex - 1].data()[i]);
		}

		// 오른쪽 위
		else if (15 == iCol && 15 == iRow)
		{
			// 현재 위치
			for (size_t i = 0; i < m_Areas[iIndex].size(); i++)
				m_Area.push_back(m_Areas[iIndex].data()[i]);

			// 왼쪽
			for (size_t i = 0; i < m_Areas[iIndex - 1].size(); i++)
				m_Area.push_back(m_Areas[iIndex - 1].data()[i]);

			// 아래
			for (size_t i = 0; i < m_Areas[iIndex - 16].size(); i++)
				m_Area.push_back(m_Areas[iIndex - 16].data()[i]);

			// 아래 왼쪽
			for (size_t i = 0; i < m_Areas[iIndex - 16 - 1].size(); i++)
				m_Area.push_back(m_Areas[iIndex - 16 - 1].data()[i]);
		}

		// 왼쪽
		else if (0 == iCol)
		{
			// 현재 위치
			for (size_t i = 0; i < m_Areas[iIndex].size(); i++)
				m_Area.push_back(m_Areas[iIndex].data()[i]);

			// 위
			for (size_t i = 0; i < m_Areas[iIndex + 16].size(); i++)
				m_Area.push_back(m_Areas[iIndex + 16].data()[i]);

			// 위 오른쪽
			for (size_t i = 0; i < m_Areas[iIndex + 16 + 1].size(); i++)
				m_Area.push_back(m_Areas[iIndex + 16 + 1].data()[i]);

			// 오른쪽
			for (size_t i = 0; i < m_Areas[iIndex + 1].size(); i++)
				m_Area.push_back(m_Areas[iIndex + 1].data()[i]);

			// 아래 오른쪽
			for (size_t i = 0; i < m_Areas[iIndex - 16 + 1].size(); i++)
				m_Area.push_back(m_Areas[iIndex - 16 + 1].data()[i]);

			// 아래
			for (size_t i = 0; i < m_Areas[iIndex - 16].size(); i++)
				m_Area.push_back(m_Areas[iIndex - 16].data()[i]);
		}

		// 오른쪽
		else if (15 == iCol)
		{
			// 현재 위치
			for (size_t i = 0; i < m_Areas[iIndex].size(); i++)
				m_Area.push_back(m_Areas[iIndex].data()[i]);

			// 위
			for (size_t i = 0; i < m_Areas[iIndex + 16].size(); i++)
				m_Area.push_back(m_Areas[iIndex + 16].data()[i]);

			// 위 왼쪽
			for (size_t i = 0; i < m_Areas[iIndex + 16 - 1].size(); i++)
				m_Area.push_back(m_Areas[iIndex + 16 - 1].data()[i]);

			// 왼쪽
			for (size_t i = 0; i < m_Areas[iIndex - 1].size(); i++)
				m_Area.push_back(m_Areas[iIndex - 1].data()[i]);

			// 아래 왼쪽
			for (size_t i = 0; i < m_Areas[iIndex - 16 - 1].size(); i++)
				m_Area.push_back(m_Areas[iIndex - 16 - 1].data()[i]);

			// 아래
			for (size_t i = 0; i < m_Areas[iIndex - 16].size(); i++)
				m_Area.push_back(m_Areas[iIndex - 16].data()[i]);
		}

		// 아래
		else if (0 == iRow)
		{
			// 현재 위치
			for (size_t i = 0; i < m_Areas[iIndex].size(); i++)
				m_Area.push_back(m_Areas[iIndex].data()[i]);

			// 왼쪽
			for (size_t i = 0; i < m_Areas[iIndex - 1].size(); i++)
				m_Area.push_back(m_Areas[iIndex - 1].data()[i]);

			// 위 왼쪽
			for (size_t i = 0; i < m_Areas[iIndex + 16 - 1].size(); i++)
				m_Area.push_back(m_Areas[iIndex + 16 - 1].data()[i]);

			// 위
			for (size_t i = 0; i < m_Areas[iIndex + 16].size(); i++)
				m_Area.push_back(m_Areas[iIndex + 16].data()[i]);

			// 위 오른쪽
			for (size_t i = 0; i < m_Areas[iIndex + 16 + 1].size(); i++)
				m_Area.push_back(m_Areas[iIndex + 16 + 1].data()[i]);

			// 오른쪽
			for (size_t i = 0; i < m_Areas[iIndex + 1].size(); i++)
				m_Area.push_back(m_Areas[iIndex + 1].data()[i]);
		}

		// 위
		else if (15 == iRow)
		{
			// 현재 위치
			for (size_t i = 0; i < m_Areas[iIndex].size(); i++)
				m_Area.push_back(m_Areas[iIndex].data()[i]);

			// 왼쪽
			for (size_t i = 0; i < m_Areas[iIndex - 1].size(); i++)
				m_Area.push_back(m_Areas[iIndex - 1].data()[i]);

			// 아래 왼쪽
			for (size_t i = 0; i < m_Areas[iIndex - 16 - 1].size(); i++)
				m_Area.push_back(m_Areas[iIndex - 16 - 1].data()[i]);

			// 아래
			for (size_t i = 0; i < m_Areas[iIndex - 16].size(); i++)
				m_Area.push_back(m_Areas[iIndex - 16].data()[i]);

			// 아래 오른쪽
			for (size_t i = 0; i < m_Areas[iIndex - 16 + 1].size(); i++)
				m_Area.push_back(m_Areas[iIndex - 16 + 1].data()[i]);

			// 오른쪽
			for (size_t i = 0; i < m_Areas[iIndex + 1].size(); i++)
				m_Area.push_back(m_Areas[iIndex + 1].data()[i]);
		}

		else 
		{
			// 현재 위치
			for (size_t i = 0; i < m_Areas[iIndex].size(); i++)
				m_Area.push_back(m_Areas[iIndex].data()[i]);

			// 위
			for (size_t i = 0; i < m_Areas[iIndex + 16].size(); i++)
				m_Area.push_back(m_Areas[iIndex + 16].data()[i]);

			// 위 오른쪽
			for (size_t i = 0; i < m_Areas[iIndex + 16 + 1].size(); i++)
				m_Area.push_back(m_Areas[iIndex + 16 + 1].data()[i]);

			// 오른쪽
			for (size_t i = 0; i < m_Areas[iIndex + 1].size(); i++)
				m_Area.push_back(m_Areas[iIndex + 1].data()[i]);

			// 아래 오른쪽
			for (size_t i = 0; i < m_Areas[iIndex - 16 + 1].size(); i++)
				m_Area.push_back(m_Areas[iIndex - 16 + 1].data()[i]);

			// 아래
			for (size_t i = 0; i < m_Areas[iIndex - 16].size(); i++)
				m_Area.push_back(m_Areas[iIndex - 16].data()[i]);

			// 아래 왼쪽
			for (size_t i = 0; i < m_Areas[iIndex - 16 - 1].size(); i++)
				m_Area.push_back(m_Areas[iIndex - 16 - 1].data()[i]);

			// 왼쪽
			for (size_t i = 0; i < m_Areas[iIndex - 1].size(); i++)
				m_Area.push_back(m_Areas[iIndex - 1].data()[i]);

			// 위 왼쪽
			for (size_t i = 0; i < m_Areas[iIndex + 16 - 1].size(); i++)
				m_Area.push_back(m_Areas[iIndex + 16 - 1].data()[i]);
		}

		m_iNumInstance = (_uint)m_Area.size();

		D3D11_MAPPED_SUBRESOURCE tInstanceMSR;
		ZeroMemory(&tInstanceMSR, sizeof(D3D11_MAPPED_SUBRESOURCE));

		m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &tInstanceMSR);

		memcpy(tInstanceMSR.pData, m_Area.data(), sizeof(_float4x4) * m_iNumInstance);

		m_pContext->Unmap(m_pInstanceBuffer, 0);
	}

	return S_OK;
}

HRESULT CVIBuffer_Model_Instancing::Ready_InstanceData_Static(ifstream& InFile)
{
	InFile.read(reinterpret_cast<char*>(&m_iNumMeshes), sizeof(_uint));
	m_iVertexStride = sizeof(VTXMESH);

	for (_uint i = 0; i < m_iNumMeshes; i++) 
	{
		_char szName[MAX_PATH];
		_uint iMaterialIndex;
		_uint iNumVertices;
		_uint iNumIndices;

		InFile.read(reinterpret_cast<char*>(&szName), sizeof(char[MAX_PATH]));
		InFile.read(reinterpret_cast<char*>(&iMaterialIndex), sizeof _uint);
		InFile.read(reinterpret_cast<char*>(&iNumVertices), sizeof _uint);
		InFile.read(reinterpret_cast<char*>(&iNumIndices), sizeof _uint);

		m_iMaterialIndex.push_back(iMaterialIndex);
		m_iNumVertices.push_back(iNumVertices);
		m_iNumIndices.push_back(iNumIndices);

#pragma region Vertex Info
		XMFLOAT3* vPosition = new XMFLOAT3[iNumVertices];
		XMFLOAT3* vNormal = new XMFLOAT3[iNumVertices];
		XMFLOAT3* vTextureCoord = new XMFLOAT3[iNumVertices];
		XMFLOAT3* vTangent = new XMFLOAT3[iNumVertices];

		InFile.read(reinterpret_cast<char*>(vPosition), sizeof _float3 * iNumVertices);
		InFile.read(reinterpret_cast<char*>(vNormal), sizeof _float3 * iNumVertices);
		InFile.read(reinterpret_cast<char*>(vTextureCoord), sizeof _float3 * iNumVertices);
		InFile.read(reinterpret_cast<char*>(vTangent), sizeof _float3 * iNumVertices);

		D3D11_BUFFER_DESC tVertexBufferDesc = {};
		{
			tVertexBufferDesc.ByteWidth = m_iVertexStride * iNumVertices;
			tVertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			tVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			tVertexBufferDesc.CPUAccessFlags = 0;
			tVertexBufferDesc.MiscFlags = 0;
			tVertexBufferDesc.StructureByteStride = m_iVertexStride;
		}

		VTXMESH* pVertices = new VTXMESH[iNumVertices];
		ZeroMemory(pVertices, sizeof(VTXMESH) * iNumVertices);

		for (_uint i = 0; i < iNumVertices; i++)
		{
			memcpy(&pVertices[i].vPosition, &vPosition[i], sizeof(_float3));
			XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), XMLoadFloat4x4(&m_vInstancePivot)));

			memcpy(&pVertices[i].vNormal, &vNormal[i], sizeof(_float3));
			XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), XMLoadFloat4x4(&m_vInstancePivot)));

			memcpy(&pVertices[i].vTexcoord, &vTextureCoord[i], sizeof(_float2));

			memcpy(&pVertices[i].vTangent, &vTangent[i], sizeof(_float3));
		}

		D3D11_SUBRESOURCE_DATA tInstanceVertexData = {};
		tInstanceVertexData.pSysMem = pVertices;

		ID3D11Buffer* pVB = nullptr;

		if (FAILED(m_pDevice->CreateBuffer(&tVertexBufferDesc, &tInstanceVertexData, &pVB)))
			return E_FAIL;

		m_pVertexBuffers.push_back(pVB);

		Safe_Delete_Array(vPosition);
		Safe_Delete_Array(vNormal);
		Safe_Delete_Array(vTextureCoord);
		Safe_Delete_Array(vTangent);
		Safe_Delete_Array(pVertices);
#pragma endregion

#pragma region Index Info
		D3D11_BUFFER_DESC tIndexBufferDesc = {};
		{
			tIndexBufferDesc.ByteWidth = 4 * iNumIndices;
			tIndexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			tIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			tIndexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			tIndexBufferDesc.MiscFlags = 0;
			tIndexBufferDesc.StructureByteStride = /*m_iIndexStride*/0;
		}

		_ulong* pIndices = new _ulong[iNumIndices];
		InFile.read(reinterpret_cast<char*>(pIndices), sizeof _ulong * iNumIndices);

		D3D11_SUBRESOURCE_DATA tInstanceIndexData = {};
		tInstanceIndexData.pSysMem = pIndices;

		ID3D11Buffer* pIB = nullptr;

		if (FAILED(m_pDevice->CreateBuffer(&tIndexBufferDesc, &tInstanceIndexData, &pIB)))
			return E_FAIL;

		m_pIndexBuffers.push_back(pIB);

		Safe_Delete_Array(pIndices);
#pragma endregion
	}

	return S_OK;
}

HRESULT CVIBuffer_Model_Instancing::Ready_InstanceData_Dynamic(ifstream& InFile)
{
	return S_OK;
}

HRESULT CVIBuffer_Model_Instancing::Ready_InstanceMaterial(ifstream& InFile, const wstring& strModelFilePath)
{
	InFile.read(reinterpret_cast<char*>(&m_iNumMaterials), sizeof(_uint));
	string szModelFilePath = ConvertWCToC(strModelFilePath);

	_char		szDrive[MAX_PATH] = "";
	_char		szDirectory[MAX_PATH] = "";
	_char		szFileName[MAX_PATH] = "";
	_char		szExt[MAX_PATH] = "";

	_char		szTextureFilePath[MAX_PATH] = "";

	_splitpath_s(szModelFilePath.c_str(), szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		MODEL_MATERIAL tModelMaterials = {};

		_uint iNumTexture = 0;
		InFile.read(reinterpret_cast<char*>(&iNumTexture), sizeof _uint);

		for (size_t j = 0; j < iNumTexture; j++)
		{
			_uint	iMaterialIdx = 0;
			char	szTexturePath[MAX_PATH];

			InFile.read(reinterpret_cast<char*>(&iMaterialIdx), sizeof _uint);
			InFile.read(reinterpret_cast<char*>(&szTexturePath), sizeof(char[MAX_PATH]));

			_splitpath_s(szTexturePath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			strcpy_s(szTextureFilePath, szDrive);
			strcat_s(szTextureFilePath, szDirectory);
			strcat_s(szTextureFilePath, szFileName);
			strcat_s(szTextureFilePath, szExt);

			wstring strPrototypeTag = L"Prototype_Component_Texture_";
			strPrototypeTag += ConvertCToWC(szFileName);

			CTexture* pTexture = static_cast<CTexture*>(m_pGameInstance->Find_Component_Prototype(0, strPrototypeTag));

			if (pTexture == nullptr)
			{
				_tchar szFullPath[MAX_PATH] = L"";
				MultiByteToWideChar(CP_ACP, 0, szTextureFilePath, static_cast<_int>(strlen(szTextureFilePath)), szFullPath, MAX_PATH);

				if (FAILED(m_pGameInstance->Add_Component_Prototype(0, strPrototypeTag,
					pTexture = CTexture::Create(m_pDevice, m_pContext, szFullPath))))
					return E_FAIL;
			}

			if (pTexture == nullptr)
				return E_FAIL;

			tModelMaterials.pMaterials[iMaterialIdx] = pTexture;
			Safe_AddRef(pTexture);

			if (nullptr == tModelMaterials.pMaterials[iMaterialIdx])
				return E_FAIL;
		}

		m_tMaterials.push_back(tModelMaterials);
	}

	return S_OK;
}

void CVIBuffer_Model_Instancing::Compute_Area(_uint iRow, _uint iCol)
{
	_uint iIndex = iRow * 16 + iCol;

	_float fMinX, fMaxX, fMinZ, fMaxZ;

	fMaxZ = 64.0f * iRow + 64.0f;
	fMinZ = fMaxZ - 64.0f;

	fMaxX = 64.0f * iCol + 64.0f;
	fMinX = fMaxX - 64.0f;

	for (size_t i = 0; i < m_vInstanceMatrices.size(); i++) 
	{
		_float4 vPosition = _float4(m_vInstanceMatrices[i].m[3][0], m_vInstanceMatrices[i].m[3][1], m_vInstanceMatrices[i].m[3][2], m_vInstanceMatrices[i].m[3][3]);

		if ((vPosition.x >= fMinX && vPosition.x < fMaxX) &&
			(vPosition.z >= fMinZ && vPosition.z < fMaxZ)) 
		{
			m_Areas[iIndex].push_back(m_vInstanceMatrices[i]);
		}
	}
}

CVIBuffer_Model_Instancing* CVIBuffer_Model_Instancing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strModelFilePath)
{
	CVIBuffer_Model_Instancing* pInstance = new CVIBuffer_Model_Instancing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strModelFilePath)))
	{
		MSG_BOX(TEXT("Failed to Create : CVIBuffer_Model_Instancing"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Model_Instancing::Clone(void* pArg)
{
	CVIBuffer_Model_Instancing* pInstance = new CVIBuffer_Model_Instancing(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CVIBuffer_Model_Instancing"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Model_Instancing::Free()
{
	__super::Free();

	for (auto& pVB : m_pVertexBuffers)
		Safe_Release(pVB);

	m_pVertexBuffers.clear();

	for (auto& pIB : m_pIndexBuffers)
		Safe_Release(pIB);

	m_pIndexBuffers.clear();

	for (auto& tMaterial : m_tMaterials)
		for (size_t i = 0; i < ::AI_TEXTURE_TYPE_MAX; i++)
			Safe_Release(tMaterial.pMaterials[i]);

	m_tMaterials.clear();

	Safe_Release(m_pInstanceBuffer);

	m_iMaterialIndex.clear();
	m_iNumVertices.clear();
	m_vInstanceMatrices.clear();
}
