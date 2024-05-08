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

class CFX_BarbaraES final : public CAssetFX
{
	/* 물방울 / 물방울 + 큰물결 / 큰물결 / 큰물결 + 작은물결 / 작은물결 / 작은물결 + 음표 / 음표 */
	enum STEP	   { STEP_1, STEP_2, STEP_3, STEP_4, STEP_5, STEP_6, STEP_7, STEP_END };
	enum SHADER	   { SHADER_MESH, SHADER_ANIM, SHADER_END };
	enum BARBARAES { BES_ES1, BES_ES2, BES_ES3, BES_ES4, BES_END };

private:
	CFX_BarbaraES(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFX_BarbaraES(const CFX_BarbaraES& rhs);
	virtual ~CFX_BarbaraES() = default;

private:
	CModel*				m_pModelCom[BES_END] = {nullptr};
	CShader*			m_pShaderCom[SHADER_END] = {nullptr};
	CTexture*			m_pTextureCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CTransform*			m_pTransformCom[BES_END] = { nullptr };

private:
	STEP				m_eStep = { STEP_END };
	_float4				m_vFrame = { };
	_float				m_fScale = { 0.5f };
	_float				m_fAnimSpeed = { 1.f };

	_float				m_fTickHeal = { 0.f };

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
	static CFX_BarbaraES*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END