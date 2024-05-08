#pragma once

#include "Tool_Defines.h"
#include "VIBuffer.h"
#include "Edit_Model.h"

BEGIN(Engine)
class CShader;
END

BEGIN(Tool)

class CEdit_Mesh final : public CVIBuffer
{
private:
	CEdit_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEdit_Mesh(const CEdit_Mesh& rhs);
	virtual ~CEdit_Mesh() = default;

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
	HRESULT Bind_BoneMatrices(CShader* pShader, const vector<class CEdit_Bone*>& Bones, const _char* pConstantName, _fmatrix PivotMatrix);

private:
	HRESULT Ready_NonAnim_Mesh(const MESH_DESC* pMeshDesc, _fmatrix PivotMatrix);
	HRESULT Ready_Anim_Mesh(const MESH_DESC* pMeshDesc, const vector<class CEdit_Bone*>& Bones);

public:
	virtual HRESULT Initialize_Prototype(CEdit_Model::MODEL_TYPE eModelType, const MESH_DESC* pMeshDesc, const vector<class CEdit_Bone*>& Bones, _fmatrix PivotMatrix);
	virtual HRESULT Initialize_Clone(void* pArg) override;

public:
	static	CEdit_Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CEdit_Model::MODEL_TYPE eModelType, const MESH_DESC* pMeshDesc, const vector<class CEdit_Bone*>& Bones, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void		Free() override;
};

END
