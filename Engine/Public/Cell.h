#pragma once

#include "Base.h"

BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };

private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	#ifdef _DEBUG
private:
	class CVIBuffer_Cell*	m_pVIBuffer = { nullptr };
#endif

private:
	_float3			m_vPoints[POINT_END] = { };
	_float3			m_vNormals[LINE_END] = { };
	_int			m_iIndex = { 0 };
	_int			m_iNeighborIndices[LINE_END] = { -1, -1, -1 };

public:
	_int		Get_Index() { return m_iIndex; }
	_vector		Get_Point(POINT ePoint) const { return XMLoadFloat3(&m_vPoints[ePoint]); }

	_vector		Get_SlidingVector(_fvector vLocalPos, _fvector vMoveDir);

	_float		Get_CellHeight() { return (m_vPoints[POINT_A].y + m_vPoints[POINT_B].y + m_vPoints[POINT_C].y) / 3.f; }

public:
	_bool		isIn(_fvector vLocalPos, _int* pNeighborIndex);

public:
	HRESULT		Initialize(const _float3* pPoints, const _int* pIndices, _int iIndex);
#ifdef _DEBUG
	HRESULT		Render();
#endif

public:
	static CCell*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, const _int* pIndices, _int iIndex);
	virtual void	Free() override;
};

END