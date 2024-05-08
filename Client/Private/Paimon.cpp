#include "stdafx.h"
#include "Paimon.h"
#include "Part_Body.h"

#include "GameInstance.h"
#include "Player.h"

CPaimon::CPaimon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCreature(pDevice, pContext)
{
}

CPaimon::CPaimon(const CPaimon& rhs)
	: CCreature(rhs)
{
}

HRESULT CPaimon::Add_Parts(void* pArg)
{
	CPartObject* pPartObject = { };
	PARTY_SLOTS	 tSlots = { };

	/* For. Part_Body */
	CPart_Body::PARTBODY_DESC tBodyDesc = { };
	tBodyDesc.tPartObjDesc.pParent = this;
	tBodyDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
	tBodyDesc.tCreatureID = NPC_PAIMON;
	tBodyDesc.eType = CTY_NPC;

	CREATUREID eCreatureID = NPC_PAIMON;
	pPartObject = CPart_Body::Create_Body(eCreatureID, tBodyDesc);
	if (pPartObject == nullptr)
		return E_FAIL;
	static_cast<CPart_Body*>(pPartObject)->Set_CreatureID(eCreatureID);

	tSlots.pPartySlot = pPartObject;
	m_Parts.push_back(tSlots);

	return S_OK;
}

HRESULT CPaimon::Follow_Player(_float fTimeDelta)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

	CTransform* pPlayerTransform =
		static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerRight = pPlayerTransform->Get_State(CTransform::STATE_RIGHT);
	vPlayerRight.m128_f32[1] = 0.f; XMVector3Normalize(vPlayerRight);
	_vector vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);
	vPlayerLook.m128_f32[1] = 0.f; XMVector3Normalize(vPlayerLook);

	_vector vSetDir = vPlayerPos - vPos;

	_float fDistance = XMVector3Length(vSetDir).m128_f32[0];
	vSetDir.m128_f32[0]; vSetDir = XMVector3Normalize(vSetDir);

	m_pTransformCom->Reset_Look_At_Direction();
	m_pTransformCom->Set_Look_At_Direction(vSetDir);

	CFiniteStateMachine* pPlayerFSM = static_cast<CFiniteStateMachine*>(pPlayer->Get_PlayerInfo(iIndex).pFiniteStateMachineCom);
	FSM_STATE ePlayerState = static_cast<FSM_STATE>(pPlayerFSM->Get_CurrentState());
	FSM_PAIMON_STATE eState = static_cast<FSM_PAIMON_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());

	vPlayerPos += vPlayerRight * 0.5f;
	vPlayerPos -= vPlayerLook * 0.5f;
	vPlayerPos.m128_f32[1] += 1.f;

	if (eState != FSM_PAIMON_CLAP && eState != FSM_PAIMON_GUIDE && eState != FSM_PAIMON_IDLE3)
	{
		if (ePlayerState == FSM_SPLINT || ePlayerState == FSM_DASH)
		{
			/*m_pTransformCom->Move_To(vPlayerPos, fTimeDelta * 2.3f, 1.f);*/
			m_pFiniteStateMachineCom->Change_State(FSM_PAIMON_FLY);
			vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			//vPos.m128_f32[1] = vPlayerPos.m128_f32[1] + 1.2f;
			//vPos.m128_f32[0] = vPlayerPos.m128_f32[0] + 0.5f;
			//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
			float smoothingFactor = 5.f;
			_vector vSmoothedPos = XMVectorLerp(vPos, vPlayerPos, 1.0f - expf(-fTimeDelta * smoothingFactor));
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vSmoothedPos);
		}
		else if (ePlayerState == FSM_RUN)
		{
			//m_pTransformCom->Move_To(vPlayerPos, fTimeDelta * 1.f, 1.f);
			m_pFiniteStateMachineCom->Change_State(FSM_PAIMON_FLY);
			vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			//vPos.m128_f32[1] = vPlayerPos.m128_f32[1] + 1.2f;
			//vPos.m128_f32[0] = vPlayerPos.m128_f32[0] + 0.5f;
			//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
			float smoothingFactor = 5.f;
			_vector vSmoothedPos = XMVectorLerp(vPos, vPlayerPos, 1.0f - expf(-fTimeDelta * smoothingFactor));
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vSmoothedPos);
		}
		else if (ePlayerState == FSM_FALL_ATTACK || ePlayerState == FSM_DROP_DOWN)
		{
			//m_pTransformCom->Move_To(vPlayerPos, fTimeDelta * 0.5f, 1.f);
			m_pFiniteStateMachineCom->Change_State(FSM_PAIMON_FLY);
			vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			//vPos.m128_f32[1] = vPlayerPos.m128_f32[1] + 1.2f;
			//vPos.m128_f32[0] = vPlayerPos.m128_f32[0] + 0.5f;
			//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
			float smoothingFactor = 15.f;
			_vector vSmoothedPos = XMVectorLerp(vPos, vPlayerPos, 1.0f - expf(-fTimeDelta * smoothingFactor));
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos);
		}
		else
		{
			//m_pTransformCom->Move_To(vPlayerPos, fTimeDelta * 0.5f, 1.f);
			m_pFiniteStateMachineCom->Change_State(FSM_PAIMON_FLY);
			vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			//vPos.m128_f32[1] = vPlayerPos.m128_f32[1] + 1.2f;
			//vPos.m128_f32[0] = vPlayerPos.m128_f32[0] + 0.5f;
			//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
			float smoothingFactor = 5.f;
			_vector vSmoothedPos = XMVectorLerp(vPos, vPlayerPos, 1.0f - expf(-fTimeDelta * smoothingFactor));
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vSmoothedPos);
		}
	}

	m_pTransformCom->Reset_Look_At_Direction();
	m_pTransformCom->Set_Look_At_Direction(vPlayerLook);

	return S_OK;
}

