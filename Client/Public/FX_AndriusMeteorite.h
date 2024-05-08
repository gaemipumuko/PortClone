#pragma once

#include "Client_Defines.h"
#include "AssetFX.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CFX_AndriusMeteorite final : public CAssetFX
{
private:
	CFX_AndriusMeteorite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFX_AndriusMeteorite(const CFX_AndriusMeteorite& rhs);
	virtual ~CFX_AndriusMeteorite() = default;

private:
	CTransform*			m_pTransformCom = { nullptr };

private:
	_float				m_fFrame = { };

	_int				m_iMeteoriteCount = { 10 };
	_float				m_fMeteoriteDelay = { 4.f };

private:
	HRESULT Add_Component(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CFX_AndriusMeteorite*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free() override;
};

END