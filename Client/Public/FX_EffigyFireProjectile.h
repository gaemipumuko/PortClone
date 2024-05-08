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

class CFX_EffigyFireProjectile final : public CAssetFX
{
	enum FIREPROJECTILE { FIRE_CYLINDER, FIRE_SPHERE, FIRE_END };

private:
	CFX_EffigyFireProjectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFX_EffigyFireProjectile(const CFX_EffigyFireProjectile& rhs);
	virtual ~CFX_EffigyFireProjectile() = default;

private:
	CModel*				m_pModelCom[FIRE_END] = { nullptr };
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CTransform*			m_pTransformCom[FIRE_END] = { nullptr };
	CCollider*			m_pColliderCom = { nullptr };

private:
	_float4				m_vFrame = { };
	_float4				m_vPrevPosition = { };
	_float4				m_vTargetPosition = { };

	list<wstring>		m_strParticleTag;

	_int				m_iColliderID = { -1 };
	_bool				m_bCollision = { FALSE };

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
	static CFX_EffigyFireProjectile*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg);
	virtual void						Free() override;
};

END