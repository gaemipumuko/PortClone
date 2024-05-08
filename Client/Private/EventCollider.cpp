#include "stdafx.h"
#include "EventCollider.h"

CEventCollider::CEventCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDeciderObject(pDevice, pContext)
{
}

CEventCollider::CEventCollider(const CEventCollider& rhs)
	: CDeciderObject(rhs)
{
}

HRESULT CEventCollider::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CEventCollider::Free()
{
	__super::Free();
}
