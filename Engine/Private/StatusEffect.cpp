#include "StatusEffect.h"

CStatusEffect::CStatusEffect()
{
}

void CStatusEffect::Set_Count_Compare(_int iCount, _bool bMax)
{
	if (bMax == TRUE)
		m_iCount = max(iCount, m_iCount);
	else
		m_iCount = iCount;
}

HRESULT CStatusEffect::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	STATUSEFFECT_DESC* pDesc = (STATUSEFFECT_DESC*)pArg;
	m_iCount	= pDesc->iCount;
	m_fDuration = pDesc->fDuration;

	return S_OK;
}

HRESULT CStatusEffect::Update(_float fTimeDelta)
{
	if (m_fDuration <= 0.f)
		return S_OK;

	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc >= m_fDuration)
	{
		m_fTimeAcc = 0.f;
		if (m_iCount <= 0)
			CStatusEffect::Set_Destroy(TRUE);
		else
			--m_iCount;
	}

	return S_OK;
}

CStatusEffect* CStatusEffect::Create(void* pArg)
{
	CStatusEffect* pInstance = new CStatusEffect;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Create : CStatusEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatusEffect::Free()
{
	__super::Free();


}
