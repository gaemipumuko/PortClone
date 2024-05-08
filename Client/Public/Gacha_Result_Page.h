#pragma once

#include "Client_Defines.h"
#include "Ortho.h"
#include "UI_DualTex.h"

BEGIN(Client)

class CGacha_Result_Page : public COrtho
{
private:
	CGacha_Result_Page(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGacha_Result_Page(const CGacha_Result_Page& rhs);
	virtual ~CGacha_Result_Page() = default;

private:
	HRESULT Add_Component(void* pArg);
	HRESULT Bind_ShaderResources();

private:

	_bool	m_bPlayingVideo = { false };
	_float m_fDelayTime = { 0.0f };

	HWND m_hVideo;
	HDC m_DC;
	HDC m_BackDC;
	HBITMAP m_hBackBit;
	HBITMAP m_hOldBackBit;

	list<CUI_DualTex*> m_pTex_List;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize_Clone(void* pArg) override;

	virtual HRESULT PreviousTick(_float fTimeDelta) override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void ShowResults(_uint iDice);
	void RollDice(_uint iDice);

public:
	static CGacha_Result_Page*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;


};

END