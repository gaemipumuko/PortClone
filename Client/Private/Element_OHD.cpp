#include "stdafx.h"
#include "Element_OHD.h"

#include "GameInstance.h"

CElement_OHD::CElement_OHD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: COrtho(pDevice, pContext)
{
}

CElement_OHD::CElement_OHD(const CElement_OHD& rhs)
	: COrtho(rhs)
{
}

HRESULT CElement_OHD::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Element_OHD"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CElement_OHD::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CElement_OHD::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	COrtho::Initialize_Matrix();

	m_eRenderType = RTYPE_GAMEPLAY;

	m_vUVScale = { 0.25f ,0.5f };

	return S_OK;
}

HRESULT CElement_OHD::PreviousTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{

	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CElement_OHD::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (FAILED(__super::Tick(fTimeDelta)))
			return E_FAIL;

		if(m_fTimeAcc >=0.f)
		m_fTimeAcc += fTimeDelta*2.f;

		if (m_fTimeAcc > 2.f)
		{
			m_fTimeAcc = -1.f;
			
			for (_int i = 0; i < OVERHEAD_END; i++)
			{
				if (m_bElementReactStatus[i] == true)
				{
					m_bElementReactStatus[i] = false;
					m_bElementStatus[i] = false;
				}
			}

		}

		/*if(m_pGameInstance->Key_Down('Z'))
		{
			Set_Element(OVERHEAD_LIGHTNING,true);
		}
		if (m_pGameInstance->Key_Down('X'))
		{
			Set_Element(OVERHEAD_FIRE,true);

		}
		if (m_pGameInstance->Key_Down('I'))
		{
			React_Elements(OVERHEAD_LIGHTNING, OVERHEAD_FIRE);
		}*/



		_int iNumElements = 0;

		m_vecElements.clear();
		for (_int i = 0; i < OVERHEAD_END; i++)
		{
			if (m_bElementStatus[i] == true)
			{
				iNumElements++;
				m_vecElements.push_back(static_cast<ELEMENTS_OVERHEAD>(i));
			}
		}
		m_iNumElements = iNumElements;
	}
	return S_OK;
}

HRESULT CElement_OHD::LateTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (FAILED(__super::LateTick(fTimeDelta)))
			return E_FAIL;
	}
	    //m_pRendererCom->Add_RenderGroup(CRenderer::RG_LATEUI, this);
    

	return S_OK;
}

HRESULT CElement_OHD::Render()
{

	_float4x4 ElementPosMat = {};
	_float2 vPos;
	_int	iCol, iRow;
	_bool	bEnlarge = false;

	if (m_iNumElements > 0)
	{

		for (_int i = 0; i < m_iNumElements; i++)
		{


			ElementPosMat = m_pTransformCom->Get_WorldFloat4x4();
			if (m_iNumElements % 2 == 1)
			{
				ElementPosMat._41 -= ElementPosMat._11 * (m_iNumElements/2-i);
			}
			else if (m_iNumElements % 2 == 0)
			{
				ElementPosMat._41 -= ElementPosMat._11 * (m_iNumElements / 2 - i)- ElementPosMat._11/2.f;
			}

			ELEMENTS_OVERHEAD eElement = m_vecElements[i];
			

			iRow = eElement / 4;
			iCol = eElement % 4;

			vPos = { static_cast<_float>(iCol)* m_vUVScale.x, static_cast<_float>(iRow)* m_vUVScale.y };

			if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", ElementPosMat)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_UVScale", &m_vUVScale, sizeof(_float2))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_UVPosition", &vPos, sizeof(_float2))))
				return E_FAIL;

			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
				return E_FAIL;

			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 1)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_bEnlarge", &bEnlarge, sizeof(_bool))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(UI_ELEMENT_OHD)))
				return E_FAIL;

			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;

			if (m_bElementReactStatus[eElement] == true)
			{
				ElementPosMat._11 += m_fTimeAcc*30.f;
				ElementPosMat._22 += m_fTimeAcc*30.f;

				if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", ElementPosMat)))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Bind_RawValue("g_bEnlarge", &m_bElementReactStatus[eElement], sizeof(_bool))))
					return E_FAIL;

				if (FAILED(m_pShaderCom->Begin(UI_ELEMENT_OHD)))
					return E_FAIL;

				if (FAILED(m_pVIBufferCom->Render()))
					return E_FAIL;
			}
		}
	}

	return S_OK;
}

void CElement_OHD::Update_Matrix(_vector vWorldPos, _float fSizeX, _float fSizeY)
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
}

void CElement_OHD::Update_Matrix(_float fX, _float fY, _float fSizeX, _float fSizeY)
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
}

void CElement_OHD::Set_Element(ELEMENTS_OVERHEAD eElements, _bool bSwitch)
{
	if (m_bElementStatus[eElements] == false || bSwitch == false)
	{
		m_bElementStatus[eElements] = bSwitch;
		m_vecElements.push_back(eElements);
		m_iNumElements++;
	}

}

void CElement_OHD::React_Elements(ELEMENTS_OVERHEAD eElement1, ELEMENTS_OVERHEAD eElement2)
{

	if (m_bElementStatus[eElement1] == true && m_bElementStatus[eElement2] == true)
	{
		m_bElementReactStatus[eElement1] = true;
		m_bElementReactStatus[eElement2] = true;

		m_fTimeAcc = 0;
	}
}

void CElement_OHD::Zero_Elements()
{
	for (_uint i = 0; i < OVERHEAD_END; ++i)
	{
		m_bElementStatus[i] = false;
	}
}

CElement_OHD* CElement_OHD::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CElement_OHD* pInstance = new CElement_OHD(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CElement_OHD"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CElement_OHD::Clone(void* pArg)
{
	CElement_OHD* pInstance = new CElement_OHD(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CElement_OHD"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CElement_OHD::Free()
{
	__super::Free();
}
