#include "stdafx.h"
#include "TelePort.h"
#include "Part_Body.h"

#include "GameInstance.h"
#include "Player.h"
#include "InteractionBox.h"

vector<_bool>       CTelePort::m_vecInteraction(TELEPORTID::TELEPORT_END);

CTelePort::CTelePort(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCreature(pDevice, pContext)
{
}

CTelePort::CTelePort(const CTelePort& rhs)
	: CCreature(rhs)
{
}

HRESULT CTelePort::Add_Parts(void* pArg)
{
	CPartObject* pPartObject = { };
	PARTY_SLOTS	 tSlots = { };

	if (pArg != nullptr)
	{
		TELEPORT_DESC TelePortDesc = *(static_cast<TELEPORT_DESC*>(pArg));

		m_pTransformCom->Set_Scale({ 0.01f, 0.01f, 0.01f });
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, TelePortDesc.vInitPos);
		m_eTelePortId = TelePortDesc.eTelePortId;

		/* For. Part_Body */
		CPart_Body::PARTBODY_DESC tBodyDesc = { };
		tBodyDesc.tPartObjDesc.pParent = this;
		tBodyDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
		tBodyDesc.tCreatureID = TELEPORT;
		tBodyDesc.eType = CTY_NPC;

		CREATUREID eCreatureID = TELEPORT;
		pPartObject = CPart_Body::Create_Body(eCreatureID, tBodyDesc);
		if (pPartObject == nullptr)
			return E_FAIL;
		static_cast<CPart_Body*>(pPartObject)->Set_CreatureID(eCreatureID);

		tSlots.pPartySlot = pPartObject;
		m_Parts.push_back(tSlots);
	}

	return S_OK;
}

HRESULT CTelePort::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTelePort::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (FAILED(Add_Parts(pArg)))
		return E_FAIL;

	m_pFiniteStateMachineCom->Change_State(FSM_TELEPORT_CLOSE);
	
	m_pInteractionBox = (CInteraction_Box*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Interaction_Box"))->Clone(nullptr);
	m_pInteractionBox->Add_InteractionBox(TEXT("워프 포인트"), CInteraction_Box::ICON_TALK);
	
	if (m_vecInteraction[m_eTelePortId] == TRUE)
	{
		m_pFiniteStateMachineCom->Change_ForceState(FSM_TELEPORT_IDLE);
	}

	return S_OK;
}

HRESULT CTelePort::PreviousTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (FAILED(__super::PreviousTick(fTimeDelta)))
			return E_FAIL;

		for (auto& pPart : m_Parts)
		{
			pPart.pPartySlot->PreviousTick(fTimeDelta);
		}

		m_pInteractionBox->PreviousTick(fTimeDelta);
	}

	return S_OK;
}

HRESULT CTelePort::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		FSM_TELEPORT_STATE eState = static_cast<FSM_TELEPORT_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());
		
		if (FAILED(__super::Tick(fTimeDelta)))
			return E_FAIL;
		
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
		_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

		CTransform* pPlayerTransform =
			static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

		_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vToPlayer = vPlayerPos - vPosition;
		XMVector3Length(vToPlayer);
		XMVector3Normalize(vToPlayer);

		if (XMVector3Length(vToPlayer).m128_f32[0] <= 3.f)
		{
			m_pInteractionBox->Tick(fTimeDelta);

			if (m_pGameInstance->Key_Down('F') && eState == FSM_TELEPORT_CLOSE)
			{
				m_pFiniteStateMachineCom->Change_State(FSM_TELEPORT_OPEN);
				m_vecInteraction[m_eTelePortId] = TRUE;
				m_pGameInstance->PlaySoundW(L"WarpPoint_Activate.mp3", 3.f, CHANNELID::SOUND_ANNOUNCE);
			}
			else if (m_pGameInstance->Key_Down('F') && eState == FSM_TELEPORT_IDLE)
			{
				pPlayer->Set_MaxHp();
				pPlayer->Set_MaxEnergy();
			}
		}

		for (auto& pPart : m_Parts)
		{
			pPart.pPartySlot->Tick(fTimeDelta);
		}
	}

	return S_OK;
}

HRESULT CTelePort::LateTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_RenderValidity() == S_OK)
	{
		if (FAILED(__super::LateTick(fTimeDelta)))
			return E_FAIL;

		for (auto& pPart : m_Parts)
		{
			pPart.pPartySlot->LateTick(fTimeDelta);
		}

		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
		_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

		CTransform* pPlayerTransform =
			static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

		_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vToPlayer = vPlayerPos - vPosition;

		XMVector3Length(vToPlayer);
		XMVector3Normalize(vToPlayer);
		if (XMVector3Length(vToPlayer).m128_f32[0] <= 3.f)
		{
			m_pInteractionBox->LateTick(fTimeDelta);
		}
	}

	return S_OK;
}

HRESULT CTelePort::Render()
{
	return S_OK;
}

CTelePort* CTelePort::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTelePort* pInstance = new CTelePort(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CTelePort"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTelePort::Clone(void* pArg)
{
	CTelePort* pInstance = new CTelePort(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CTelePort"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTelePort::Free()
{
	__super::Free();
	
	for (auto& pPart : m_Parts)
	{
		pPart.pPartySlot->Release_ColliderID();
		Safe_Release(pPart.pPartySlot);
	}
	m_Parts.clear();

	Safe_Release(m_pInteractionBox);

}