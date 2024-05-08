#include "stdafx.h"
#include "Hp_Bar.h"
 // #include "CUI_Manager.h"

#include "GameInstance.h"

CHp_Bar::CHp_Bar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : COrtho(pDevice, pContext)
{
}

CHp_Bar::CHp_Bar(const CHp_Bar& rhs)
    : COrtho(rhs)
{
}

HRESULT CHp_Bar::Add_Component(void* pArg)
{
    /* For.Com_Shader */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Renderer */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
        TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
        return E_FAIL;

    /* For.Com_Transform */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
        return E_FAIL;

    /* For.Com_VIBuffer */
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(CGameObject::Add_Component(m_tOrthoDesc.eTextureLevel, m_tOrthoDesc.strTextureTag,
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CHp_Bar::Set_Texture_Color(_vector vColor)
{
    if (m_eBarType == BAR_TYPE_MONSTER || m_eBarType == BAR_TYPE_BOSS)
    {
        m_vColor = XMVectorSet(1.f, 0.f, 0.f, 1.f);
    }
    else if (m_eBarType == BAR_TYPE_SHIELD)
    {
        m_vColor = XMVectorSet(0.64f, 0.97f, 1.f, 1.f);
    }
    else
    {
        m_vColor = XMVectorSet(0.f, 1.f, 0.f, 1.f);
    }

    m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(vColor));

    return S_OK;
}

HRESULT CHp_Bar::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CHp_Bar::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    COrtho::Initialize_Matrix();

    m_ReductionMat = m_pTransformCom->Get_WorldFloat4x4();

    m_HealthMat = m_pTransformCom->Get_WorldFloat4x4();

    m_ShieldReducMat = m_pTransformCom->Get_WorldFloat4x4();

    m_ShieldMat = m_pTransformCom->Get_WorldFloat4x4();
    //612,26 vs 720,49



    // CUI_Manager::Get_Instance()->Add_UI_Object(PAGE_MAIN, this);
    m_eRenderType = RTYPE_GAMEPLAY;
    return S_OK;
}

HRESULT CHp_Bar::PreviousTick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (FAILED(__super::PreviousTick(fTimeDelta)))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CHp_Bar::Tick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (FAILED(__super::Tick(fTimeDelta)))
            return E_FAIL;


        if (m_pGameInstance->Key_Down('O'))
        {
            m_fCurrentShield += 100.f;
        }
        if (m_pGameInstance->Key_Down('P'))
        {
            m_fCurrentShield -= 100.f;
        }

        _float fSizeX;
        fSizeX = m_tOrthoDesc.fSize.x * m_fCurrentHp / m_fMaxHp;


        if (m_fCurrentHp / m_fMaxHp >= 0.f)
        {
            m_vUVScale.x = (_float)(m_fCurrentHp / m_fMaxHp);

            m_HealthMat._11 = fSizeX;
            m_HealthMat._41 = m_tOrthoDesc.fPosition.x - (m_tOrthoDesc.fSize.x - fSizeX) / 2;
            m_HealthMat._42 = m_tOrthoDesc.fPosition.y;

        }
        else
        {
            m_vUVScale.x = 0.f;
            m_HealthMat._11 = 0.f;
        }

        if (m_ReductionMat._11 > m_HealthMat._11)
        {
            m_ReductionMat._11 -= fTimeDelta* 20.f;
            m_ReductionMat._41 = m_tOrthoDesc.fPosition.x - (m_tOrthoDesc.fSize.x - m_ReductionMat._11) / 2; ;
            //m_ReductionMat._41 -= fTimeDelta* 20.f /2;
            m_ReductionMat._42 = m_tOrthoDesc.fPosition.y;

            m_vReductionUVScale = { m_ReductionMat._11 / m_tOrthoDesc.fSize.x,1.f };
        }
        else
        {
            m_ReductionMat._11 = m_HealthMat._11;
            m_ReductionMat._22 = m_HealthMat._22;
            m_ReductionMat._41 = m_HealthMat._41;
            m_ReductionMat._42 = m_HealthMat._42;

            //m_vReductionUVScale.x = 1.f;
        }

        if (m_fCurrentHp / m_fMaxHp <= 0.3f && m_eBarType != BAR_TYPE_SHIELD)
        {
            m_vColor = XMVectorSet(1.f, 0.f, 0.f, 1.f);
        }



        //SHIELD
        fSizeX = m_tOrthoDesc.fSize.x * m_fCurrentShield / m_fMaxShield;


        if (m_fCurrentShield / m_fMaxShield >= 0.f)
        {
            m_vUVShieldScale.x = (_float)(m_fCurrentShield / m_fMaxShield);

            m_ShieldMat._11 = fSizeX;
            m_ShieldMat._41 = m_tOrthoDesc.fPosition.x - (m_tOrthoDesc.fSize.x - fSizeX) / 2;
            m_ShieldMat._42 = m_tOrthoDesc.fPosition.y;

        }
        else
        {
            m_vUVShieldScale.x = 0.f;
            m_ShieldMat._11 = 0.f;
        }

        if (m_ShieldReducMat._11 > m_ShieldMat._11)
        {
            m_ShieldReducMat._11 -= fTimeDelta * 200.f;
            m_ShieldReducMat._41 = m_tOrthoDesc.fPosition.x - (m_tOrthoDesc.fSize.x - m_ShieldReducMat._11) / 2; ;
            //m_ReductionMat._41 -= fTimeDelta* 20.f /2;
            m_ShieldReducMat._42 = m_tOrthoDesc.fPosition.y;

            m_vShieldReductionUVScale = { m_ShieldReducMat._11 / m_tOrthoDesc.fSize.x,1.f };
        }
        else
        {
            m_ShieldReducMat._11 = m_ShieldMat._11;
            m_ShieldReducMat._22 = m_ShieldMat._22;
            m_ShieldReducMat._41 = m_ShieldMat._41;
            m_ShieldReducMat._42 = m_ShieldMat._42;

            //m_vShieldReductionUVScale.x = 1.f;
        }

        if (m_pGameInstance->Key_Down('T'))
        {
            m_fCurrentHp -= 1000.f;
        }

        if (m_fCurrentHp / m_fMaxHp <= 0.3f && m_eBarType != BAR_TYPE_SHIELD)
        {
            m_vColor = XMVectorSet(1.f, 0.f, 0.f, 1.f);
        }

    }
    return S_OK;
}

