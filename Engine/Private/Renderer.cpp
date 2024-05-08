#include "Renderer.h"

#include "Particle.h"
#include "GameObject.h"
#include "RenderTarget.h"

#include "Camera.h"
#include "GameInstance.h"

#define NUM_LIGHTDEPTH 3

#define LIGHTDEPTH_X 3600
#define LIGHTDEPTH_Y 3600

#define WINCX 1600
#define WINCY 900

#define RENDER_MACRO(RENDER_PASS)                               \
    for (auto& iter : m_RenderObjects[RENDER_PASS])             \
    {                                                           \
        if (iter != nullptr)                                    \
        {                                                       \
            if (FAILED(iter->Render()))                         \
            {                                                   \
                MSG_BOX(TEXT("Failed to Render : CRenderer"));  \
            }                                                   \
        }                                                       \
                                                                \
        Safe_Release(iter);                                     \
    }                                                           \
                                                                \
    m_RenderObjects[RENDER_PASS].clear();                       \

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

HRESULT CRenderer::Ready_ShadowDepthStencilView(_uint iSizeX, _uint iSizeY)
{
	if (m_pDevice == nullptr)
		return E_FAIL;

	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iSizeX;
	TextureDesc.Height = iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = NUM_LIGHTDEPTH;
	TextureDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr,
		&pDepthStencilTexture)))
		return E_FAIL;

	D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc = { };

	DSVDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	DSVDesc.Texture2DArray.MipSlice = 0;
	DSVDesc.Texture2DArray.FirstArraySlice = 0;
	DSVDesc.Texture2DArray.ArraySize = NUM_LIGHTDEPTH;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, &DSVDesc, &m_pShadowDSV)))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = { };
	SRVDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	SRVDesc.Texture2DArray.MostDetailedMip = 0;
	SRVDesc.Texture2DArray.MipLevels = 1;
	SRVDesc.Texture2DArray.FirstArraySlice = 0;
	SRVDesc.Texture2DArray.ArraySize = TextureDesc.ArraySize;

	if (FAILED(m_pDevice->CreateShaderResourceView(pDepthStencilTexture, &SRVDesc, &m_pShadowSRV)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}

void CRenderer::Extract_FrustumBoundSphere(_float fNear, _float fFar, _fvector& vCenter, _float& fBoundRadius)
{
	_matrix CameraMatrix = m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_VIEW);
}

