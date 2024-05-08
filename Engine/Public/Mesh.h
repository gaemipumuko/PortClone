#pragma once

#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

private:
	char				m_szName[MAX_PATH] = "";

	_uint				m_iNumBones = { 0 };
	vector<_uint>		m_BoneIndices;
	vector<_float4x4>	m_OffsetMatices;

	_uint				m_iMaterialIndex = { 0 };

public:
	_uint		Get_NumVertices() { return m_iNumVertices; }
	_uint		Get_NumIndices() { return m_iNumIndices; }

	char*		Get_Name() { return m_szName; }
	_uint		Get_MaterialIndex() { return m_iMaterialIndex; }

public:
	HRESULT Bind_BoneMatrices(CShader* pShader, const vector<class CBone*>& Bones, const _char* pConstantName, _fmatrix PivotMatrix);

public:
	HRESULT Cooking_StaticMesh(_float4x4 vWorldMatrix, const char* pMeshName, PxRigidStatic** ppRigidStatic = nullptr);
	HRESULT Compute_VertexExtremes(_float3* pMin, _float3* pMax);

private:
	HRESULT Ready_NonAnim_Mesh(const MESH_DESC* pMeshDesc, _fmatrix PivotMatrix);
	HRESULT Ready_Anim_Mesh(const MESH_DESC* pMeshDesc, const vector<class CBone*>& Bones);

public:
	virtual HRESULT Initialize_Prototype(CModel::MODEL_TYPE eModelType, const MESH_DESC* pMeshDesc, const vector<class CBone*>& Bones, _fmatrix PivotMatrix);
	virtual HRESULT Initialize_Clone(void* pArg) override;

public:
	static	CMesh*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::MODEL_TYPE eModelType, const MESH_DESC* pMeshDesc, const vector<class CBone*>& Bones, _fmatrix PivotMatrix);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};

END