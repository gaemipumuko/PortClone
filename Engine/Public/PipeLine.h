#pragma once

#include "Base.h"

BEGIN(Engine)

class CPipeLine final : public CBase
{
public:
	enum TRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };

public:
	CPipeLine();
	virtual ~CPipeLine() = default;

private:
	_float4x4	m_ViewPortMatrix = { };

	_float4x4	m_TransformMatrix[D3DTS_END] = { };
	_float4x4	m_TransformMatrix_Inverse[D3DTS_END] = { };

	_float4		m_vCamRight = { };
	_float4		m_vCamLook = { };
	_float4		m_vCamPosition = { };

public:
	_float4x4	Get_ViewPortMatrix() { return m_ViewPortMatrix; }

	_matrix		Get_Transform(TRANSFORMSTATE eTransformState) const;
	_float4x4	Get_Transform_Float4x4(TRANSFORMSTATE eTransformState) const;

	void		Set_Transform(TRANSFORMSTATE eTransformState, _fmatrix TransformMatrix);
	void		Set_Transform(TRANSFORMSTATE eTransformState, const _float4x4& TransformMatrix);

	_matrix		Get_Transform_Inverse(TRANSFORMSTATE eTransformState) const;
	_float4x4	Get_Transform_Inverse_Float4x4(TRANSFORMSTATE eTransformState) const;

	_float4		Get_CamRight() const;
	_float4		Get_CamLook() const;
	_float4		Get_CamPosition() const;

public:
	HRESULT Initialize(_float fTopLeftX, _float TopLeftY, _float fWinCX, _float fWinCY, _float fMinDepth, _float fMaxDepth);
	HRESULT Tick(_float fTimeDelta);

public:
	static CPipeLine*	Create(_float fTopLeftX, _float TopLeftY, _float fWinCX, _float fWinCY, _float fMinDepth, _float fMaxDepth);
	virtual void		Free() override;
};

END