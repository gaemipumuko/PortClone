#include "PipeLine.h"

CPipeLine::CPipeLine()
{
}

_matrix CPipeLine::Get_Transform(TRANSFORMSTATE eTransformState) const
{
	return XMLoadFloat4x4(&m_TransformMatrix[eTransformState]);
}

_float4x4 CPipeLine::Get_Transform_Float4x4(TRANSFORMSTATE eTransformState) const
{
	return m_TransformMatrix[eTransformState];
}

void CPipeLine::Set_Transform(TRANSFORMSTATE eTransformState, _fmatrix TransformMatrix)
{
	XMStoreFloat4x4(&m_TransformMatrix[eTransformState], TransformMatrix);
}

void CPipeLine::Set_Transform(TRANSFORMSTATE eTransformState, const _float4x4& TransformMatrix)
{
	m_TransformMatrix[eTransformState] = TransformMatrix;
}

_matrix CPipeLine::Get_Transform_Inverse(TRANSFORMSTATE eTransformState) const
{
	return XMLoadFloat4x4(&m_TransformMatrix_Inverse[eTransformState]);
}

_float4x4 CPipeLine::Get_Transform_Inverse_Float4x4(TRANSFORMSTATE eTransformState) const
{
	return m_TransformMatrix_Inverse[eTransformState];
}

_float4 CPipeLine::Get_CamRight() const
{
	return m_vCamRight;
}

_float4 CPipeLine::Get_CamLook() const
{
	return m_vCamLook;
}

_float4 CPipeLine::Get_CamPosition() const
{
	return m_vCamPosition;
}

HRESULT CPipeLine::Initialize(_float fTopLeftX, _float TopLeftY, _float fWinCX, _float fWinCY, _float fMinDepth, _float fMaxDepth)
{
	_matrix ScaleMatrix = XMMatrixScaling(fWinCX / 2.0f, -fWinCY / 2.0f, fMaxDepth - fMinDepth);
	_matrix TranslateMatrix = XMMatrixTranslation(fTopLeftX + fWinCX / 2.0f, TopLeftY - fWinCY / 2.0f, fMinDepth);

	XMStoreFloat4x4(&m_ViewPortMatrix, ScaleMatrix * TranslateMatrix);

	for (size_t idx = 0; idx < D3DTS_END; ++idx)
	{
		XMStoreFloat4x4(&m_TransformMatrix[idx], XMMatrixIdentity());
		XMStoreFloat4x4(&m_TransformMatrix_Inverse[idx], XMMatrixIdentity());
	}
	
	m_vCamRight = _float4(0.f, 0.f, 0.f, 1.f);
	m_vCamLook = _float4(0.f, 0.f, 0.f, 1.f);
	m_vCamPosition = _float4(0.f, 0.f, 0.f, 1.f);

	return S_OK;
}

HRESULT CPipeLine::Tick(_float fTimeDelta)
{
	for (size_t idx = 0; idx < D3DTS_END; ++idx)
	{
		XMStoreFloat4x4(&m_TransformMatrix_Inverse[idx], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrix[idx])));
	}

	memcpy(&m_vCamRight, &m_TransformMatrix_Inverse[D3DTS_VIEW].m[0][0], sizeof(_float4));
	memcpy(&m_vCamLook, &m_TransformMatrix_Inverse[D3DTS_VIEW].m[2][0], sizeof(_float4));
	memcpy(&m_vCamPosition, &m_TransformMatrix_Inverse[D3DTS_VIEW].m[3][0], sizeof(_float4));

	return S_OK;
}

CPipeLine* CPipeLine::Create(_float fTopLeftX, _float TopLeftY, _float fWinCX, _float fWinCY, _float fMinDepth, _float fMaxDepth)
{
	CPipeLine* pInstance = new CPipeLine();

	if (FAILED(pInstance->Initialize(fTopLeftX, TopLeftY, fWinCX, fWinCY, fMinDepth, fMaxDepth)))
	{
		MSG_BOX(TEXT("Failed to Create : CPipeLine"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPipeLine::Free()
{
	__super::Free();
}
