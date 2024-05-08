#include "Edit_Mesh.h"
#include "Edit_Bone.h"
#include "Shader.h"

CEdit_Mesh::CEdit_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CEdit_Mesh::CEdit_Mesh(const CEdit_Mesh& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CEdit_Mesh::Bind_BoneMatrices(CShader* pShader, const vector<class CEdit_Bone*>& Bones, const _char* pConstantName, _fmatrix PivotMatrix)
{
	_float4x4		BoneMatrices[256] = {};
	ZeroMemory(BoneMatrices, sizeof(_float4x4) * 256);

	_uint			iNumBones = 0;

	for (size_t i = 0; i < m_iNumBones; i++)
	{
		XMStoreFloat4x4(&BoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatices[i]) *
			XMLoadFloat4x4(Bones[m_BoneIndices[i]]->Get_CombinedMatrix()) * PivotMatrix);
	}

	return pShader->Bind_Matrices(pConstantName, BoneMatrices, m_iNumBones);
}

HRESULT CEdit_Mesh::Ready_NonAnim_Mesh(const MESH_DESC* pMeshDesc, _fmatrix PivotMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	{
		m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_BufferDesc.CPUAccessFlags = 0;
		m_BufferDesc.MiscFlags = 0;
		m_BufferDesc.StructureByteStride = m_iVertexStride;
	}

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pMeshDesc->vPositions[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));

		memcpy(&pVertices[i].vNormal, &pMeshDesc->vNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PivotMatrix));

		memcpy(&pVertices[i].vTexcoord, &pMeshDesc->vTextureCoords[i], sizeof(_float2));

		memcpy(&pVertices[i].vTangent, &pMeshDesc->vTangents[i], sizeof(_float3));
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CEdit_Mesh::Ready_Anim_Mesh(const MESH_DESC* pMeshDesc, const vector<class CEdit_Bone*>& Bones)
{
	m_iVertexStride = sizeof(VTXANIMMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	{
		m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		m_BufferDesc.CPUAccessFlags = 0;
		m_BufferDesc.MiscFlags = 0;
		m_BufferDesc.StructureByteStride = m_iVertexStride;
	}

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pMeshDesc->vPositions[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pMeshDesc->vNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &pMeshDesc->vTextureCoords[i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pMeshDesc->vTangents[i], sizeof(_float3));
		memcpy(&pVertices[i].vBlendIndices, &pMeshDesc->vBlendIndices[i], sizeof(XMUINT4));
		memcpy(&pVertices[i].vBlendWeights, &pMeshDesc->vBlendWeights[i], sizeof(_float4));
	}

	m_iNumBones = pMeshDesc->iNumBones;
	if (m_iNumBones == 0)
	{
		m_iNumBones = 1;

		_uint iBoneIndex = 0;

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CEdit_Bone* pBone)->_bool {
			if (0 == strcmp(pBone->Get_BoneName(), m_szName))
				return true;
			++iBoneIndex;
			return false;
			});

		_float4x4		OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

		m_BoneIndices.push_back(iBoneIndex);
		m_OffsetMatices.push_back(OffsetMatrix);
	}
	else
	{
		m_BoneIndices.reserve(m_iNumBones);
		m_OffsetMatices.reserve(m_iNumBones);

		for (size_t idx = 0; idx < m_iNumBones; ++idx)
		{
			m_BoneIndices.push_back(pMeshDesc->BoneIndices[idx]);
			m_OffsetMatices.push_back(pMeshDesc->OffsetMatices[idx]);
		}
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CEdit_Mesh::Initialize_Prototype(CEdit_Model::MODEL_TYPE eModelType, const MESH_DESC* pMeshDesc, const vector<class CEdit_Bone*>& Bones, _fmatrix PivotMatrix)
{
	/* Mesh 정보 저장 */
	strcpy_s(m_szName, pMeshDesc->szName);
	m_iMaterialIndex = pMeshDesc->iMaterialIndex;

	/* VIBuffer 생성 */
	m_iNumVertexBuffers = 1;
	m_iNumVertices = pMeshDesc->iNumVertices;

	m_iIndexStride = 4;
	m_iNumIndices = pMeshDesc->iNumIndices;

	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	HRESULT		hr = eModelType == CEdit_Model::MTYPE_NONANIM ? Ready_NonAnim_Mesh(pMeshDesc, PivotMatrix) : Ready_Anim_Mesh(pMeshDesc, Bones);
	if (FAILED(hr))
		return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	{
		m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		m_BufferDesc.CPUAccessFlags = 0;
		m_BufferDesc.MiscFlags = 0;
		m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;
	}

	_ulong* pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

	for (size_t i = 0; i < pMeshDesc->iNumIndices; ++i)
	{
		pIndices[i] = pMeshDesc->pIndices[i];
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CEdit_Mesh::Initialize_Clone(void* pArg)
{
	return S_OK;
}

CEdit_Mesh* CEdit_Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CEdit_Model::MODEL_TYPE eModelType, const MESH_DESC* pMeshDesc, const vector<class CEdit_Bone*>& Bones, _fmatrix PivotMatrix)
{
	CEdit_Mesh* pInstance = new CEdit_Mesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pMeshDesc, Bones, PivotMatrix)))
	{
		MSG_BOX(TEXT("Failed to Create : CEdit_Mesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CEdit_Mesh::Clone(void* pArg)
{
	CEdit_Mesh* pInstance = new CEdit_Mesh(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CEdit_Mesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEdit_Mesh::Free()
{
	__super::Free();
}
