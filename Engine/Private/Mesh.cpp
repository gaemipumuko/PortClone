#include "Mesh.h"

#include "Bone.h"
#include "Shader.h"

#include"GameInstance.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CMesh::Bind_BoneMatrices(CShader* pShader, const vector<class CBone*>& Bones, const _char* pConstantName, _fmatrix PivotMatrix)
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

HRESULT CMesh::Cooking_StaticMesh(_float4x4 vWorldMatrix, const char* pMeshName, PxRigidStatic** ppRigidStatic)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr == pGameInstance)
		return E_FAIL;

	if (FAILED(pGameInstance->Create_StaticMesh(m_iNumVertices, m_pVerticesPos, m_iNumIndices, m_pIndices, vWorldMatrix, ppRigidStatic)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CMesh::Compute_VertexExtremes(_float3* pMin, _float3* pMax)
{
	if (nullptr == pMin || nullptr == pMax)
		return E_FAIL;

	for (_uint i = 0; i < m_iNumVertices; i++) 
	{
		if (m_pVerticesPos[i].x < pMin->x)
			pMin->x = m_pVerticesPos[i].x;

		if (m_pVerticesPos[i].y < pMin->y)
			pMin->y = m_pVerticesPos[i].y;

		if (m_pVerticesPos[i].z < pMin->z)
			pMin->z = m_pVerticesPos[i].z;

		if (m_pVerticesPos[i].x > pMax->x)
			pMax->x = m_pVerticesPos[i].x;

		if (m_pVerticesPos[i].y > pMax->y)
			pMax->y = m_pVerticesPos[i].y;

		if (m_pVerticesPos[i].z > pMax->z)
			pMax->z = m_pVerticesPos[i].z;
	}

	return S_OK;
}

HRESULT CMesh::Ready_NonAnim_Mesh(const MESH_DESC* pMeshDesc, _fmatrix PivotMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	m_pVerticesPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition,	&pMeshDesc->vPositions[i],		sizeof (_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));

		m_pVerticesPos[i] = pVertices[i].vPosition;

		memcpy(&pVertices[i].vNormal,	&pMeshDesc->vNormals[i],		sizeof (_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PivotMatrix));

		memcpy(&pVertices[i].vTexcoord,	&pMeshDesc->vTextureCoords[i],	sizeof (_float2));

		memcpy(&pVertices[i].vTangent,	&pMeshDesc->vTangents[i],		sizeof (_float3));
	}

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_Anim_Mesh(const MESH_DESC* pMeshDesc, const vector<class CBone*>& Bones)
{
	m_iVertexStride = sizeof(VTXANIMMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	m_pVerticesPos = new _float3[m_iNumVertices];
	ZeroMemory(m_pVerticesPos, sizeof(_float3) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition,		&pMeshDesc->vPositions[i],		sizeof (_float3));

		m_pVerticesPos[i] = pVertices[i].vPosition;

		memcpy(&pVertices[i].vNormal,		&pMeshDesc->vNormals[i],		sizeof (_float3));
		memcpy(&pVertices[i].vTexcoord,		&pMeshDesc->vTextureCoords[i],	sizeof (_float2));
		memcpy(&pVertices[i].vTangent,		&pMeshDesc->vTangents[i],		sizeof (_float3));
		memcpy(&pVertices[i].vBlendIndices, &pMeshDesc->vBlendIndices[i],	sizeof (XMUINT4));
		memcpy(&pVertices[i].vBlendWeights, &pMeshDesc->vBlendWeights[i],	sizeof (_float4));
	}

	m_iNumBones = pMeshDesc->iNumBones;
	if (m_iNumBones == 0)
	{
		m_iNumBones = 1;

		_uint		iBoneIndex = 0;

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool {
			if (0 == strcmp(pBone->Get_BoneName(), m_szName))
				return true;
			++iBoneIndex;
			return false;
			});

		if (iter == Bones.end())
			iBoneIndex = 2;

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

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Initialize_Prototype(CModel::MODEL_TYPE eModelType, const MESH_DESC* pMeshDesc, const vector<class CBone*>& Bones, _fmatrix PivotMatrix)
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

	HRESULT		hr = eModelType == CModel::MTYPE_NONANIM ? Ready_NonAnim_Mesh(pMeshDesc, PivotMatrix) : Ready_Anim_Mesh(pMeshDesc, Bones);
	if (FAILED(hr))
		return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = /*m_iIndexStride*/0;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);

	_ulong* pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ulong) * m_iNumIndices);

	m_pIndices = new _ulong[m_iNumIndices];
	ZeroMemory(m_pIndices, sizeof(_ulong) * m_iNumIndices);

	for (size_t i = 0; i < pMeshDesc->iNumIndices; ++i)
	{
		m_pIndices[i] = pIndices[i] = pMeshDesc->pIndices[i];
	}

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CMesh::Initialize_Clone(void* pArg)
{
	return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::MODEL_TYPE eModelType, const MESH_DESC* pMeshDesc, const vector<class CBone*>& Bones, _fmatrix PivotMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pMeshDesc, Bones, PivotMatrix)))
	{
		MSG_BOX(TEXT("Failed to Create : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CMesh::Clone(void* pArg)
{
	CMesh* pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	__super::Free();

	if (false == m_bCloned)
		Safe_Delete_Array(m_pVerticesPos);

	if (false == m_bCloned)
		Safe_Delete_Array(m_pIndices);
}
