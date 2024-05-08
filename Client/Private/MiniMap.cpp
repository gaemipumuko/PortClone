#include "stdafx.h"
#include "MiniMap.h"

#include "GameInstance.h"

#include "InteractionBox.h"
#include "AlphaRect.h"
#include "Player.h"

CMini_Map::CMini_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : COrtho(pDevice, pContext)
{
}

CMini_Map::CMini_Map(const CMini_Map& rhs)
    : COrtho(rhs)
{
}

HRESULT CMini_Map::Add_Component(void* pArg)
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
    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Mini_Map_Boundary"),
        TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Interaction_Icons"),
        TEXT("Com_Texture_Icon"), (CComponent**)&m_pMapIconCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMini_Map::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_vUVScale, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &m_vUVPosition, sizeof(_float2))))
        return E_FAIL;

    return S_OK;
}

void CMini_Map::Register_OnMap(CTransform* TargetTransform, MAP_ICON eIcon)
{
    m_vecTargetObject.push_back(TargetTransform);
    m_vecTargetIcon.push_back(eIcon);

}

HRESULT CMini_Map::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMini_Map::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

    COrtho::Initialize_Matrix();


    _float4x4 OnMapMat = { };
    XMStoreFloat4x4(&OnMapMat, XMMatrixIdentity());


    //Location on Map
    OnMapMat._11 = 180.f; OnMapMat._22 = 180.f;
    OnMapMat._41 = static_cast<_float>(145.f - g_iWinSizeX / 2);
    OnMapMat._42 = static_cast<_float>(-(115.f - g_iWinSizeY / 2));
    m_vecBoundaryMat = OnMapMat;

    OnMapMat._11 = 20.f; OnMapMat._22 = 20.f;
    OnMapMat._41 = 0.f;
    OnMapMat._42 = 0.f;
    //OnMapMat._41 = static_cast<_float>(145.f - g_iWinSizeX / 2);
    //OnMapMat._42 = static_cast<_float>(-(115.f - g_iWinSizeY / 2));
    m_vPlayerIconMat = OnMapMat;

    OnMapMat._11 = 64.f; OnMapMat._22 = 128.f;
    OnMapMat._41 = 0.f;
    OnMapMat._42 = 0.f;
    //OnMapMat._41 = static_cast<_float>(145.f - g_iWinSizeX / 2);
    //OnMapMat._42 = static_cast<_float>(-(115.f - g_iWinSizeY / 2));
    m_vCamMat = OnMapMat;

    m_eRenderType = RTYPE_GAMEPLAY;

    return S_OK;
}

HRESULT CMini_Map::PreviousTick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (FAILED(__super::PreviousTick(fTimeDelta)))
            return E_FAIL;
    }
    return S_OK;
}

HRESULT CMini_Map::Tick(_float fTimeDelta)
{
    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {
        if (FAILED(__super::Tick(fTimeDelta)))
            return E_FAIL;

        if (LEVEL_LOADING == m_pGameInstance->Get_CurrentLevel())
            return S_OK;

        //플레이어 look 이용해 회전 적용
        CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
        _int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

        m_pPlayerTransform = static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);
        m_vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

        _vector vPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
        _vector vLook = m_pPlayerTransform->Get_State(CTransform::STATE_LOOK);
        vLook.m128_f32[1] = 0.f;
        XMVector3Normalize(vLook);

        _float fRotation = atan2(vLook.m128_f32[2], vLook.m128_f32[0]) + XMConvertToRadians(90.f);
        _matrix RotMat = XMMatrixRotationZ(fRotation);
        _matrix PlayerMat = XMLoadFloat4x4(&m_vPlayerIconMat);
        XMStoreFloat4x4(&m_vPlayerOnMapMat, PlayerMat * RotMat);

        m_vPlayerOnMapMat._41 = static_cast<_float>(145.f - g_iWinSizeX / 2);
        m_vPlayerOnMapMat._42 = static_cast<_float>(-(115.f - g_iWinSizeY / 2));

        //카메라 Look 이용해 회전 적용
        

        _vector vCamLook = m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_VIEW).r[2];
        vCamLook.m128_f32[1] = 0.f; 
        vCamLook = XMVector4Normalize(vCamLook);

        fRotation = atan2(vCamLook.m128_f32[2], vCamLook.m128_f32[0]) + XMConvertToRadians(90.f);
        RotMat = XMMatrixRotationZ(fRotation);
        _matrix CamMat = XMLoadFloat4x4(&m_vCamMat);
        XMStoreFloat4x4(&m_vCamOnMapMat, CamMat * RotMat);

        m_vCamOnMapMat._41 = static_cast<_float>(145.f - g_iWinSizeX / 2);
        m_vCamOnMapMat._42 = static_cast<_float>(-(115.f - g_iWinSizeY / 2));

        // 월드 변환 행렬을 만들어 Z축 주위의 회전을 적용
        //XMMATRIX worldMatrix = XMMatrixRotationZ(rotationAngle);

       // Get Camera Look
       // m_pGameInstance->Get_Transform



    }
    return S_OK;
}

