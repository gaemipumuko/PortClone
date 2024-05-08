#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CDebugCheat final : public CGameObject
{
private:
	CDebugCheat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDebugCheat(const CDebugCheat& rhs);
	virtual ~CDebugCheat() = default;

private:
	_int			m_iPlayerIdx = { -1 };
	_int			m_iRotY = { 0 };
	class CPlayer*	m_pPlayer = { nullptr };

private:
	_float			m_fSize = { 0.f };
	_int			m_iTrail = 0;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CDebugCheat*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END