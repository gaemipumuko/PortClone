#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Model_Instancing final : public CComponent
{
public:
	enum INSTANCE_TYPE { INSTANCE_STATIC, INSTANCE_DYNAMIC, INSTANCE_END };

private:
	CVIBuffer_Model_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Model_Instancing(const CVIBuffer_Model_Instancing& rhs);
	virtual ~CVIBuffer_Model_Instancing() = default;

public:
	_uint Get_NumMeshes() { return m_iNumMeshes; }

public:
	virtual HRESULT Initialize_Prototype(const wstring& strModelFilePath);
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual HRESULT Render(_uint iNumInstanceMesh);

public:
	HRESULT Add_Instance(_float4x4 vInstanceWorld);
	HRESULT Load_InstanceData(const wstring& strInstanceDataPath);
	HRESULT Save_InstanceData(const wstring& strInstanceDataPath);
	HRESULT Clear_InstanceData();

public:
	HRESULT Bind_Material(class CShader* pShader, const _char* pConstantName, _uint iNumInstanceMesh, TEXTURETYPE eMaterialType);
	HRESULT Update_InstanceData(_float4* pPosition);

private:
	HRESULT Ready_InstanceData_Static(ifstream& InFile);
	HRESULT Ready_InstanceData_Dynamic(ifstream& InFile);
	HRESULT Ready_InstanceMaterial(ifstream& InFile, const wstring& strModelFilePath);

	void Compute_Area(_uint iRow, _uint iCol);

private: /* Common */
	_uint m_iNumVertexBuffers = 0;
	D3D11_PRIMITIVE_TOPOLOGY m_ePrimitiveTopology = { (D3D11_PRIMITIVE_TOPOLOGY)0 };
	DXGI_FORMAT m_eIndexFormat = { (DXGI_FORMAT)0 };

private: /* Model Info */
	INSTANCE_TYPE m_eInstanceType = INSTANCE_END;
	_float4x4 m_vInstancePivot;
	_uint m_iNumMeshes = {};
	_uint m_iNumMaterials = {};
	vector<_uint> m_iMaterialIndex;
	vector<MODEL_MATERIAL> m_tMaterials;

private: /* Vertex Info */
	_uint m_iVertexStride = 0;
	vector<_uint> m_iNumVertices;
	vector<ID3D11Buffer*> m_pVertexBuffers;

private: /* Index Info */
	_uint m_iIndexStride = 0;
	vector<_uint> m_iNumIndices;
	vector<ID3D11Buffer*> m_pIndexBuffers;

private: /* Instance Info */
	_uint m_iNumInstance = 0;
	_uint m_iInstanceStride = 0;
	ID3D11Buffer* m_pInstanceBuffer = { nullptr };
	vector<_float4x4> m_vInstanceMatrices;
	_uint m_iNumMaxInstance = 100000;

private: /* For.Area */
	_uint m_iAreaIndex = 256;
	vector<_float4x4> m_Areas[256];
	vector<_float4x4> m_Area;

public:
	static CVIBuffer_Model_Instancing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strModelFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
