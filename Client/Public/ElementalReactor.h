#pragma once

#include "Client_Defines.h"
#include "Part_Body.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CElementalReactor final : public CPart_Body
{
public:
	typedef struct tagReactorDesc
	{
		ELEMENT	eReactElement		= { ELEMENT_END };
		LEVELID	eReactorLevel		= { LEVEL_END };

		_float4	vReactorPosition	= { };	// 반응 리액터
	} REACTOR_DESC;

private:
	CElementalReactor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CElementalReactor(const CElementalReactor& rhs);
	virtual ~CElementalReactor() = default;

private:
	CTexture*		m_pTextureCom = { nullptr };

	CModel*			m_pActiveFXModelCom = { nullptr };
	CShader*		m_pActiveFXShaderCom = { nullptr };
	CTexture*		m_pActiveFXTextureCom = { nullptr };

private:
	_byte			m_byStep   = { 0 };
	_bool			m_bReacted = { FALSE };
	ELEMENT			m_eElement = { ELEMENT_END };

	_bool			m_bActiveFX = { FALSE };
	_float4			m_fActiveFXFrame = { };

	_byte			m_byRenderStep = { 0 };

public:
	_bool			Get_Reacted() { return m_bReacted; }

private:
	virtual HRESULT	Invalidate_Animation(_float fTimeDelta);

private:
	HRESULT			Add_Component(void* pArg);
	HRESULT			Bind_ShaderResources();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CElementalReactor*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END