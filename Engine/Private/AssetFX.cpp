#include "AssetFX.h"

CAssetFX::CAssetFX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CAssetFX::CAssetFX(const CAssetFX& rhs)
	: CGameObject(rhs)
{
}

HRESULT CAssetFX::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	ASFX_DESC* pAsfxDesc = static_cast<ASFX_DESC*>(pArg);

	if (pAsfxDesc->fDuration != -1.f)
	{
		m_bDuration = TRUE;
		m_fDuration = pAsfxDesc->fDuration;
	}

	return S_OK;
}

HRESULT CAssetFX::PreviousTick(_float fTimeDelta)
{
	if (m_fDuration > 0.f)
		m_fDuration -= fTimeDelta;
	
	return S_OK;
}

void CAssetFX::Free()
{
	__super::Free();

	Safe_Release(m_pRelativeTransform);
}
