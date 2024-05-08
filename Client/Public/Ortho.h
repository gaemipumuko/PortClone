#pragma once

#include "Client_Defines.h"
#include "DeciderObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class COrtho abstract : public CDeciderObject
{
public:
	typedef struct tagOrthoDesc
	{
		LEVELID		eTextureLevel;
		wstring		strTextureTag;
		_float2		fPosition, fSize;
		UI_PAGE		ePage = {PAGE_END};
		_uint		iDepth = { 0 };
	} ORTHO_DESC;

protected:
	COrtho(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COrtho(const COrtho& rhs);
	virtual ~COrtho() = default;

protected:
	CShader*			m_pShaderCom = { nullptr };
	CTexture*			m_pTextureCom = { nullptr };
	CRenderer*			m_pRendererCom = { nullptr };
	CTransform*			m_pTransformCom = { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom = { nullptr };

protected:
	ORTHO_DESC			m_tOrthoDesc = { };
	_float4x4			m_ViewMatrix = { }, m_ProjMatrix = { };
	_float4				m_vDimentionOrigin = {};
	_float2				m_vUVScale = { 1.f, 1.f };
	_float2				m_vUVPosition = { 0.f, 0.f };
protected:
	_bool						m_bCharMeneBlurRect = false;

		/* 메뉴 별 UI랜더 스위치  */
	_bool						m_bWindowTrue = false;

protected:
	HRESULT Add_Component(void* pArg);
	HRESULT Bind_ShaderResources();

	HRESULT Initialize_Matrix();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	
	virtual void	SetTextureIndex(_uint iTextureIndex);
	//Button Effects
public:

	HRESULT Update_Matrix(_float fX, _float fY, _float fSizeX = 0.f, _float fSizeY = 0.f);
	HRESULT Update_Matrix(_vector vWorldPos, _float fSizeX = 0.f, _float fSizeY = 0.f);
	_bool In_From_Right(_float fDestX,_float fSpeed);
	_bool In_From_Below(_float fDestY, _float fSpeed);

	_float2 Get_WindowPos() { return m_tOrthoDesc.fPosition; }

	//Get & Set
public:
	void	SetTextureUV(_float2 vPos = { 0.f,0.f }, _float2 vScale = {1.f,1.f}) { m_vUVPosition = vPos; m_vUVScale = vScale; }
	void	SetWindowSwitch(_bool bWindow) { m_bWindowTrue = bWindow;}

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

END