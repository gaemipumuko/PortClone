#include "stdafx.h"
#include "Updraft.h"
#include "Effect.h"
#include "Player.h"
#include "CharacterController.h"
#include "GameInstance.h"

CUpdraft::CUpdraft(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEventCollider(pDevice, pContext)
{
}

CUpdraft::CUpdraft(const CUpdraft& rhs)
	: CEventCollider(rhs)
{
}

HRESULT CUpdraft::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUpdraft::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (pArg == nullptr)
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;


	UPDRAFT_DESC* pDesc = (UPDRAFT_DESC*)pArg;
	if (pDesc->fLifeTime <= 0)
		m_bLoop = TRUE;
	else
		m_fLifeTime = pDesc->fLifeTime;

	m_pTransformCom->RotationFromCurrentState(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(pDesc->vRadians.x));
	m_pTransformCom->RotationFromCurrentState(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(pDesc->vRadians.y));
	m_pTransformCom->RotationFromCurrentState(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(pDesc->vRadians.z));

	/* For.Com_Collider_OBB */
	CBounding_OBB::BOUNDING_OBB_DESC CollDesc;
	CollDesc.vExtents = pDesc->vColliderSize;
	CollDesc.vCenter = _float3(0.f, CollDesc.vExtents.y, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pColliderCom, &CollDesc)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pDesc->vPosition));
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	CEffect::EFFECT_DESC descc = {};
	_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	/*vSelfPos.m128_f32[2] -= 1.f;*/
	descc.vSpawnPos = vSelfPos;
	descc.vScale = XMVectorSet(pDesc->vColliderSize.x, pDesc->vColliderSize.y, pDesc->vColliderSize.z, 0.f);
	descc.vRadians = pDesc->vRadians;
	m_pWindField = (CEff_WindField*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Effect_Wind_Field"))->Clone(&descc);


	return S_OK;
}

HRESULT CUpdraft::PreviousTick(_float fTimeDelta)
{
	m_pWindField->PreviousTick(fTimeDelta);

	return S_OK;
}

HRESULT CUpdraft::Tick(_float fTimeDelta)
{
//#ifdef _DEBUG
//	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
//#endif

	if (Get_Destroy() == TRUE)
		return S_OK;

	if (m_bLoop == FALSE)
	{
		m_fLifeTime -= fTimeDelta;
		if (m_fLifeTime <= 0.f)
			Set_Destroy(TRUE);
	}

	vector<CGameObject*> isColl = m_pGameInstance->Find_Collision_Objects(m_pColliderCom, COLL_EVENTREACT_PLAYER);
	if (isColl.empty() == FALSE)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
		if (pPlayer == nullptr)
			return E_FAIL;
		FSM_STATE eState = pPlayer->Get_State();

		if (eState >= FSM_FLY_START && eState <= FSM_FLY_RIGHT) // 날고 있으면
		{
			_float fSpeed = 10.f;
			if (m_pGameInstance->Get_CurrentLevel() == LEVEL_STORMTERROR)
				fSpeed = 5.f;

			if (m_fUpSpeed < fSpeed)
				m_fUpSpeed += fTimeDelta * fSpeed;

			//pPlayer->Get_Controller()->Movement(fTimeDelta, _float4(0.f, 1.f, 0.f, 1.f), m_fUpSpeed); // 위로 상승
			_float4 vUp = {};
			XMStoreFloat4(&vUp, m_pTransformCom->Get_State(CTransform::STATE_UP));
			pPlayer->Get_Controller()->Movement(fTimeDelta, vUp, m_fUpSpeed);
			
			pPlayer->Set_Gravity(FALSE);
		}
		else
		{
			/*pPlayer->Get_Controller()->Movement(fTimeDelta, _float4(0.f, 1.f, 0.f, 1.f), 0.f);
			pPlayer->Set_Gravity();*/
			m_fUpSpeed = 0.f;
		}
	}
	else
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
		pPlayer->Set_Gravity();
		m_fUpSpeed = 0.f;
	}

	m_pWindField->Tick(fTimeDelta);

	return S_OK;
}

HRESULT CUpdraft::LateTick(_float fTimeDelta)
{
	m_pWindField->LateTick(fTimeDelta);

	return S_OK;
}

HRESULT CUpdraft::Render()
{
	return S_OK;
}

CUpdraft* CUpdraft::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUpdraft* pInstance = new CUpdraft(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CUpdraft"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUpdraft::Clone(void* pArg)
{
	CUpdraft* pInstance = new CUpdraft(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CUpdraft"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUpdraft::Free()
{
	__super::Free();

	Safe_Release(m_pWindField);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
