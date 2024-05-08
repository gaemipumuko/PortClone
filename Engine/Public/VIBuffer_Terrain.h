#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
protected:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	_uint Get_NumVertices() { return m_iNumVertices; }
	_uint Get_NumIndices() { return m_iNumIndices; }
	_uint Get_NumVerticesX() { return m_iNumVerticesX; }
	_uint Get_NumVerticesZ() { return m_iNumVerticesZ; }

	_float3* Get_VerticesPos() { return m_pVerticesPos; }
	_ulong* Get_Indices() { return m_pIndices; }
	_float3* Get_Normals() { return m_pNormals; }
	_float2* Get_TexCoord() { return m_pTexCoord; }

	ID3D11Buffer* Get_VB() { return m_pVB; }
	ID3D11Buffer* Get_IB() { return m_pIB; }

public:
	virtual HRESULT Initialize_Prototype( _uint iNumVerticesX, _uint iNumVerticesZ);
	virtual HRESULT Initialize_Prototype( _uint iNumVerticesX, _uint iNumVerticesZ, _float fGridSpacing);
	virtual HRESULT Initialize_Prototype(const wstring& strDataFilePath);
	virtual HRESULT Initialize_Clone(void* pArg) override;

public: /* For.Culling */
	HRESULT Initialize_QuadTree();
	void Culling_Frustum(_fmatrix WorldMatrix);
	void Culling_QuadTree(_fmatrix WorldMatrix);

public: /* For.Terrain Edit */
	HRESULT Resize_Terrain(_uint iNumVerticesX, _uint iNumVerticesZ);
	HRESULT Apply_HeightMap(const wstring& strDataFilePath);
	HRESULT Save_HeightMap(const wstring& strDataFilePath);
	HRESULT Brushing_OnTerrain(_float& fBrushForce, _float3& vBrushPos, _float& fBrushSize, _float& fStrength, _float& fMaxHeight);

public: /* For.Ray */
	_float3 Compute_RayCast(const _float3 vRayPos_InWorld, const _float3 vRayDir_InWorld, const _matrix WorldMatrix);

private:
	_bool RayCast_InTerrain(const _float3 vRayPos_InLocal, const _float3 vRayDir_InLocal, const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, _float3* vPickPos);
	_float Compute_BrushInfluence(_float3& vBrushPoint, _float3& vBrushPos, _float& fBrushSize, _float& fStrength);
	
private:
	_uint m_iNumVerticesX = 0;
	_uint m_iNumVerticesZ = 0;

	_float3* m_pNormals = { nullptr };
	_float2* m_pTexCoord = { nullptr };

	class CQuadTree* m_pQuadTree = { nullptr };

public:
	static CVIBuffer_Terrain*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumVerticesX, _uint iNumVerticesZ);
	static CVIBuffer_Terrain*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumVerticesX, _uint iNumVerticesZ, _float fGridSpacing);
	static CVIBuffer_Terrain*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strDataFilePath);
	virtual CComponent*			Clone(void* pArg) override;
	virtual void				Free() override;
};

END