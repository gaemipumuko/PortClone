#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CPage : public CGameObject
{
public:
	enum UI_PAGE {PAGE_PAIMON,PAGE_CHARACTER,PAGE_TEAMCONFIG,PAGE_INVEN,PAGE_END};

protected:
	CPage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPage(const CPage& rhs);
	virtual ~CPage() = default;

protected:
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };

protected:
	ORTHO_DESC			m_tOrthoDesc = { };
	_float4x4			m_ViewMatrix = { }, m_ProjMatrix = { };
	
protected:
	_bool				m_bWindowTrue = false;
	_bool				m_bWindowIconTrue = false;
	_bool				m_bWindowIconTrue_1 = false;

protected:
	HRESULT Add_Component(void* pArg);
	HRESULT Bind_ShaderResources();

	HRESULT Initialize_Matrix();
	HRESULT Update_Matrix(_float fX, _float fY, _float fSizeX = 0.f, _float fSizeY = 0.f);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

END