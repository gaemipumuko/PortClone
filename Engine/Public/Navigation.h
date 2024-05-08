#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNavigationDesc
	{
		_int			iCurrentCellIndex = { -1 };
	} NAVIGATION_DESC;

private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

private:
	vector<class CCell*>	m_Cells;
	static _float4x4		m_WorldMatrix;
	_int					m_iNumCell = { };
	_int					m_iCurrentCellIndex = { -1 };

#ifdef _DEBUG
private:
	class CShader*			m_pDebugShader = { nullptr };
#endif

public:
	HRESULT	Set_CellIndex(_int iCellIndex);
	HRESULT	Set_CellIndex(_fvector vPosition);
	_int	Get_CellIndex() { return m_iCurrentCellIndex; }

	_bool	isMove(_fvector vPosition);
	void	Invalidate_Navigation(_fmatrix WorldMatrix);	/* Terrain에서 자신의 WorldMatrix를 Update */

	_vector	Get_CurrCellCenter();
	_vector	Get_CellCenter(_int iCellIndex);
	_float	Get_CurrCellHeight(_fvector vPosition);
	_vector	Get_SlidingVector(_fvector vPosition, _fvector vMoveDir);

public:
	virtual HRESULT Initialize_Prototype(const wstring& strDataFilePath);
	virtual HRESULT Initialize_Clone(void* pArg) override;
#ifdef _DEBUG
	virtual HRESULT Render() override;
#endif

public:
	static	CNavigation*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strDataFilePath);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};

END