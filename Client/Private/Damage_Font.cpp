#include "stdafx.h"
#include "Damage_Font.h"

#include "GameInstance.h"

#include "UI_Manager.h"

CDamage_Font::CDamage_Font()
{
}

CDamage_Font::CDamage_Font(const CDamage_Font& rhs)
{
}

HRESULT CDamage_Font::Initialize_Prototype(void* pArg)
{
	if (pArg != nullptr)
	{
		DMGFONT_DESC* DMG_Desc = (DMGFONT_DESC*)pArg;
		
		if (DMG_Desc->iDamage >= 0)
		m_iDamage		= DMG_Desc->iDamage;
		m_vColor		= DMG_Desc->vColor;
		m_strAilments	= DMG_Desc->strAilments;
		XMStoreFloat4(&m_vWorldPos, DMG_Desc->vWorldPos);

	}

	m_pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(m_pGameInstance);

	m_fPosRange.x = m_pGameInstance->Get_Float_Random(-50.f,50.f);
	m_fPosRange.y = m_pGameInstance->Get_Float_Random(-30.f, 50.f);

	CUI_Manager* UI_Manager = (CUI_Manager*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager"), 0);
	UI_Manager->Add_DamageFonts(this);

	return S_OK;
}

HRESULT CDamage_Font::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CDamage_Font::Tick(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc >= 2.f)
	{
		m_bDead = true;
	}

	_float2 vPosition2D;
	_vector vPos;
	_matrix ProjMatrix, ViewMatrix, WorldMatrix;
	ViewMatrix = m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW);
	ProjMatrix = m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ);
	vPos = XMLoadFloat4(&m_vWorldPos);
	vPos.m128_f32[1] += 1.5f;

	WorldMatrix = XMMatrixIdentity();

	XMStoreFloat2(&vPosition2D, XMVector3Project(vPos,
		0, 0, g_iWinSizeX, g_iWinSizeY, 0, 1, ProjMatrix, ViewMatrix, WorldMatrix));


	// 월드 좌표를 정규화된 장치 좌표로 변환
	XMMATRIX worldViewProjectionMatrix = WorldMatrix * ViewMatrix * ProjMatrix;
	XMFLOAT4 clipCoord;
	XMStoreFloat4(&clipCoord, XMVector3TransformCoord(vPos, worldViewProjectionMatrix));

	// 정규화된 장치 좌표를 화면 좌표로 변환
	float screenX = (clipCoord.x + 1.0f) * 0.5f * 1600.0f;
	float screenY = (1.0f - clipCoord.y) * 0.5f * 900.0f;

	m_vScreenPos = { screenX, screenY };

	return S_OK;
}

