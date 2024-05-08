#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	CComponent(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

protected:
	class CGameInstance*	m_pGameInstance = { nullptr };

protected:
	_bool	m_bCloned = { FALSE };

public:
	_bool	Get_Cloned() { return m_bCloned; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual HRESULT	Render() { return S_OK; }

public:
	virtual CComponent*		Clone(void* pArg) PURE;
	virtual void			Free() override;
};

END