HRESULT CHp_Bar::LateTick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (FAILED(__super::LateTick(fTimeDelta)))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CHp_Bar::Render()
{
         if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
             return E_FAIL;
    
         if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
             return E_FAIL;

         _float4x4 CombinedMatrix = {};
         XMStoreFloat4x4(&CombinedMatrix,XMMatrixIdentity());

         if (m_eBarType == BAR_TYPE_BOSS)
         {
             if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
                 return E_FAIL;
             
             _float2 vBaseScale = { 1.f,1.f };
             _float2 vBasePos = { 0.f,0.f };

             CombinedMatrix = m_pTransformCom->Get_WorldFloat4x4();

             CombinedMatrix._11 *= 1.17647058824f;
             CombinedMatrix._22 *= 1.88461538462f;

             if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
                 return E_FAIL;

             if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vBaseScale, sizeof(_float2))))
                 return E_FAIL;

             if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vBasePos, sizeof(_float2))))
                 return E_FAIL;

             if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
                 return E_FAIL;

             if (FAILED(m_pVIBufferCom->Render()))
                 return E_FAIL;

             if (m_fMaxShield > 0)
             {

                 if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
                     return E_FAIL;
                 CombinedMatrix = m_pTransformCom->Get_WorldFloat4x4();

                 CombinedMatrix._42 -= 15.f;

                 _vector vBarColor = { 0.2f,0.2f,0.2f,-0.5f };

                 if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vBarColor, sizeof(vBarColor))))
                     return E_FAIL;

                 if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
                     return E_FAIL;

                 if (FAILED(m_pShaderCom->Begin(UI_DEFINED)))
                     return E_FAIL;

                 if (FAILED(m_pVIBufferCom->Render()))
                     return E_FAIL;
             }
         }
         else if (m_eBarType == BAR_TYPE_MONSTER)
         {
             if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
                 return E_FAIL;

             _float2 vBaseScale = { 1.f,1.f };
             _float2 vBasePos = { 0.f,0.f };

             CombinedMatrix = m_pTransformCom->Get_WorldFloat4x4();

             if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
                 return E_FAIL;

             if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vBaseScale, sizeof(_float2))))
                 return E_FAIL;

             if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vBasePos, sizeof(_float2))))
                 return E_FAIL;
            
             _vector vBarColor = { 0.3f,0.3f,0.3f,1.f };
             if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vBarColor, sizeof(vBarColor))))
                 return E_FAIL;

             if (FAILED(m_pShaderCom->Begin(UI_DEFINED)))
                 return E_FAIL;

             if (FAILED(m_pVIBufferCom->Render()))
                 return E_FAIL;
         }


         if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
             return E_FAIL;

         if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
             return E_FAIL;

         if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_vReductionUVScale, sizeof(_float2))))
             return E_FAIL;

         if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &m_vUVPosition, sizeof(_float2))))
             return E_FAIL;

         _vector vReductionColor = { 208.f / 255.f,180.f / 255.f,119.f / 255.f,1.f };

         if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vReductionColor, sizeof(vReductionColor))))
             return E_FAIL;

         if (FAILED(m_pShaderCom->Begin(UI_DISCARD)))
             return E_FAIL;

         if (FAILED(m_pVIBufferCom->Render()))
             return E_FAIL;

         if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
             return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_vUVScale, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &m_vUVPosition, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(Set_Texture_Color(m_vColor)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DISCARD)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;


        //SHIELD
        if (m_eBarType == BAR_TYPE_BOSS &&m_fMaxShield>0)
        {
            if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
                return E_FAIL;

            CombinedMatrix = m_ShieldReducMat;

            CombinedMatrix._42 -= 15.f;

            if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_vShieldReductionUVScale, sizeof(_float2))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &m_vUVShieldPosition, sizeof(_float2))))
                return E_FAIL;

            vReductionColor = { 208.f / 255.f,180.f / 255.f,119.f / 255.f,1.f };

            if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vReductionColor, sizeof(vReductionColor))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(UI_DEFINED)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;

            CombinedMatrix = m_ShieldMat;

            CombinedMatrix._42 -= 15.f;

            if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_vUVShieldScale, sizeof(_float2))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &m_vUVShieldPosition, sizeof(_float2))))
                return E_FAIL;

            _vector vShieldColor = { 1.f, 0.8f,0.2f,1.f };

            if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vShieldColor, sizeof(_vector))))
                return E_FAIL;

            if (FAILED(m_pShaderCom->Begin(UI_DEFINED)))
                return E_FAIL;

            if (FAILED(m_pVIBufferCom->Render()))
                return E_FAIL;
        }


        //Render Font

        _uint iLength = 0;
        _tchar szLevel[MAX_PATH];
        if(m_iOwnerLevel != 0)
        {
            wsprintf(szLevel, TEXT("Lv.%d"), m_iOwnerLevel);

            m_pGameInstance->Render_Font(L"Font_Default", szLevel, _float2(static_cast<_float>(m_tOrthoDesc.fPosition.x - ((m_tOrthoDesc.fSize.x*1.08) / 2) + g_iWinSizeX / 2),
                static_cast<_float>(-(m_tOrthoDesc.fPosition.y+9.f - g_iWinSizeY / 2))), 0.5f, Colors::White);
        }


        iLength = static_cast<_int>(m_strOwnerDetail.size());
        if (iLength != 0)
        {
        m_pGameInstance->Render_Font(L"Font_Default", m_strOwnerDetail, _float2(static_cast<_float>(m_tOrthoDesc.fPosition.x -iLength/2 * 37 * 0.38 + g_iWinSizeX / 2),
            static_cast<_float>(-(m_tOrthoDesc.fPosition.y + 30.f - g_iWinSizeY / 2))), 0.4f, Colors::Gray, 0.f, { 0.f,0.f }, false);

        iLength = static_cast<_int>(m_strOwnerName.size());
        m_pGameInstance->Render_Font(L"Font_Default", m_strOwnerName, _float2(static_cast<_float>(m_tOrthoDesc.fPosition.x - iLength/2 * 37 * 0.7 + g_iWinSizeX / 2),
            static_cast<_float>(-(m_tOrthoDesc.fPosition.y + 52.f - g_iWinSizeY / 2))), 0.55f, Colors::White, 0.f, { 0.f,0.f }, false);
        }
        else
        {
            iLength = static_cast<_int>(m_strOwnerName.size());
            m_pGameInstance->Render_Font(L"Font_Default", m_strOwnerName, _float2(static_cast<_float>(m_tOrthoDesc.fPosition.x - iLength / 2 * 37 * 0.38 + g_iWinSizeX / 2),
                static_cast<_float>(-(m_tOrthoDesc.fPosition.y + 30.f - g_iWinSizeY / 2))), 0.4f, Colors::White, 0.f, { 0.f,0.f }, false);
        }


        // at 1.f -> 40px
    return S_OK;
}