void CRenderer::Set_ViewPort(_uint iSizeX, _uint iSizeY)
{
	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)iSizeX;
	ViewPortDesc.Height = (_float)iSizeY;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pGameObject)
{
	if (pGameObject == nullptr)
		return E_FAIL;

	if (eRenderGroup >= RG_END)
		return E_FAIL;

	m_RenderObjects[eRenderGroup].push_back(pGameObject);
	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CRenderer::Add_RenderParticle(CParticle* pParticle)
{
	if (pParticle == nullptr)
		return E_FAIL;

	m_RenderParticles.push_back(pParticle);
	Safe_AddRef(pParticle);

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Add_DebugComponent(CComponent* pDebugComponent)
{
	if (pDebugComponent == nullptr)
		return E_FAIL;

	if (m_bRenderDebugCom == TRUE)
	{
		m_DebugComponent.push_back(pDebugComponent);
		Safe_AddRef(pDebugComponent);
	}

	return S_OK;
}
#endif

HRESULT CRenderer::Draw_RenderGroup()
{
	/* Invalidate Cam Far */
	CCamera* pCamera = static_cast<CCamera*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
	if (pCamera != nullptr)
	{
		_float4 vFovArNearFar = pCamera->Get_FovArNearFar();

		m_fFovy = vFovArNearFar.x;
		m_fAspect = vFovArNearFar.y;
		m_fCamNear = vFovArNearFar.z;
		m_fCamFar = vFovArNearFar.w;
	}
	else
	{
		m_fFovy =  60.f;
		m_fAspect = 16.f / 9.f;
		m_fCamNear = 0.1f;
		m_fCamFar = 200.f;
	}

	if (FAILED(m_pShader->Bind_RawValue("g_fFar", &m_fCamFar, sizeof(_float))))
		return E_FAIL;

	/* Draw RenderGroup */
	if (FAILED(Render_Priority()))
		return E_FAIL;

	if (FAILED(Render_Shadow()))
		return E_FAIL;

	if (FAILED(Render_Terrain()))
		return E_FAIL;

	if (FAILED(Render_NonBlend()))
		return E_FAIL;

	if (FAILED(Render_MTrail()))
		return E_FAIL;

	if (FAILED(Render_LightAcc()))
		return E_FAIL;

	if (FAILED(Render_Blend()))
		return E_FAIL;

	if (FAILED(Render_Trail()))
		return E_FAIL;

	if (FAILED(Render_Particle()))
		return E_FAIL;

	if (FAILED(Render_SceneDeferred()))
		return E_FAIL;

	if (FAILED(Render_EffectDeferred()))
		return E_FAIL;

	if (FAILED(Render_PostEffect()))
		return E_FAIL;

	if (FAILED(Render_UI()))
		return E_FAIL;

	if (FAILED(Render_Font()))
		return E_FAIL;

	if (FAILED(Render_Mouse()))
		return E_FAIL;

#ifdef _DEBUG
	// 주석 금지 주석 금지 주석 금지 주석 금지 주석 금지 주석 금지 주석 금지 주석 금지 주석 금지
	// 헤더에서 m_bRenderDebugCom 를 FALSE로 "해줘"
	if (FAILED(Render_Debug()))
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	/* Bind RenderTarget */
	if (FAILED(m_pGameInstance->Begin_MultiRenderTarget(TEXT("MRT_Scene"))))
		return E_FAIL;

	RENDER_MACRO(RG_PRIORITY);

	/* UnBind RenderTarget */
	if (FAILED(m_pGameInstance->End_MultiRenderTarget()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Shadow()
{
	/* Light Exception */
	if (m_pGameInstance->Find_LightDesc(m_pGameInstance->Get_CurrentLevel(), 0) == nullptr)
		return S_OK;

	/* Cascade Shadow Mapping */
	_float	fSplitLambda	= 0.95f;
	_float	fRatio			= m_fCamFar / m_fCamNear;
	_float	fClipRange		= m_fCamFar - m_fCamNear;

	_float	fCascadeSplit[NUM_LIGHTDEPTH] = { };
	for (_int iCascade = 0; iCascade < NUM_LIGHTDEPTH; ++iCascade)
	{
		_float fP = (iCascade + 1.f) / static_cast<_float>(NUM_LIGHTDEPTH);
		_float fLog = static_cast<_float>(m_fCamNear * pow(fRatio, fP));
		_float fUniform = m_fCamNear + fClipRange * fP;
		_float fD = fSplitLambda * (fLog - fUniform) + fUniform;
		fCascadeSplit[iCascade] = ((fD - m_fCamNear) / fClipRange);
	}

	_float	fLastSplitDist = 0.f;
	for (_int iIndex = 0; iIndex < NUM_LIGHTDEPTH; ++iIndex)
	{
		_float	fSplitDist = fCascadeSplit[iIndex];

		/* Clip 공간 상의 절두체 */
		_float3	vFrustumCorners[8] =
		{
			/* Near */
			{ -1.f, 	1.f,	-1.f },
			{ 1.f, 		1.f,	-1.f },
			{ 1.f, 		-1.f,	-1.f },
			{ -1., 		-1.f,	-1.f },

			/* Far */
			{ -1.f, 	1.f,	1.f },
			{ 1.f, 		1.f,	1.f },
			{ 1.f, 		-1.f,	1.f },
			{ -1., 		-1.f,	1.f },
		};

		/* Clip To World */
		_matrix	InverseVPMatrix = XMMatrixIdentity();
		InverseVPMatrix = m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_PROJ) * m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_VIEW);

		for (_int iCorner = 0; iCorner < 8; ++iCorner)
		{
			_vector	vInverseCorner = XMVector4Transform(XMLoadFloat4(&_float4{ vFrustumCorners[iCorner].x, vFrustumCorners[iCorner].y, vFrustumCorners[iCorner].z, 1.f }), InverseVPMatrix);

			vFrustumCorners[iCorner] = 
				_float3{ 
					vInverseCorner.m128_f32[0] / vInverseCorner.m128_f32[3], 
					vInverseCorner.m128_f32[1] / vInverseCorner.m128_f32[3], 
					vInverseCorner.m128_f32[2] / vInverseCorner.m128_f32[3] 
				};
		}

		for (_int iCorner = 0; iCorner < 4; ++iCorner)
		{
			_float3	vDist = { };
			XMStoreFloat3(&vDist, XMLoadFloat3(&vFrustumCorners[iCorner + 4]) - XMLoadFloat3(&vFrustumCorners[iCorner]));

			XMStoreFloat3(&vFrustumCorners[iCorner + 4], XMLoadFloat3(&vFrustumCorners[iCorner]) + (XMLoadFloat3(&vDist) * fSplitDist));
			XMStoreFloat3(&vFrustumCorners[iCorner], XMLoadFloat3(&vFrustumCorners[iCorner]) + (XMLoadFloat3(&vDist) * fLastSplitDist));
		}

		/* 절두체의 중심점 계산 */
		_float3 vCenterPos = { };
		for (_int iCenter = 0; iCenter < 8; ++iCenter)
			XMStoreFloat3(&vCenterPos, XMLoadFloat3(&vCenterPos) + XMLoadFloat3(&vFrustumCorners[iCenter]));
		XMStoreFloat3(&vCenterPos, XMLoadFloat3(&vCenterPos) / 8.f);

		_float fRadius = { };
		for (_int iRadius = 0; iRadius < 8; ++iRadius)
		{
			_float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vFrustumCorners[iRadius]) - XMLoadFloat3(&vCenterPos)));
			fRadius = max(fRadius, fDistance);
		}

		fRadius = ceil(fRadius);

		/* AABB Box */
		_float3	vMaxExtents = { fRadius, fRadius, fRadius };
		_float3	vMinExtents = { -fRadius, -fRadius, -fRadius };

		_vector vShadowCamPos = { };
		_vector vLightDirection = XMLoadFloat4(&m_pGameInstance->Find_LightDesc(m_pGameInstance->Get_CurrentLevel(), 0)->vDirection);
		vLightDirection = XMVector4Normalize(vLightDirection);

		vShadowCamPos.m128_f32[0] = vCenterPos.x + (vLightDirection.m128_f32[0] * vMinExtents.z);
		vShadowCamPos.m128_f32[1] = vCenterPos.y + (vLightDirection.m128_f32[1] * vMinExtents.z);
		vShadowCamPos.m128_f32[2] = vCenterPos.z + (vLightDirection.m128_f32[2] * vMinExtents.z);
		vShadowCamPos.m128_f32[3] = 1.f;

		_matrix LightViewMatrix = XMMatrixLookAtLH(vShadowCamPos, XMLoadFloat3(&vCenterPos), { 0.f, 1.f, 0.f, 0.f });

		_float3 vCascadeExtents = { };
		vCascadeExtents.x = vMaxExtents.x - vMinExtents.x;
		vCascadeExtents.y = vMaxExtents.y - vMinExtents.y;
		vCascadeExtents.z = vMaxExtents.z - vMinExtents.z;
		
		_matrix	LightOrhtoMatrix = XMMatrixOrthographicLH(vCascadeExtents.x, vCascadeExtents.y, -m_fCamFar, vCascadeExtents.z);

		/* Offset */
		_vector	vShadowOrigin = { 0.f, 0.f, 0.f, 1.f };
		vShadowOrigin = XMVector4Transform(vShadowOrigin, LightOrhtoMatrix * LightViewMatrix);
		vShadowOrigin = vShadowOrigin * (LIGHTDEPTH_X * 0.5f);

		_vector	vRoundedOrigin = XMVectorRound(vShadowOrigin);
		_vector	vRoundOffset = vRoundedOrigin - vShadowOrigin;

		vRoundOffset = vRoundOffset * 2.f / LIGHTDEPTH_X;
		vRoundOffset.m128_f32[2] = 0.f;
		vRoundOffset.m128_f32[3] = 0.f;

		LightOrhtoMatrix.r[3] += vRoundOffset;

		XMStoreFloat4x4(&m_ShadowViewProj[iIndex], LightViewMatrix * LightOrhtoMatrix);
		m_fCascadeClip[iIndex] = (m_fCamNear + fSplitDist * fClipRange);
	
		fLastSplitDist = fCascadeSplit[iIndex];
	}

	if (FAILED(m_pGameInstance->Begin_MultiRenderTarget(TEXT("MRT_Shadow"), m_pShadowDSV)))
		return E_FAIL;

	Set_ViewPort(LIGHTDEPTH_X, LIGHTDEPTH_Y);

	for (auto& pGameObject : m_RenderObjects[RG_SHADOW])
	{
		if (nullptr != pGameObject)
			pGameObject->Render_Shadow(m_ShadowViewProj, NUM_LIGHTDEPTH);

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RG_SHADOW].clear();

	if (FAILED(m_pGameInstance->End_MultiRenderTarget()))
		return E_FAIL;

	Set_ViewPort(WINCX, WINCY);

	return S_OK;
}

