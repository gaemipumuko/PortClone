#pragma once

#include "Client_Defines.h"
#include "AssetFX.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CFX_EffigyFire_MagicCircle final : public CAssetFX
{
private:
	CFX_EffigyFire_MagicCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFX_EffigyFire_MagicCircle(const CFX_EffigyFire_MagicCircle& rhs);
	virtual ~CFX_EffigyFire_MagicCircle() = default;

private:
	_float m_fTimeAcc = { 0.f };

	_float4x4 m_WorldMatrix = {};
	_float3 m_vLook = {};

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

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
	static CFX_EffigyFire_MagicCircle*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg);
	virtual void						Free() override;
};

END