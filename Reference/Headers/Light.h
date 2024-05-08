#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight final : public CBase
{
private:
	CLight();
	virtual ~CLight() = default;

private:
	LIGHT_DESC			m_LightDesc = {};

public:
	const LIGHT_DESC*	Get_LightDesc() const { return &m_LightDesc; }
	void				Set_LightDesc(const LIGHT_DESC pDesc) { m_LightDesc = pDesc; }

	void				Set_LightPosition(const _fvector vLightPosition) { XMStoreFloat4(&m_LightDesc.vPosition, vLightPosition); }

public:
	HRESULT	Initialize(const LIGHT_DESC& LightDesc);
	HRESULT	Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

public:
	static CLight*	Create(const LIGHT_DESC& LightDesc);
	virtual void	Free();
};

END