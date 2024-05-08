#pragma once

#include "Client_Defines.h"
#include "Ortho.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CUI_DualTex final : public COrtho
{
public:
	typedef struct tagDualTexDesc : public ORTHO_DESC
	{
		wstring		strDiffTextureTag;

	} DUALTEX_DESC;

private:
	CUI_DualTex(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_DualTex(const CUI_DualTex& rhs);
	virtual ~CUI_DualTex() = default;


private:
	_vector	m_vRadianceColor = {};

	CTexture* m_pDiffTextureCom = { nullptr };

private:
	wstring strDiffTexTag = {};

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

	virtual void	SetTextureIndex(_uint iTextureIndex) override;
	virtual void	SetRadianceColor(_vector vColor) { m_vRadianceColor = vColor; }

public:
	static CUI_DualTex*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END