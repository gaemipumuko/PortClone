#pragma once

#include "Effect.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CChainLightning final : public CEffect
{
public:
	typedef struct tagChainLightningDesc
	{
		_vector vPointA;
		_vector vPointB;
	}CHAIN_LIGHTNING_DESC;

private:
	CChainLightning(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChainLightning(const CChainLightning& rhs);
	virtual ~CChainLightning() = default;

private:
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_float			m_fLifeTime = { 0.f };
	_float			m_fFrame = { 0.f };
	_int			m_iFrameIndex = { 0 };

private:
	HRESULT	Add_Component(void* pArg);
	HRESULT Bind_ShaderResources();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CChainLightning*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END