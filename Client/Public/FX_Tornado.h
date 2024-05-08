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

class CFX_Tornado final : public CAssetFX
{
private:
	CFX_Tornado(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFX_Tornado(const CFX_Tornado& rhs);
	virtual ~CFX_Tornado() = default;

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

private:
	_int				m_iColliderID = { -1 };
	_float				m_fFrame = { };
	_float				m_fAttack = { 0.f };
	_float				m_fEffect = { 0.f };
	wstring				m_strParticleTag[2] = { };

	list<class CSlash_Trail*> m_TrailList;

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
	static CFX_Tornado* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void			Free() override;
};

END