#pragma once

#include "Tool_Defines.h"
#include "Base.h"

#ifdef _DEBUG
BEGIN(Engine)
class CVIBuffer_Cell;
END
#endif

BEGIN(Tool)

class CTriangle  final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };

private:
	CTriangle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTriangle() = default;

public:
	_vector Get_Point(POINT ePoint) const {
		return XMLoadFloat3(&m_vPoints[ePoint]);
	}

	_float3* Get_Points() { return m_vPoints; }
	_int* Get_Neighbors() { return m_iNeighborIndices; }

	void Set_Neighbor(LINE eLine, CTriangle* pNeighbor) {
		m_iNeighborIndices[eLine] = pNeighbor->m_iIndex;
	}

public:
	HRESULT Initialize(const _float3* pPoints, _int iIndex);
	HRESULT Initialize(const _float3* pPoints, const _int* pIndices, _int iIndex);
	_bool Compare_Points(_fvector vPoint_A, _fvector vPoint_B);
	_bool isIn(_fvector vPosition, _int* pNeighborIndex);

#ifdef _DEBUG

public:
	HRESULT Render();

#endif

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	_float3			m_vPoints[POINT_END] = {};
	_int			m_iIndex = { 0 };
	_int			m_iNeighborIndices[LINE_END] = { -1, -1, -1 };
	_float3			m_vNormals[LINE_END];

#ifdef _DEBUG
private:
	CVIBuffer_Cell* m_pVIBufferCom = { nullptr };
#endif

public:
	static CTriangle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex);
	static CTriangle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, const _int* pIndices, _int iIndex);
	virtual void Free() override;
};

END
