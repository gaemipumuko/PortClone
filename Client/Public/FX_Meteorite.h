#pragma once

#include "Client_Defines.h"
#include "AssetFX.h"

BEGIN(Engine)
class CModel;
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CCollider;
END

BEGIN(Client)

class CFX_Meteorite final : public CAssetFX
{
public:
	typedef struct tagMeteoriteDesc : public ASFX_DESC
	{
		_float		fDistance;
		_float		fSpeed;
		_float		fScale;
		_float4		vDirection;
		_float4		vTargetPosition;
		_float4		vMeteoriteColor;
	} METEO_DESC;

private:
	CFX_Meteorite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFX_Meteorite(const CFX_Meteorite& rhs);
	virtual ~CFX_Meteorite() = default;

private:
	CModel*				m_pModelCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };
	CCollider*			m_pColliderCom = { nullptr };

private:
	_float				m_fSpeed = {};
	_float				m_fScale = {};
	_float4				m_vFrame = { };
	_float4				m_vDirection = { };
	_float4				m_vTargetPosition = { };
	_float4				m_vMeteoriteColor = { };

	_int				m_iColliderID = { -1 };

	wstring				m_strIndicatorTag = { };

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
	static CFX_Meteorite*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

END