#pragma once

#include "Client_Defines.h"
#include "AssetFX.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Trail;
END

BEGIN(Client)

class CFX_EffigyFireTrail final : public CAssetFX
{
private:
	CFX_EffigyFireTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFX_EffigyFireTrail(const CFX_EffigyFireTrail& rhs);
	virtual ~CFX_EffigyFireTrail() = default;

private:
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };
	CVIBuffer_Trail*	m_pVIBufferTrailCom = { nullptr };

private:
	_float				m_fFrame = { };

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
	static CFX_EffigyFireTrail*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free() override;
};

END