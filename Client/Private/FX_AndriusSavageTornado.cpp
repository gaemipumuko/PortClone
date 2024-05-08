#include "stdafx.h"
#include "FX_AndriusSavageTornado.h"

#include "DeciderObject.h"
#include "GameInstance.h"

_float CFX_AndriusSavageTornado::TORNADO_DESC::fMaxLifeTime = 5.f;

CFX_AndriusSavageTornado::CFX_AndriusSavageTornado(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAssetFX(pDevice, pContext)
{
}

CFX_AndriusSavageTornado::CFX_AndriusSavageTornado(const CFX_AndriusSavageTornado& rhs)
	: CAssetFX(rhs)
{
}

HRESULT CFX_AndriusSavageTornado::Add_Component(void* pArg)
{
	/* For.Com_Transform */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFX_AndriusSavageTornado::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CFX_AndriusSavageTornado::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFX_AndriusSavageTornado::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	ASFX_DESC* pAsfxDesc = static_cast<ASFX_DESC*>(pArg);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vector{ 953.0f, 0.f, 187.0f, 1.0f });

	return S_OK;
}

HRESULT CFX_AndriusSavageTornado::PreviousTick(_float fTimeDelta)
{
	if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
	{
		if (FAILED(__super::PreviousTick(fTimeDelta)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CFX_AndriusSavageTornado::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Get_RenderMode() != CDeciderObject::RMODE_UI)
	{
		/* Frame */
		m_vFrame.x += fTimeDelta;

		/* Create Tornado */
		if (m_vFrame.x > 2.f)
		{
			for (_uint i = 0; i < TORNADO_DESC::DIR_END; ++i)
			{
				CTransform* pTransform = { nullptr };

				TORNADO_DESC tTornadoDesc = { };
				tTornadoDesc.fLifeTime = 0.f;
				tTornadoDesc.bTrailDestroy = FALSE;

				CAssetFX::ASFX_DESC tAsfxDesc = { };
				tAsfxDesc.ePosType = CAssetFX::ASFX_DESC::POS_RELATIVE;

				/* Tornado_1 */
				pTransform = static_cast<CTransform*>(m_pGameInstance->Find_Component_Prototype(LEVEL_STATIC, L"Prototype_Component_Transform")->Clone(nullptr));
				pTransform->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix());

				tTornadoDesc.strFXTag = Get_ParticleTag(L"Tornado");
				tTornadoDesc.pTransform = pTransform;
				tTornadoDesc.eDirection = TORNADO_DESC::DIRECTION(i);
				m_TornadoTransforms.push_back(tTornadoDesc);
				tAsfxDesc.pTransform = pTransform;
				m_pGameInstance->Play_AssetFX(L"Tornado", tTornadoDesc.strFXTag, &tAsfxDesc);
			}

			m_vFrame.x = 0.f;
		}

		/* Invalidate Tornado */
		for (auto& iter = m_TornadoTransforms.begin(); iter != m_TornadoTransforms.end();)
		{
			iter->fLifeTime += fTimeDelta;

			_vector vDirection = { };
			_vector vRightDirection = { };
			switch (iter->eDirection)
			{
			case TORNADO_DESC::DIR_LT:

				vDirection = { -1.f, 0.f, 1.f, 0.f };
				vRightDirection = XMVector3Cross({ 0.f, 1.f, 0.f, 0.f }, vDirection);
				break;
			case TORNADO_DESC::DIR_RT:
				vDirection = { 1.f, 0.f, 1.f, 0.f };
				vRightDirection = XMVector3Cross({ 0.f, 1.f, 0.f, 0.f }, vDirection);
				break;
			case TORNADO_DESC::DIR_LB:
				vDirection = { -1.f, 0.f, -1.f, 0.f };
				vRightDirection = XMVector3Cross({ 0.f, 1.f, 0.f, 0.f }, vDirection);
				break;
			case TORNADO_DESC::DIR_RB:
				vDirection = { 1.f, 0.f, -1.f, 0.f };
				vRightDirection = XMVector3Cross({ 0.f, 1.f, 0.f, 0.f }, vDirection);
				break;
			}

			_vector vPosition = iter->pTransform->Get_State(CTransform::STATE_POSITION);
			vPosition += (XMVector4Normalize(vDirection) + XMVector4Normalize(vRightDirection) * sin(iter->fLifeTime * 3.f) * 3.f) * fTimeDelta * 5.f;

			iter->pTransform->Set_State(CTransform::STATE_POSITION, vPosition);

			if (iter->fLifeTime > TORNADO_DESC::fMaxLifeTime)
			{
				m_pGameInstance->Stop_AssetFX(iter->strFXTag);
				Safe_Release(iter->pTransform);

				iter = m_TornadoTransforms.erase(iter);
			}
			else
				++iter;
		}
	}

	return S_OK;
}

HRESULT CFX_AndriusSavageTornado::LateTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CFX_AndriusSavageTornado::Render()
{
	return S_OK;
}

CFX_AndriusSavageTornado* CFX_AndriusSavageTornado::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFX_AndriusSavageTornado* pInstance = new CFX_AndriusSavageTornado(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CFX_AndriusSavageTornado"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFX_AndriusSavageTornado::Clone(void* pArg)
{
	CFX_AndriusSavageTornado* pInstance = new CFX_AndriusSavageTornado(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CFX_AndriusSavageTornado"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFX_AndriusSavageTornado::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);

	for (auto& iter : m_TornadoTransforms)
	{
		m_pGameInstance->Stop_AssetFX(iter.strFXTag);
		Safe_Release(iter.pTransform);
	}
}