void CHp_Bar::Update_Hp_Matrix(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
    m_tOrthoDesc.fPosition.x = fX;
    m_tOrthoDesc.fPosition.y = fY;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION,
        XMVectorSet(m_tOrthoDesc.fPosition.x, m_tOrthoDesc.fPosition.y, 0.f, 1.f));

    m_HealthMat._41 = fX;
    m_HealthMat._42 = fY;

    if (fSizeX != 0.f || fSizeY != 0.f)
    {
        m_HealthMat._11 = fSizeX;
        m_HealthMat._22 = fSizeY;

        m_tOrthoDesc.fSize.x = fSizeX;
        m_tOrthoDesc.fSize.y = fSizeY;

        _vector vScale = XMLoadFloat2(&m_tOrthoDesc.fSize); vScale.m128_f32[2] = 1.f;
        m_pTransformCom->Set_Scale(vScale);

    }

    _float fSizeHp;
    fSizeHp = m_tOrthoDesc.fSize.x * m_fCurrentHp / m_fMaxHp;

    if (m_fCurrentHp / m_fMaxHp >= 0.f)
    {
        m_vUVScale.x = (_float)(m_fCurrentHp / m_fMaxHp);
        m_pTransformCom->Set_Scale(XMVectorSet(fSizeHp, m_tOrthoDesc.fSize.y, 1.f, 1.f));
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_tOrthoDesc.fPosition.x - (m_tOrthoDesc.fSize.x - fSizeHp) / 2, m_tOrthoDesc.fPosition.y, 0.f, 1.f));
        m_HealthMat._11 = fSizeHp; m_pTransformCom->Set_Scale(XMVectorSet(fSizeHp, m_tOrthoDesc.fSize.y, 1.f, 1.f));
        m_HealthMat._41 = m_tOrthoDesc.fPosition.x - (m_tOrthoDesc.fSize.x - fSizeHp) / 2;
        m_HealthMat._42 = m_tOrthoDesc.fPosition.y;
    }



    m_ReductionMat._11 = m_HealthMat._11;
    m_ReductionMat._22 = m_HealthMat._22;
    m_ReductionMat._41 = m_HealthMat._41;
    m_ReductionMat._42 = m_HealthMat._42;

    if (m_fCurrentHp <= 0.f)
    {
        m_ReductionMat._11 = 0.f;

    }


}

CHp_Bar* CHp_Bar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CHp_Bar* pInstance = new CHp_Bar(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Create : CHp_Bar"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CHp_Bar::Clone(void* pArg)
{
    CHp_Bar* pInstance = new CHp_Bar(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CHp_Bar"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHp_Bar::Free()
{
    __super::Free();
}
