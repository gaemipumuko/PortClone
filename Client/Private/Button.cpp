#include "stdafx.h"
#include "Button.h"
#include "GameInstance.h"

CButton::CButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : COrtho(pDevice, pContext)
{
}

CButton::CButton(const CButton& rhs)
    : COrtho(rhs)
{
}

HRESULT CButton::Add_Component(void* pArg)
{
    if (FAILED(__super::Add_Component(pArg)))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(CGameObject::Add_Component(m_tOrthoDesc.eTextureLevel, m_tOrthoDesc.strTextureTag,
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CButton::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CButton::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    COrtho::Initialize_Matrix();

    m_eRenderType = RENDER_TYPE::RTYPE_GAMEPLAY;

    return S_OK;
}

HRESULT CButton::PreviousTick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (FAILED(__super::PreviousTick(fTimeDelta)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CButton::Tick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (m_bWindowTrue == true)
        {
            if (FAILED(__super::Tick(fTimeDelta)))
                return E_FAIL;


            if (m_bMouseOver == false)
            {

                if (m_bMouseEnter == false)
                {
                    //Exit

                    m_bMouseEnter = true;
                }
            }

            if (m_bPressedDown == false && m_bMouseOver == false)
            {

                if (m_tOrthoDesc.strTextureTag == L"Prototype_Component_Texture_Gacha_Banner")
                {
                    Update_Matrix(m_vDimentionOrigin.x, m_vDimentionOrigin.y, m_vDimentionOrigin.z, m_vDimentionOrigin.w);
                }

            }

            if (m_bPressedDown == true)
            {
                Update_Matrix(m_vDimentionOrigin.x, m_vDimentionOrigin.y, m_vDimentionOrigin.z + 10.f, m_vDimentionOrigin.w + 10.f);
            }

            POINT       ptMouse;
            RECT        rcDimentions;
            int i = 0;

            GetCursorPos(&ptMouse);

            ScreenToClient(g_hWnd, &ptMouse);

            ptMouse.x = ptMouse.x - g_iWinSizeX / 2;
            ptMouse.y = (-ptMouse.y + g_iWinSizeY / 2);

            rcDimentions.left = static_cast<LONG>(m_tOrthoDesc.fPosition.x - m_tOrthoDesc.fSize.x / 2.f);
            rcDimentions.top = static_cast<LONG>(m_tOrthoDesc.fPosition.y - m_tOrthoDesc.fSize.y / 2.f);
            rcDimentions.right = static_cast<LONG>(m_tOrthoDesc.fPosition.x + m_tOrthoDesc.fSize.x / 2.f);
            rcDimentions.bottom = static_cast<LONG>(m_tOrthoDesc.fPosition.y + m_tOrthoDesc.fSize.y / 2.f);

            if (PtInRect(&rcDimentions, ptMouse))
            {
                m_bMouseOver = true;

                if (m_pGameInstance->Key_Down(VK_LBUTTON))
                {

                    if (m_tOrthoDesc.strTextureTag == L"Prototype_Component_Texture_UI_Icon_Wish")
                    {
                        m_pGameInstance->StopAll();
                        m_pGameInstance->PlayBGM(L"Gacha_Bgm.mp3", 0.3f);

                    // CUI_Manager::Get_Instance()->Close_Page(PAGE_MAIN);
                    // CUI_Manager::Get_Instance()->Show_Page(PAGE_GACHA);
                    m_bPressedDown = false;

                    }

                    m_bPressedDown = true;
                }

                if (m_bMouseEnter == true)
                {
                    //When Entered

                    if (m_tOrthoDesc.strTextureTag == L"Prototype_Component_Texture_Gacha_Banner")
                    {
                        m_pGameInstance->PlaySoundW(L"Mouse_Over.mp3", 0.5f, Engine::SOUND_CH0);

                        if (m_bPressedDown == false)
                        {
                            Update_Matrix(m_vDimentionOrigin.x, m_vDimentionOrigin.y, m_vDimentionOrigin.z + 10.f, m_vDimentionOrigin.w + 10.f);
                        }
                    }

                    m_bMouseEnter = false;
                }

            }
            else
            {
                m_bMouseOver = false;
            }

        }
    }

    return S_OK;
}

HRESULT CButton::LateTick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (m_bWindowTrue == true)
        {
            if (FAILED(__super::LateTick(fTimeDelta)))
                return E_FAIL;
            // m_pRendererCom->Add_RenderGroup(CRenderer::RG_LATEUI, this);
        }
    }
    return S_OK;
}

HRESULT CButton::Render()
{
    if (m_bWindowTrue == true)
    {
        if (FAILED(__super::Render()))
            return E_FAIL;


        //Render_Text
        //if (m_szText != NULL)
        //{
        //       m_pGameInstance->Render_Font(TEXT("Font_Default"), wstring(m_szText), _float2(m_fTextDimentions.x, m_fTextDimentions.y), 0.45f, Colors::Black);
        //}
    }
    return S_OK;
}

CButton* CButton::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CButton* pInstance = new CButton(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CButton"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CButton::Clone(void* pArg)
{
    CButton* pInstance = new CButton(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CButton"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CButton::Free()
{
    __super::Free();
}
