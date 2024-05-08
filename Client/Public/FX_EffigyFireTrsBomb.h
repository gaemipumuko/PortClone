#pragma once

#include "Client_Defines.h"
#include "AssetFX.h"

BEGIN(Client)

class CFX_EffigyFireTrsBomb final : public CAssetFX
{
public:
	typedef struct tagTrsBombDesc : public ASFX_DESC
	{
		const _float4x4*	pBoneMatrix = { };
	} TRSBOMB_DESC;

private:
	CFX_EffigyFireTrsBomb(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFX_EffigyFireTrsBomb(const CFX_EffigyFireTrsBomb& rhs);
	virtual ~CFX_EffigyFireTrsBomb() = default;

private:
	CTransform*			m_pTransformCom = { nullptr };
	const _float4x4*	m_pBoneMatrix = { };

private:
	_float4				m_vFrame = { };
	_bool				m_bTrsBombEff = { FALSE };

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
	static CFX_EffigyFireTrsBomb*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free() override;
};

END