HRESULT CMini_Map::LateTick(_float fTimeDelta)
{

    if (CDeciderObject::Verify_TickValidity() == S_OK)
    {

        if (FAILED(__super::LateTick(fTimeDelta)))
            return E_FAIL;



    }
    return S_OK;
}

HRESULT CMini_Map::Render()
{

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _float  fSlide = 20.f;
    _float4x4 CombinedMatrix = {};
    _float2 vUVPos = { 0.f,0.f };
    _float2 vUVScale = { 1.f,1.f };

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    CombinedMatrix = m_vecBoundaryMat;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_vUVScale, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &m_vUVPosition, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;


    //Render Player Icon
    if (FAILED(m_pMapIconCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    CombinedMatrix = m_vPlayerOnMapMat;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_vPlayerOnMapMat)))
        return E_FAIL;

    vUVPos = _float2(815.f / 1024.f, 346.f / 1024.f);
    vUVScale = _float2(35.f / 1024.f, 35.f / 1024.f);

    if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    //Render CamDir

    if (FAILED(m_pMapIconCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    CombinedMatrix = m_vCamOnMapMat;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_vCamOnMapMat)))
        return E_FAIL;

    vUVPos = _float2(868.f / 1024.f, 11.f / 1024.f);
    vUVScale = _float2(136.f / 1024.f, 200.f / 1024.f);

    if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;


    //Render Targets On Map;
    for (_int iIndex = 0; iIndex < m_vecTargetObject.size(); iIndex++)
    {
        _vector vTargetPos = m_vecTargetObject[iIndex]->Get_State(CTransform::STATE_POSITION);
        vTargetPos -= m_vPlayerPos;
        vTargetPos *= 8.f;
        

        if (FAILED(m_pMapIconCom->Bind_ShaderResource(m_pShaderCom, "g_Texture",0)))
            return E_FAIL;

        XMStoreFloat4x4(&CombinedMatrix, XMMatrixIdentity());
        CombinedMatrix._11 = m_fIconSize.x;
        CombinedMatrix._22 = m_fIconSize.y;
        CombinedMatrix._41 = static_cast<_float>(-vTargetPos.m128_f32[0] + 145.f - g_iWinSizeX / 2);
        CombinedMatrix._42 = static_cast<_float>(-(vTargetPos.m128_f32[2] + 115.f - g_iWinSizeY / 2));
        if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
            return E_FAIL;

        switch (m_vecTargetIcon[iIndex])
        {
        case MAP_MONSTER:
            vUVPos = _float2(854.f / 1024.f, 244.f / 1024.f);
            vUVScale = _float2(30.f / 1024.f, 30.f / 1024.f);
            break;
        case MAP_NPC:
            vUVPos = _float2(4.f / 1024.f, 579.f / 1024.f);
            vUVScale = _float2(70.f / 1024.f, 70.f / 1024.f);
            break;
        case MAP_QUEST:
             vUVPos = _float2(4.f / 1024.f, 579.f / 1024.f);
             vUVScale = _float2(70.f / 1024.f, 70.f / 1024.f);
            break;
        case MAP_SHOP:
            vUVPos = _float2{ 363.f / 1024.f,53.f / 1024.f };
            vUVScale = _float2{ 42.f / 1024.f,48.f / 1024.f };
        case MAP_ALCHEMY:
            vUVPos = _float2{ 194.f / 1024.f,271.f / 1024.f };
            vUVScale = _float2{ 52.f / 1024.f,48.f / 1024.f };
            break;
        case MAP_COOKING:
            vUVPos = _float2{ 529.f / 1024.f,118.f / 1024.f };
            vUVScale = _float2{ 50.f / 1024.f,45.f / 1024.f };
            break;
        default:
            break;
        }


        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &vUVScale, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vUVPos, sizeof(_float2))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
            return E_FAIL;

        if (FAILED(m_pVIBufferCom->Render()))
            return E_FAIL;

    }

        m_vecTargetObject.clear();
        m_vecTargetIcon.clear();

    return S_OK;
}

CMini_Map* CMini_Map::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMini_Map* pInstance = new CMini_Map(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Clone(nullptr)))
    {
        MSG_BOX(TEXT("Failed to Create : CMini_Map"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMini_Map::Clone(void* pArg)
{
    CMini_Map* pInstance = new CMini_Map(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Clone : CMini_Map"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMini_Map::Free()
{
    __super::Free();

    Safe_Release(m_pMapIconCom);
    Safe_Release(m_pPlayerTransform);

}
