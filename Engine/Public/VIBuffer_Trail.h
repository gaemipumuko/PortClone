#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Trail final : public CVIBuffer
{
private:
	CVIBuffer_Trail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CVIBuffer_Trail(const CVIBuffer_Trail& rhs);
	virtual ~CVIBuffer_Trail() = default;

private:
	_bool			m_bPlayTrail = { FALSE };
	vector<_vector>	m_VertexList;

	_float			m_fCurrFrame = { };
	_float			m_fCreateTime = { 0.005f };
	_float			m_fRemoveTime = { 0.005f };

	_uint			m_iVertexCount = { };
	_uint			m_iNumTriCount = { };

	/* CatmullRom */
	_uint			m_iEndIndex = { };
	_uint			m_iCatmullRomCount = { 4 };
	_uint			m_iCatmullRomIndex[4] = { };

	_float3			m_vLastLow = { };
	_float3			m_vLastHigh = { };

public:
	void	Set_PlayTrail(_bool bPlayTail);
	HRESULT	Append_TrailVertex(_float fTimeDelta, _fvector vLocalLow, _fvector vLocalHigh);

public:
	HRESULT Invalidate_VertexBuff(_float fTimeDelta);
	HRESULT	Invalidate_PlayVertexBuff(_float fTimeDelta);
	HRESULT	Invalidate_StopVertexBuff(_float fTimeDelta);

public:
	virtual HRESULT Initialize_Prototype(_uint iNumTriCnt);
	virtual HRESULT Initialize_Clone(void* pArg) override;
	virtual HRESULT Render();

public:
	static CVIBuffer_Trail*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumTriCnt);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};

END