HRESULT CDamage_Font::LateTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CDamage_Font::Render()
{

		//Render Font
	_tchar szDamage[MAX_PATH];
	_float fFontSize = 0.5f;
	_float fFontZoomRate = 1.f;
	_float fFontBrightness = 0.f;
	_float fFontSpeed = 8.f;
	_float2 fPos = {0.f,0.f};
	_vector vColor;
	vColor = m_vColor;
	fPos = m_vScreenPos;

	if (m_fTimeAcc >= 1.5f)
	{
		fFontBrightness = m_fTimeAcc-1.5f;
	}

	if (m_fTimeAcc < .3f)
	{
		fFontZoomRate = 1.6f - m_fTimeAcc*2.f;
	}

	vColor.m128_f32[0] -= fFontBrightness;
	vColor.m128_f32[1] -= fFontBrightness;
	vColor.m128_f32[2] -= fFontBrightness;
	vColor.m128_f32[3] -= fFontBrightness;

	fPos.x += m_fPosRange.x;
	fPos.y += m_fPosRange.y - (m_fTimeAcc * fFontSpeed);

	if (m_iDamage >= 1 && m_iDamage < 100)
	{
		fFontSize = 0.45f;
	}
	if (m_iDamage >= 100 && m_iDamage < 1000)
	{
		//fFontSize = 0.55f;
		fFontSize = 0.6f;
	}
	if (m_iDamage >= 1000 && m_iDamage < 100000)
	{
		//fFontSize = 0.6f;
		fFontSize = 0.8f;

	}
	fFontSize *= fFontZoomRate;

	wsprintf(szDamage, TEXT("%d"), m_iDamage);

	if (m_iDamage != 0)
	{
		_float fTemp = 1.f;
		_vector vOutlineColor = { vColor.m128_f32[0] * 0.5f,  vColor.m128_f32[1] * 0.5f, vColor.m128_f32[2] * 0.5f ,  vColor.m128_f32[3]};
		_float2	vOutlinePos = { fPos.x - fTemp, fPos.y - fTemp };
		m_pGameInstance->Render_Font(L"Font_Number", szDamage, vOutlinePos, fFontSize, vOutlineColor, 0.f, { 0.f,0.f }, false);

		vOutlinePos = { fPos.x - fTemp, fPos.y };
		m_pGameInstance->Render_Font(L"Font_Number", szDamage, vOutlinePos, fFontSize, vOutlineColor, 0.f, { 0.f,0.f }, false);

		vOutlinePos = { fPos.x - fTemp, fPos.y + fTemp };
		m_pGameInstance->Render_Font(L"Font_Number", szDamage, vOutlinePos, fFontSize, vOutlineColor, 0.f, { 0.f,0.f }, false);

		vOutlinePos = { fPos.x, fPos.y + fTemp };
		m_pGameInstance->Render_Font(L"Font_Number", szDamage, vOutlinePos, fFontSize, vOutlineColor, 0.f, { 0.f,0.f }, false);

		vOutlinePos = { fPos.x, fPos.y - fTemp };
		m_pGameInstance->Render_Font(L"Font_Number", szDamage, vOutlinePos, fFontSize, vOutlineColor, 0.f, { 0.f,0.f }, false);

		vOutlinePos = { fPos.x + fTemp, fPos.y - fTemp };
		m_pGameInstance->Render_Font(L"Font_Number", szDamage, vOutlinePos, fFontSize, vOutlineColor, 0.f, { 0.f,0.f }, false);

		vOutlinePos = { fPos.x + fTemp, fPos.y };
		m_pGameInstance->Render_Font(L"Font_Number", szDamage, vOutlinePos, fFontSize, vOutlineColor, 0.f, { 0.f,0.f }, false);

		vOutlinePos = { fPos.x + fTemp, fPos.y  + fTemp };
		m_pGameInstance->Render_Font(L"Font_Number", szDamage, vOutlinePos, fFontSize, vOutlineColor, 0.f, { 0.f,0.f }, false);

		m_pGameInstance->Render_Font(L"Font_Number", szDamage, fPos, fFontSize, vColor, 0.f, { 0.f,0.f }, false);

	}
	else
	{
		_float fTemp = 1.f;
		_vector vOutlineColor = { vColor.m128_f32[0] * 0.5f,  vColor.m128_f32[1] * 0.5f, vColor.m128_f32[2] * 0.5f ,  vColor.m128_f32[3] };
		_float2	vOutlinePos = { fPos.x - fTemp, fPos.y - fTemp };
		m_pGameInstance->Render_Font(L"Font_Default", m_strAilments, vOutlinePos, 0.7f, vOutlineColor, 0.f, { 0.f,0.f }, false);

		vOutlinePos = { fPos.x - fTemp, fPos.y };
		m_pGameInstance->Render_Font(L"Font_Default", m_strAilments, vOutlinePos, 0.7f, vOutlineColor, 0.f, { 0.f,0.f }, false);

		vOutlinePos = { fPos.x - fTemp, fPos.y + fTemp };
		m_pGameInstance->Render_Font(L"Font_Default", m_strAilments, vOutlinePos, 0.7f, vOutlineColor, 0.f, { 0.f,0.f }, false);

		vOutlinePos = { fPos.x, fPos.y + fTemp };
		m_pGameInstance->Render_Font(L"Font_Default", m_strAilments, vOutlinePos, 0.7f, vOutlineColor, 0.f, { 0.f,0.f }, false);

		vOutlinePos = { fPos.x, fPos.y - fTemp };
		m_pGameInstance->Render_Font(L"Font_Default", m_strAilments, vOutlinePos, 0.7f, vOutlineColor, 0.f, { 0.f,0.f }, false);

		vOutlinePos = { fPos.x + fTemp, fPos.y - fTemp };
		m_pGameInstance->Render_Font(L"Font_Default", m_strAilments, vOutlinePos, 0.7f, vOutlineColor, 0.f, { 0.f,0.f }, false);

		vOutlinePos = { fPos.x + fTemp, fPos.y };
		m_pGameInstance->Render_Font(L"Font_Default", m_strAilments, vOutlinePos, 0.7f, vOutlineColor, 0.f, { 0.f,0.f }, false);

		vOutlinePos = { fPos.x + fTemp, fPos.y + fTemp };
		m_pGameInstance->Render_Font(L"Font_Default", m_strAilments, vOutlinePos, 0.7f, vOutlineColor, 0.f, { 0.f,0.f }, false);

		m_pGameInstance->Render_Font(L"Font_Default", m_strAilments, fPos, 0.7f, vColor, 0.f, { 0.f,0.f }, false);


		//m_pGameInstance->Render_Font(L"Font_Default", m_strAilments, fPos, 0.7f, m_vColor, 0.f, { 0.f,0.f }, false);
	}



	return S_OK;
}

CDamage_Font* CDamage_Font::Create(void* pArg)
{
	CDamage_Font* pInstance = new CDamage_Font();

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSG_BOX(TEXT("Failed to Create : CDamage_Font"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDamage_Font::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
