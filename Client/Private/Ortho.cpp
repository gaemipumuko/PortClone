#include "stdafx.h"
#include "Ortho.h"
// #include "CUI_Manager.h"

#include "GameInstance.h"

COrtho::COrtho(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDeciderObject(pDevice, pContext)
{
}

COrtho::COrtho(const COrtho& rhs)
	: CDeciderObject(rhs)
{
}

HRESULT COrtho::Add_Component(void* pArg)
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

	return S_OK;
}

HRESULT COrtho::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_vUVScale, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &m_vUVPosition, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderCurrentTexture(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

HRESULT COrtho::Initialize_Matrix()
{
	_vector vScale = XMLoadFloat2(&m_tOrthoDesc.fSize);
	m_pTransformCom->Set_Scale(vScale);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_tOrthoDesc.fPosition.x, m_tOrthoDesc.fPosition.y, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(static_cast<_float>(g_iWinSizeX), static_cast<_float>(g_iWinSizeY), 0.f, 1.f));

	return S_OK;
}

HRESULT COrtho::Update_Matrix(_vector vWorldPos, _float fSizeX, _float fSizeY)
{
	_float2 vPosition2D;
	_vector vPos;
	_matrix ProjMatrix, ViewMatrix, WorldMatrix;
	ViewMatrix = m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW);
	ProjMatrix = m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ);
	vPos = vWorldPos;
	vPos.m128_f32[1] += 1.5f;

	WorldMatrix = XMMatrixIdentity();

	XMStoreFloat2(&vPosition2D, XMVector3Project(vPos,
		0.f, 0.f, (_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f, ProjMatrix, ViewMatrix, WorldMatrix));

	// 월드 좌표를 정규화된 장치 좌표로 변환
	XMMATRIX worldViewProjectionMatrix = WorldMatrix * ViewMatrix * ProjMatrix;
	XMFLOAT4 clipCoord;
	XMStoreFloat4(&clipCoord, XMVector3TransformCoord(vPos, worldViewProjectionMatrix));

	// 정규화된 장치 좌표를 화면 좌표로 변환
	float screenX = (clipCoord.x + 1.0f) * 0.5f * (_float)g_iWinSizeX;
	float screenY = (1.0f - clipCoord.y) * 0.5f * (_float)g_iWinSizeY;

	m_tOrthoDesc.fPosition.x = screenX - g_iWinSizeX / 2;
	m_tOrthoDesc.fPosition.y = -(screenY - g_iWinSizeY / 2);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_tOrthoDesc.fPosition.x, m_tOrthoDesc.fPosition.y, 0.f, 1.f));

	if (fSizeX != 0.f || fSizeY != 0.f)
	{
		m_tOrthoDesc.fSize.x = fSizeX;
		m_tOrthoDesc.fSize.y = fSizeY;

		_vector vScale = XMLoadFloat2(&m_tOrthoDesc.fSize); vScale.m128_f32[2] = 1.f;
		m_pTransformCom->Set_Scale(vScale);
	}

	return S_OK;
}

HRESULT COrtho::Update_Matrix(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	m_tOrthoDesc.fPosition.x = fX;
	m_tOrthoDesc.fPosition.y = fY;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_tOrthoDesc.fPosition.x, m_tOrthoDesc.fPosition.y, 0.f, 1.f));

	if (fSizeX != 0.f || fSizeY != 0.f)
	{
		m_tOrthoDesc.fSize.x = fSizeX;
		m_tOrthoDesc.fSize.y = fSizeY;

		_vector vScale = XMLoadFloat2(&m_tOrthoDesc.fSize); vScale.m128_f32[2] = 1.f;
		m_pTransformCom->Set_Scale(vScale);
	}

	return S_OK;
}

HRESULT COrtho::Initialize_Prototype()
{
	return S_OK;
}

HRESULT COrtho::Initialize_Clone(void* pArg)
{
	if (pArg != nullptr)
	{
		ORTHO_DESC* pDesc = static_cast<ORTHO_DESC*>(pArg);

		m_tOrthoDesc.eTextureLevel = pDesc->eTextureLevel;
		m_tOrthoDesc.strTextureTag = pDesc->strTextureTag;
		m_tOrthoDesc.fPosition = pDesc->fPosition;
		m_tOrthoDesc.fSize = pDesc->fSize;
		m_tOrthoDesc.ePage = pDesc->ePage;
		m_tOrthoDesc.iDepth = pDesc->iDepth;

		m_vDimentionOrigin.x = pDesc->fPosition.x;
		m_vDimentionOrigin.y = pDesc->fPosition.y;
		m_vDimentionOrigin.z = pDesc->fSize.x;
		m_vDimentionOrigin.w = pDesc->fSize.y;

		if(pDesc->ePage == PAGE_INVEN)
			m_eUIState = RENDER_UI::RUI_INVEN;
	}

	return S_OK;
}

HRESULT COrtho::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT COrtho::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT COrtho::LateTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		switch (m_tOrthoDesc.iDepth)
		{
		case 0:
			m_pRendererCom->Add_RenderGroup(CRenderer::RG_PREVUI, this);
			break;
		case 1:
			m_pRendererCom->Add_RenderGroup(CRenderer::RG_LATEUI, this);
			break;
		case 2:
			m_pRendererCom->Add_RenderGroup(CRenderer::RG_POPUPUI, this);
			break;
		case 3:
			m_pRendererCom->Add_RenderGroup(CRenderer::RG_FONT, this);
			break;
		default:
			break;
		}
	}

	return S_OK;
}

HRESULT COrtho::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(UI_DEFAULT)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

_bool COrtho::In_From_Right(_float fDestX, _float fSpeed)
{
	if (m_tOrthoDesc.fPosition.x > fDestX)
	{
	Update_Matrix(m_tOrthoDesc.fPosition.x - fSpeed, m_tOrthoDesc.fPosition.y, m_tOrthoDesc.fSize.x, m_tOrthoDesc.fSize.y);

		return false;
	}
	else
	{
		return false;
	}
}

_bool COrtho::In_From_Below(_float fDestY, _float fSpeed)
{
	if (m_tOrthoDesc.fPosition.y < fDestY)
	{
		Update_Matrix(m_tOrthoDesc.fPosition.x , m_tOrthoDesc.fPosition.y + fSpeed, m_tOrthoDesc.fSize.x, m_tOrthoDesc.fSize.y);

		return false;
	}
	else
	{
		return true;
	}
}

void COrtho::SetTextureIndex(_uint iTextureIndex)
{
	m_pTextureCom->Set_CurrentTexture(static_cast<_float>(iTextureIndex));

}

void COrtho::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}
