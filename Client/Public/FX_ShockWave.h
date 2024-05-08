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

class CFX_ShockWave final : public CAssetFX
{
public:
	enum CYLINDER { CY_CONE, CY_CYLINDER, CY_END };
	typedef struct tagShockWaveDesc : public ASFX_DESC
	{
		_float	fScale;
	} SW_DESC;

private:
	CFX_ShockWave(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFX_ShockWave(const CFX_ShockWave& rhs);
	virtual ~CFX_ShockWave() = default;

private:
	CModel*				m_pModelCom[CY_END] = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CTransform*			m_pTransformCom[CY_END] = { nullptr };

private:
	_float				m_fFrame = { };
	_float				m_fScale = { 0.5f };
	_float				m_fScaleFactor = { 1.f };

	_float4				m_vConeScale = { 2.f, 1.f, 2.f, 0.f };
	_float4				m_vThornScale = { 3.f, 6.f, 3.f, 0.f };
	_float4				m_vCylinderScale = { 3.f, 1.f, 3.f, 0.f };

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
	static CFX_ShockWave*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END