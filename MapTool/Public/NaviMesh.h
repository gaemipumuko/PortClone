#pragma once

#include "Tool_Defines.h"
#include "Component.h"

#ifdef _DEBUG
BEGIN(Engine)
class CShader;
END
#endif

BEGIN(Tool)

class CNaviMesh final : public CComponent
{
public:
	typedef struct tagNavigationDesc
	{
		_int			iCurrentCellIndex = -1;
	} NAVIGATION_DESC;

private:
	CNaviMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNaviMesh(const CNaviMesh& rhs);
	virtual ~CNaviMesh() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strDataFilePath);
	virtual HRESULT Initialize(void* pArg);
public:
	void Update(_fmatrix WorldMatrix);
	_bool isMove(_fvector vPosition);

public:
	HRESULT Load_NaviMesh(const wstring& strDataFilePath);
	HRESULT Add_Triangle(_float3* vPoints);
	HRESULT Delete_Triangle(_float3 vPoint);
	HRESULT Search_Point(_float3* pPoint);
	HRESULT Save_Triangles();

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif

#ifdef _DEBUG
private:
	CShader* m_pDebugShader = { nullptr };
#endif

private:
	vector<class CTriangle*> m_pTriangles;
	static _float4x4 m_WorldMatrix;
	_int m_iCurrentCellIndex = { -1 };
	_int m_iNumTriangle = {};

private:
	HRESULT SetUp_Neighbors();

public:
	static CNaviMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strDataFilePath);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END
