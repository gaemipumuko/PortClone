#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CInstancing_Model final : public CComponent
{
public:
	enum INSTANCE_TYPE { INSTANCE_STATIC, INSTANCE_DYNAMIC, INSTANCE_END };

private:
	CInstancing_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInstancing_Model(const CInstancing_Model& rhs);
	virtual ~CInstancing_Model() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strModelFilePath, _uint iNumInstance);
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual HRESULT Render(_uint iMeshIndex);

public:
	_uint Get_NumMeshes() { return m_iNumMeshes; }

	void Invalidate_Model(_float fTimeDelta);
	HRESULT Bind_Material(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, TEXTURETYPE eMaterialType);

private:
	HRESULT Ready_StaticMesh(ifstream& InFile);
	HRESULT Ready_DynamicMesh(ifstream& InFile);

	HRESULT Ready_Material(ifstream& InFile, const wstring& strModelFilePath);

private: /* For.Common Variable */
	INSTANCE_TYPE m_eInstanceType = INSTANCE_END;
	_float4x4 m_vPivotMatrix;

	_uint m_iNumMeshes;
	
	vector<ID3D11Buffer*> m_pVertexBuffers;
	vector<ID3D11Buffer*> m_pIndexBuffers;
	vector<_uint> m_iMaterialIndex;
	vector<_uint> m_iNumVertices;
	vector<_uint> m_iNumIndices;

	_uint m_iNumVertexBuffers = { 0 };
	_uint m_iVertexStride = { 0 };

	DXGI_FORMAT m_eIndexFormat;

	D3D11_PRIMITIVE_TOPOLOGY m_ePrimitiveTopology;

	_uint m_iNumMaterials;
	vector<MODEL_MATERIAL> m_tMaterials;

private: /* For.Instancing */
	ID3D11Buffer* m_pInstanceBuffer = { nullptr };

	_uint m_iInstanceStride = { 0 };
	_uint m_iNumInstance = { 0 };

public:
	static CInstancing_Model* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strModelFilePath, _uint iNumInstance);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
