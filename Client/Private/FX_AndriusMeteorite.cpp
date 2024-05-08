#include "stdafx.h"
#include "FX_AndriusMeteorite.h"

#include "FX_Meteorite.h"
#include "DeciderObject.h"
#include "GameInstance.h"

#define RADIUS 40
#define INIT_RANGE 120.f

CFX_AndriusMeteorite::CFX_AndriusMeteorite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAssetFX(pDevice, pContext)
{
}

CFX_AndriusMeteorite::CFX_AndriusMeteorite(const CFX_AndriusMeteorite& rhs)
	: CAssetFX(rhs)
{
}

HRESULT CFX_AndriusMeteorite::Add_Component(void* pArg)
{
	/* For.Com_Transform */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFX_AndriusMeteorite::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CFX_AndriusMeteorite::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFX_AndriusMeteorite::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	ASFX_DESC* pAsfxDesc = static_cast<ASFX_DESC*>(pArg);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vector{ 953.0f, 0.f, 187.0f, 1.0f });

	return S_OK;
}

HRESULT CFX_AndriusMeteorite::PreviousTick(_float fTimeDelta)
{
	if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
	{
		if (FAILED(__super::PreviousTick(fTimeDelta)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CFX_AndriusMeteorite::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
	{
		m_fFrame += fTimeDelta;

		if (m_fFrame > m_fMeteoriteDelay)
		{
			CFX_Meteorite::METEO_DESC tMeteoDesc = { };
			tMeteoDesc.ePosType = CFX_Meteorite::METEO_DESC::POS_FIX;
			tMeteoDesc.fDistance = INIT_RANGE;
			tMeteoDesc.vMeteoriteColor = _float4{ 0.4392132f, 0.6797233f, 0.8867924f, 1.f };

			for (_int iMeteorite = 0; iMeteorite < m_iMeteoriteCount; ++iMeteorite)
			{
				_float fRadius = m_pGameInstance->Get_Float_Random(0.f, RADIUS);
				_float fTheta = XMScalarACos(m_pGameInstance->Get_Float_Random(0.f, 1.f) * 2.f - 1.f);
				_float fPhi = m_pGameInstance->Get_Float_Random(0.f, 1.f) * XM_2PI;

				tMeteoDesc.vTargetPosition = { 953.0f + fRadius * sin(fPhi) * cos(fTheta), 0.f, 187.0f + fRadius * cos(fPhi), 1.0f };

				_vector vDirection = { m_pGameInstance->Get_Float_Random(-1.f, 1.f), 2.f, m_pGameInstance->Get_Float_Random(-1.f, 1.f), 0.f };
				tMeteoDesc.vInitPosition = XMLoadFloat4(&tMeteoDesc.vTargetPosition) + INIT_RANGE * XMVector3Normalize(vDirection);
				XMStoreFloat4(&tMeteoDesc.vDirection, vDirection);

				m_pGameInstance->Play_AssetFX(L"Meteorite", Get_ParticleTag(L"Meteorite"), &tMeteoDesc);
			}

			m_fFrame = 0.f;
		}
	}

	return S_OK;
}

HRESULT CFX_AndriusMeteorite::LateTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CFX_AndriusMeteorite::Render()
{
	return S_OK;
}

CFX_AndriusMeteorite* CFX_AndriusMeteorite::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFX_AndriusMeteorite* pInstance = new CFX_AndriusMeteorite(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CFX_AndriusMeteorite"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFX_AndriusMeteorite::Clone(void* pArg)
{
	CFX_AndriusMeteorite* pInstance = new CFX_AndriusMeteorite(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CFX_AndriusMeteorite"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFX_AndriusMeteorite::Free()
{
	__super::Free();
	
	Safe_Release(m_pTransformCom);
}