HRESULT CPaimon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPaimon::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (FAILED(Add_Parts(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale({ 0.8f,0.8f,0.8f,0.f });

	CTransform* pPlayerTransform
		= static_cast<CTransform*>(m_pGameInstance->
			Find_Layer_Component(LEVEL_WORLD, TEXT("Layer_Player"), TEXT("Com_Transform")));
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	vPlayerPos.m128_f32[2] -= 1.5f; vPlayerPos.m128_f32[1] = 1.2f; 
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos);

	return S_OK;
}

HRESULT CPaimon::PreviousTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (FAILED(__super::PreviousTick(fTimeDelta)))
			return E_FAIL;

		if (FAILED(CPaimon::Follow_Player(fTimeDelta)))
			return E_FAIL;

		for (auto& pPart : m_Parts)
		{
			pPart.pPartySlot->PreviousTick(fTimeDelta);
		}
	}

	return S_OK;
}

HRESULT CPaimon::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (FAILED(__super::Tick(fTimeDelta)))
			return E_FAIL;
		
		for (auto& pPart : m_Parts)
		{
			pPart.pPartySlot->Tick(fTimeDelta);
		}
	}

	return S_OK;
}

HRESULT CPaimon::LateTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_RenderValidity() == S_OK)
	{
		if (FAILED(__super::LateTick(fTimeDelta)))
			return E_FAIL;

		for (auto& pPart : m_Parts)
		{
			pPart.pPartySlot->LateTick(fTimeDelta);
		}
	}

	return S_OK;
}

HRESULT CPaimon::Render()
{
	return S_OK;
}

CPaimon* CPaimon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPaimon* pInstance = new CPaimon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CPaimon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPaimon::Clone(void* pArg)
{
	CPaimon* pInstance = new CPaimon(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CPaimon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPaimon::Free()
{
	__super::Free();
	
	for (auto& pPart : m_Parts)
	{
		pPart.pPartySlot->Release_ColliderID();
		Safe_Release(pPart.pPartySlot);
	}
	m_Parts.clear();
}