HRESULT CRenderer::Render_Terrain()
{
	/* Bind RenderTarget */
	if (FAILED(m_pGameInstance->Begin_MultiRenderTarget(TEXT("MRT_Scene"), nullptr, FALSE, FALSE)))
		return E_FAIL;

	RENDER_MACRO(RG_TERRAIN);
	RENDER_MACRO(RG_OBJECT);

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	RENDER_MACRO(RG_NONBLEND);

	return S_OK;
}

HRESULT CRenderer::Render_MTrail()
{
	for (auto& iter : m_RenderObjects[RG_MTRAIL])
	{
		if (iter != nullptr)
		{
			if (FAILED(iter->Render_MTrail()))
			{
				MSG_BOX(TEXT("Failed to Render : CRenderer"));
			}
		}

		Safe_Release(iter);
	}

	m_RenderObjects[RG_MTRAIL].clear();

	/* UnBind RenderTarget */
	if (FAILED(m_pGameInstance->End_MultiRenderTarget()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
	/* Bind RenderTarget */
	if (FAILED(m_pGameInstance->Begin_MultiRenderTarget(TEXT("MRT_Light"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(m_pShader, "g_NormalTexture", TEXT("Target_SceneNormal"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(m_pShader, "g_WorldTexture", TEXT("Target_SceneWorld"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(m_pShader, "g_ShadowWorldTexture", TEXT("Target_SceneShadowWorld"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(m_pShader, "g_LightMapTexture", TEXT("Target_SceneLightMap"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(m_pShader, "g_SceneFactorTexture", TEXT("Target_SceneFactor"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(m_pShader, "g_SceneFaceTexture", TEXT("Target_SceneFace"))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	/* Bind Resource */
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	/* Render */
	if (FAILED(m_pGameInstance->Render_Lights(m_pGameInstance->Get_CurrentLevel(), m_pShader, m_pVIBuffer)))
		return E_FAIL;

	/* UnBind RenderTarget */
	if (FAILED(m_pGameInstance->End_MultiRenderTarget()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	/* Bind RenderTarget */
	if (FAILED(m_pGameInstance->Begin_MultiRenderTarget(TEXT("MRT_Scene"), nullptr, FALSE, FALSE)))
		return E_FAIL;

	RENDER_MACRO(RG_BLEND);

	/* UnBind RenderTarget */
	if (FAILED(m_pGameInstance->End_MultiRenderTarget()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Trail()
{
	/* Bind RenderTarget */
	if (FAILED(m_pGameInstance->Begin_MultiRenderTarget(TEXT("MRT_Effect"))))
		return E_FAIL;

	for (auto& iter : m_RenderObjects[RG_TRAIL])
	{
		if (iter != nullptr)
		{
			if (FAILED(iter->Render_Trail()))
			{
				MSG_BOX(TEXT("Failed to Render : CRenderer"));
			}
		}

		Safe_Release(iter);
	}

	m_RenderObjects[RG_TRAIL].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Particle()
{
	RENDER_MACRO(RG_EFFECT);

	for (auto& iter : m_RenderParticles)
	{
		if (iter != nullptr)
		{
			if (FAILED(iter->Render()))
			{
				MSG_BOX(TEXT("Failed to Render : CRenderer"));
			}
		}

		Safe_Release(iter);
	}

	m_RenderParticles.clear();

	/* UnBind RenderTarget */
	if (FAILED(m_pGameInstance->End_MultiRenderTarget()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_SceneDeferred()
{
	/* Bind RenderTarget */
	if (FAILED(m_pGameInstance->Begin_MultiRenderTarget(TEXT("MRT_PostProcessing"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(m_pShader, "g_DiffuseTexture", TEXT("Target_SceneDiffuse"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(m_pShader, "g_WorldTexture", TEXT("Target_SceneWorld"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(m_pShader, "g_ShadowWorldTexture", TEXT("Target_SceneShadowWorld"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(m_pShader, "g_SceneFactorTexture", TEXT("Target_SceneFactor"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(m_pShader, "g_ShadeTexture", TEXT("Target_Shade"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(m_pShader, "g_RimLightTexture", TEXT("Target_RimLight"))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_ShaderResourceView("g_LightDepthTexture", m_pShadowSRV)))
		return E_FAIL;

	/* Bind Resource */
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_SceneViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_SceneProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrices("g_LightViewProj", m_ShadowViewProj, NUM_LIGHTDEPTH)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vCascadeClip", m_fCascadeClip, sizeof(_float3))))
		return E_FAIL;

#ifdef _DEBUG
	static _bool bRenderCascade = FALSE;

	if (FAILED(m_pShader->Bind_RawValue("g_bDebugCascade", &bRenderCascade, sizeof(_bool))))
		return E_FAIL;
#endif

	if (FAILED(m_pShader->Bind_Matrix("g_SceneViewMatrixInv", m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_SceneProjMatrixInv", m_pGameInstance->Get_Transform_Inverse_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	/* Render */
	m_pShader->Begin(DEFERRED_SCENE);

	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MultiRenderTarget()))
		return E_FAIL;

	/* Post Processing */
	CRenderTarget* pSceneSRV = m_pGameInstance->Find_RenderTarget(L"Target_PostProcessing");
	if (pSceneSRV == nullptr)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Scene(pSceneSRV->Get_ShaderResourceView())))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_EffectDeferred()
{
	if (FAILED(m_pGameInstance->Bind_RenderTarget(m_pShader, "g_DiffuseTexture", TEXT("Target_EffectDiffuse"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(m_pShader, "g_FactorTexture", TEXT("Target_EffectFactor"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RenderTarget(m_pShader, "g_WeightTexture", TEXT("Target_EffectWeight"))))
		return E_FAIL;

	/* Render */
	m_pShader->Begin(DEFERRED_EFFECT);

	m_pVIBuffer->Render();

	/* Post Processing */
	if (FAILED(m_pGameInstance->Render_Effect(nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_PostEffect()
{
	RENDER_MACRO(RG_POSTEFFECT);

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	RENDER_MACRO(RG_PREVUI);
	RENDER_MACRO(RG_LATEUI);
	RENDER_MACRO(RG_POPUPUI);

	return S_OK;
}

HRESULT CRenderer::Render_Font()
{
	RENDER_MACRO(RG_FONT);

	return S_OK;
}

HRESULT CRenderer::Render_Mouse()
{
	RENDER_MACRO(RG_MOUSE);

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Render_Debug()
{
	if (m_bRenderRTV == TRUE)
	{
		if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Render_RTDebug(TEXT("MRT_Effect"), m_pShader, m_pVIBuffer)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Render_RTDebug(TEXT("MRT_Scene"), m_pShader, m_pVIBuffer)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Render_RTDebug(TEXT("MRT_Light"), m_pShader, m_pVIBuffer)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Render_RTDebug(TEXT("MRT_PostProcessing"), m_pShader, m_pVIBuffer)))
			return E_FAIL;

		if (FAILED(CRenderer::Render_LightDepth()))
			return E_FAIL;
	}

	if (m_bRenderDebugCom == TRUE)
	{
		for (auto& pDebugCom : m_DebugComponent)
		{
			pDebugCom->Render();

			Safe_Release(pDebugCom);
		}
		m_DebugComponent.clear();
	}

	return S_OK;
}

HRESULT CRenderer::Render_LightDepth()
{
	_float fIndex = 0.f;

	if (FAILED(m_pShader->Bind_ShaderResourceView("g_TextureArray", m_pShadowSRV)))
		return E_FAIL;

	for (_int iIndex = 0; iIndex < NUM_LIGHTDEPTH; ++iIndex)
	{
		_float4x4 CombinedMatrix = m_LightDepthMatrix;

		CombinedMatrix._41 -= iIndex * CombinedMatrix._11;

		if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_RawValue("g_fTexArrayIndex", &(fIndex = static_cast<_float>(iIndex)), sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShader->Begin(DEFERRED_MULTIDEBUG)))
			return E_FAIL;

		if (FAILED(m_pVIBuffer->Render()))
			return E_FAIL;
	}

	return S_OK;
}
#endif

HRESULT CRenderer::Initialize_Prototype()
{
	_uint           iNumViewports = { 1 };
	D3D11_VIEWPORT  ViewportDesc = { };

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

#pragma region PostProcessing RenderTarget

	if (FAILED(m_pGameInstance->Create_RenderTarget(TEXT("Target_PostProcessing"),
		static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height),
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 0.f, 1.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Create_RenderTarget(TEXT("Target_DeferredDepth"),
		static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height),
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

#pragma endregion

#pragma region PostProcessing
	if (FAILED(m_pGameInstance->Add_MultiRenderTarget(TEXT("MRT_PostProcessing"), TEXT("Target_PostProcessing"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MultiRenderTarget(TEXT("MRT_PostProcessing"), TEXT("Target_DeferredDepth"))))
		return E_FAIL;
#pragma endregion

#pragma region Shadow RenderTarget

	if (FAILED(m_pGameInstance->Create_RenderTarget(TEXT("Target_LightDepth"),
		LIGHTDEPTH_X, LIGHTDEPTH_Y,
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 1.f, 0.f, 1.f), NUM_LIGHTDEPTH)))
		return E_FAIL;

#pragma endregion

#pragma region Shadow
	if (FAILED(m_pGameInstance->Add_MultiRenderTarget(TEXT("MRT_Shadow"), TEXT("Target_LightDepth"))))
		return E_FAIL;
#pragma endregion

#pragma region Scene RenderTarget

	if (FAILED(m_pGameInstance->Create_RenderTarget(TEXT("Target_SceneDiffuse"),
		static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height),
		DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Create_RenderTarget(TEXT("Target_SceneNormal"),
		static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height),
		DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Create_RenderTarget(TEXT("Target_SceneWorld"),
		static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height),
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Create_RenderTarget(TEXT("Target_SceneShadowWorld"),
		static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height),
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Create_RenderTarget(TEXT("Target_SceneLightMap"),
		static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height),
		DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Create_RenderTarget(TEXT("Target_SceneFactor"),
		static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height),
		DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(1.f, 0.f, 0.f, 1.f))))	// R : Additional Exposure, G : Not Outline, BA : Face Factor
		return E_FAIL;

	if (FAILED(m_pGameInstance->Create_RenderTarget(TEXT("Target_SceneFace"),
		static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height),
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Create_RenderTarget(TEXT("Target_SceneCShader"),
		static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height),
		DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 1.f, 0.f, 0.f))))	// R : Additional Bloom
		return E_FAIL;

#pragma endregion

#pragma region Scenes
	if (FAILED(m_pGameInstance->Add_MultiRenderTarget(TEXT("MRT_Scene"), TEXT("Target_SceneDiffuse"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MultiRenderTarget(TEXT("MRT_Scene"), TEXT("Target_SceneNormal"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MultiRenderTarget(TEXT("MRT_Scene"), TEXT("Target_SceneWorld"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MultiRenderTarget(TEXT("MRT_Scene"), TEXT("Target_SceneShadowWorld"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MultiRenderTarget(TEXT("MRT_Scene"), TEXT("Target_SceneLightMap"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MultiRenderTarget(TEXT("MRT_Scene"), TEXT("Target_SceneFactor"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MultiRenderTarget(TEXT("MRT_Scene"), TEXT("Target_SceneFace"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MultiRenderTarget(TEXT("MRT_Scene"), TEXT("Target_SceneCShader"))))
		return E_FAIL;
#pragma endregion

#pragma region Light RenderTarget

	if (FAILED(m_pGameInstance->Create_RenderTarget(TEXT("Target_Shade"),
		static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height),
		DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Create_RenderTarget(TEXT("Target_RimLight"),
		static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height),
		DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

#pragma endregion

#pragma region Light
	if (FAILED(m_pGameInstance->Add_MultiRenderTarget(TEXT("MRT_Light"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MultiRenderTarget(TEXT("MRT_Light"), TEXT("Target_RimLight"))))
		return E_FAIL;
#pragma endregion

#pragma region Effect RenderTarget

	if (FAILED(m_pGameInstance->Create_RenderTarget(TEXT("Target_EffectDiffuse"),
		static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height),
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Create_RenderTarget(TEXT("Target_EffectFactor"),
		static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height),
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Create_RenderTarget(TEXT("Target_EffectWeight"),
		static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height),
		DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

#pragma endregion

#pragma region Effect
	if (FAILED(m_pGameInstance->Add_MultiRenderTarget(TEXT("MRT_Effect"), TEXT("Target_EffectDiffuse"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MultiRenderTarget(TEXT("MRT_Effect"), TEXT("Target_EffectFactor"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MultiRenderTarget(TEXT("MRT_Effect"), TEXT("Target_EffectWeight"))))
		return E_FAIL;
#pragma endregion

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, VTXPOSTEX::VertexElements, VTXPOSTEX::iNumElements,
		TEXT("../../Reference/ShaderFiles/Shader_Deferred.hlsl"));
	if (nullptr == m_pShader)
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_WorldMatrix._11 = ViewportDesc.Width;
	m_WorldMatrix._22 = ViewportDesc.Height;
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	if (FAILED(Ready_ShadowDepthStencilView(LIGHTDEPTH_X, LIGHTDEPTH_Y)))
		return E_FAIL;

#ifdef _DEBUG

	XMStoreFloat4x4(&m_LightDepthMatrix, XMMatrixIdentity());
	m_LightDepthMatrix._11 = 150.f;
	m_LightDepthMatrix._22 = 150.f;
	m_LightDepthMatrix._41 = (WINCX - (m_LightDepthMatrix._11 * 0.5f)) - ViewportDesc.Width * 0.5f;
	m_LightDepthMatrix._42 = (WINCY - (m_LightDepthMatrix._22 * 0.5f)) - ViewportDesc.Height * 0.5f;

	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_EffectDiffuse"), 75.f, 75.f, 150.f, 150.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_EffectFactor"), 225.f, 75.f, 150.f, 150.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_EffectWeight"), 375.f, 75.f, 150.f, 150.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_SceneWorld"), 75.f, 225.f, 150.f, 150.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_SceneFactor"), 225.f, 225.f, 150.f, 150.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_DeferredDepth"), 375.f, 225.f, 150.f, 150.f)))
		return E_FAIL;
	
#endif

	return S_OK;
}

HRESULT CRenderer::Initialize_Clone(void* pArg)
{
	return S_OK;
}

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CRenderer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CRenderer::Clone(void* pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);

	Safe_Release(m_pShadowDSV);
	Safe_Release(m_pShadowSRV);

	for (auto& ObjectList : m_RenderObjects)
	{
		for (auto& pGameObject : ObjectList)
			Safe_Release(pGameObject);
		ObjectList.clear();
	}

	for (auto& Particle : m_RenderParticles)
		Safe_Release(Particle);
	m_RenderParticles.clear();

#ifdef _DEBUG
	for (auto& Effect : m_DebugComponent)
		Safe_Release(Effect);
	m_DebugComponent.clear();
#endif
}
