#pragma once

#include "Client_Defines.h"
#include "Ortho.h"

BEGIN(Client)

class CPaimon_Page : public COrtho
{
private:
	CPaimon_Page(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPaimon_Page(const CPaimon_Page& rhs);
	virtual ~CPaimon_Page() = default;

private:
	_int				m_iMOverIndex = { -1 };

	vector<_float4x4>	m_PaimonBaseMat;
	vector<_float4x4>	m_NameCardMat;
	vector<_float4x4>	m_PaimonIconsMat;

	CTexture* m_pIconTextureCom;
	CTexture* m_pMainIconTextureCom;
	CTexture* m_pPortraitTextureCom;
	CTexture* m_pNameCardTextureCom;

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
	static CPaimon_Page*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

END