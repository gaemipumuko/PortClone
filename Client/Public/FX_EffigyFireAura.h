#pragma once

#include "Client_Defines.h"
#include "AssetFX.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CFX_EffigyFireAura final : public CAssetFX
{
	enum AURA { AURA_0, AURA_45, AURA_90, AURA_135, AURA_END };

private:
	CFX_EffigyFireAura(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFX_EffigyFireAura(const CFX_EffigyFireAura& rhs);
	virtual ~CFX_EffigyFireAura() = default;

private:
	CModel*				m_pModelCom[AURA_END] = {nullptr};
	CShader*			m_pShaderCom = {nullptr};
	CTexture*			m_pTextureCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };
	CTransform*			m_pTargetTransformCom = { nullptr };

private:
	_float4				m_vFrame = { };

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
	static CFX_EffigyFireAura*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END