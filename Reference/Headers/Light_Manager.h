#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

private: /* Normal Lights */
	_uint					m_iNumLevel = { 0 };
	list<class CLight*>*	m_pLights = { nullptr };

public:
	HRESULT				Add_Light(_uint iLevelIndex, const LIGHT_DESC& LightDesc);
	const LIGHT_DESC*	Find_LightDesc(_uint iLevelIndex, _uint iIndex) const;

	HRESULT				Update_LightPosition(_uint iLevelIndex, _uint iIndex, const _fvector& vLightPosition);

public:
	HRESULT Initialize(_uint iNumLevel);
	HRESULT Render(_uint iLevelIndex, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

public:
	static CLight_Manager*		Create(_uint iNumLevel);
	void						Clear(_uint iLevel);
	virtual void				Free() override;
};

END