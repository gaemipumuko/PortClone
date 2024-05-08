#pragma once

#include "Client_Defines.h"
#include "Ortho.h"
#include "Gacha_Result_Page.h"
#include "Button.h"
#include "AlphaRect.h"
#include "SelRect.h"

BEGIN(Client)

class CGacha_Page : public COrtho
{
private:
	CGacha_Page(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGacha_Page(const CGacha_Page& rhs);
	virtual ~CGacha_Page() = default;

private:
	_bool				m_bShowSelectBox = { false };

	_int				m_iMOverIndex = { -1 };	/* -1은 아무것도 오버되지 않은 상태 */
	_int				m_iSelectedIndex = { 0 };	/* -1은 아무것도 오버되지 않은 상태 */

	_int				m_iNumGems = {};
	_uint				m_iTabIndex = {};

	vector<_float4x4>	m_GachaMat;
	vector<_float4x4>	m_BannerMat;

	CTexture*			m_pBannerTexture = {};

	CGacha_Result_Page* m_pResultPg = { nullptr };
	
	CSelRect*			m_pSelect_Box = { nullptr };
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
	static CGacha_Page*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;

};

END