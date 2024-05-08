#include "stdafx.h"
#include "CommonNPC.h"
#include "Part_Body.h"

#include "GameInstance.h"
#include "Player.h"
#include "InteractionBox.h"
#include "UI_Manager.h"

CCommon_NPC::CCommon_NPC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCreature(pDevice, pContext)
{
}

CCommon_NPC::CCommon_NPC(const CCommon_NPC& rhs)
	: CCreature(rhs)
{
}

HRESULT CCommon_NPC::NPC_Action(_float fTimeDelta)
{
	switch (m_eNPCId)
	{
	case NPC_STANDBY:
		if (FAILED(CCommon_NPC::Standby_Action(fTimeDelta)))
			return S_OK;
		break;
	case NPC_PRAY:
		if (FAILED(CCommon_NPC::Pray_Action(fTimeDelta)))
			return S_OK;
		break;
	case NPC_CLAP:
		if (FAILED(CCommon_NPC::Clap_Action(fTimeDelta)))
			return S_OK;
		break;
	case NPC_SOLIDER:
		if (FAILED(CCommon_NPC::Solider_Action(fTimeDelta)))
			return S_OK;
		break;
	case NPC_SIT:
		if (FAILED(CCommon_NPC::SitHand_Action(fTimeDelta)))
			return S_OK;
		break;
	case NPC_AKIMBO:
		if (FAILED(CCommon_NPC::Akimbo_Action(fTimeDelta)))
			return S_OK;
		break;
	case NPC_HOLDARMHAND:
		if (FAILED(CCommon_NPC::HoldArmHand_Action(fTimeDelta)))
			return S_OK;
		break;
	case NPC_HOLDARM:
		if (FAILED(CCommon_NPC::HoldArm_Action(fTimeDelta)))
			return S_OK;
		break;
	case NPC_TALK1:
		if (FAILED(CCommon_NPC::Talk_Action(fTimeDelta, 1)))
			return S_OK;
	case NPC_TALK2:
		if (FAILED(CCommon_NPC::Talk_Action(fTimeDelta, 2)))
			return S_OK;
	case NPC_TALK3:
		if (FAILED(CCommon_NPC::Talk_Action(fTimeDelta, 3)))
			return S_OK;
		break;
	case NPC_REQUEST:
		if (FAILED(CCommon_NPC::Request_Action(fTimeDelta)))
			return S_OK;
		break;
	case NPC_KNIGHTS:
		if (FAILED(CCommon_NPC::Knight_Action(fTimeDelta)))
			return S_OK;
		break;
	case NPC_WALK_STRAIGHT:
		if (FAILED(CCommon_NPC::Walk_Action(fTimeDelta)))
			return S_OK;
		break;
	}

	return S_OK;
}

HRESULT CCommon_NPC::Standby_Action(_float fTimeDelta)
{
	m_pFiniteStateMachineCom->Change_ForceState(FSM_NPC_IDLE);

	return S_OK;
}

HRESULT CCommon_NPC::Pray_Action(_float fTimeDelta)
{
	m_pFiniteStateMachineCom->Change_ForceState(FSM_NPC_PRAY);

	return S_OK;
}

HRESULT CCommon_NPC::Clap_Action(_float fTimeDelta)
{
	m_pFiniteStateMachineCom->Change_ForceState(FSM_NPC_CLAP);

	return S_OK;
}

HRESULT CCommon_NPC::Solider_Action(_float fTimeDelta)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

	CTransform* pPlayerTransform =
		static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vToPlayer = vPlayerPos - vPosition;
	_float fDistance = XMVector3Length(vToPlayer).m128_f32[0];

	FSM_NPC_STATE eState = static_cast<FSM_NPC_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());

	if (fDistance < 15.f && eState == FSM_NPC_IDLE)
		m_pFiniteStateMachineCom->Change_ForceState(FSM_NPC_SOLIDER_START);
	else if (fDistance > 20.f)
		m_pFiniteStateMachineCom->Change_ForceState(FSM_NPC_SOLIDER_STANDBY);

	return S_OK;
}

HRESULT CCommon_NPC::SitHand_Action(_float fTimeDelta)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

	CTransform* pPlayerTransform =
		static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vToPlayer = vPlayerPos - vPosition;
	_float fDistance = XMVector3Length(vToPlayer).m128_f32[0];

	FSM_NPC_STATE eState = static_cast<FSM_NPC_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());

	if (fDistance < 15.f && eState == FSM_NPC_IDLE)
		m_pFiniteStateMachineCom->Change_ForceState(FSM_NPC_SITHAND_START);

	return S_OK;
}

HRESULT CCommon_NPC::Akimbo_Action(_float fTimeDelta)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

	CTransform* pPlayerTransform =
		static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vToPlayer = vPlayerPos - vPosition;
	_float fDistance = XMVector3Length(vToPlayer).m128_f32[0];

	FSM_NPC_STATE eState = static_cast<FSM_NPC_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());

	if (fDistance < 15.f && eState == FSM_NPC_IDLE)
		m_pFiniteStateMachineCom->Change_ForceState(FSM_NPC_AKIMBO_START);
	else if (fDistance > 20.f)
		m_pFiniteStateMachineCom->Change_ForceState(FSM_NPC_IDLE);

	return S_OK;
}

HRESULT CCommon_NPC::HoldArmHand_Action(_float fTimeDelta)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

	CTransform* pPlayerTransform =
		static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vToPlayer = vPlayerPos - vPosition;
	_float fDistance = XMVector3Length(vToPlayer).m128_f32[0];

	FSM_NPC_STATE eState = static_cast<FSM_NPC_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());

	if (fDistance < 15.f && eState == FSM_NPC_IDLE)
		m_pFiniteStateMachineCom->Change_ForceState(FSM_NPC_HOLDARMHAND_START);
	else if (fDistance > 20.f)
		m_pFiniteStateMachineCom->Change_ForceState(FSM_NPC_IDLE);

	return S_OK;
}

HRESULT CCommon_NPC::HoldArm_Action(_float fTimeDelta)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

	CTransform* pPlayerTransform =
		static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vToPlayer = vPlayerPos - vPosition;
	_float fDistance = XMVector3Length(vToPlayer).m128_f32[0];

	FSM_NPC_STATE eState = static_cast<FSM_NPC_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());

	if (fDistance < 15.f && eState == FSM_NPC_IDLE)
		m_pFiniteStateMachineCom->Change_ForceState(FSM_NPC_HOLDARM_START);
	else if (fDistance > 20.f)
		m_pFiniteStateMachineCom->Change_ForceState(FSM_NPC_IDLE);

	return S_OK;
}

HRESULT CCommon_NPC::Talk_Action(_float fTimeDelta, _int iAnim)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

	CTransform* pPlayerTransform =
		static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vToPlayer = vPlayerPos - vPosition;
	_float fDistance = XMVector3Length(vToPlayer).m128_f32[0];

	FSM_NPC_STATE eState = static_cast<FSM_NPC_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());

	if (fDistance < 15.f && eState == FSM_NPC_IDLE)
	{
		if (iAnim == 1)
			m_pFiniteStateMachineCom->Change_ForceState(FSM_NPC_TALK1);
		else if (iAnim == 2)
			m_pFiniteStateMachineCom->Change_ForceState(FSM_NPC_TALK2);
		else
			m_pFiniteStateMachineCom->Change_ForceState(FSM_NPC_TALK3);
	}
	else if (fDistance > 20.f)
		m_pFiniteStateMachineCom->Change_ForceState(FSM_NPC_IDLE);

	return S_OK;
}

HRESULT CCommon_NPC::Request_Action(_float fTimeDelta)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

	CTransform* pPlayerTransform =
		static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vToPlayer = vPlayerPos - vPosition;
	_float fDistance = XMVector3Length(vToPlayer).m128_f32[0];

	FSM_NPC_STATE eState = static_cast<FSM_NPC_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());

	if (fDistance < 15.f && eState == FSM_NPC_IDLE)
		m_pFiniteStateMachineCom->Change_ForceState(FSM_NPC_REQUEST_START);
	else if (fDistance > 20.f)
		m_pFiniteStateMachineCom->Change_ForceState(FSM_NPC_IDLE);

	return S_OK;
}

HRESULT CCommon_NPC::Knight_Action(_float fTimeDelta)
{
	m_pFiniteStateMachineCom->Change_ForceState(FSM_NPC_SOLIDER_IDLE);

	return S_OK;
}

HRESULT CCommon_NPC::Walk_Action(_float fTimeDelta)
{
	m_fWalkTime += fTimeDelta;
	
	if (m_fWalkTime > 5.f && m_bWalk == FALSE)
		m_bWalk = TRUE;

	if (m_bWalk)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		if (m_fWalkTime < 35.f)
		{
			_vector vSetDir = vPos - m_vInitPos;
			vSetDir.m128_f32[1] = 0.f;  vSetDir = XMVector4Normalize(vSetDir);

			m_pTransformCom->Reset_Look_At_Direction();
			m_pTransformCom->Set_Look_At_Direction(vSetDir);
			m_pTransformCom->Go_Straight(fTimeDelta * 0.18f, vSetDir);

			m_pFiniteStateMachineCom->Change_ForceState(FSM_NPC_WALK);
		}
		else if(m_fWalkTime >= 40.f)
		{
			_vector vSetDir = m_vInitPos - vPos;
			vSetDir.m128_f32[1] = 0.f;	
			_float fDistance = XMVector3Length(vSetDir).m128_f32[0];
			vSetDir = XMVector4Normalize(vSetDir);

			m_pTransformCom->Reset_Look_At_Direction();
			m_pTransformCom->Set_Look_At_Direction(vSetDir);
			m_pTransformCom->Move_To_Dir(vSetDir, fTimeDelta * 0.18f, 0.f);

			m_pFiniteStateMachineCom->Change_ForceState(FSM_NPC_WALK);

			if (m_bWalk == TRUE && fDistance <= 0.1f)
			{
				m_bWalk = FALSE;
				m_fWalkTime = 0.f;
				m_pFiniteStateMachineCom->Change_ForceState(FSM_NPC_IDLE);
			}
		}
		else
			m_pFiniteStateMachineCom->Change_State(FSM_NPC_IDLE);
	}

	return S_OK;
}

HRESULT CCommon_NPC::Add_Parts(void* pArg)
{
	if (pArg != nullptr)
	{
		NPC_DESC NPCDesc = *(static_cast<NPC_DESC*>(pArg));

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, NPCDesc.vInitPos);
		m_pTransformCom->Rotation({ 0.f,1.f,0.f,0.f }, XMConvertToRadians(NPCDesc.fAngle));

		CPartObject* pPartObject = { };
		PARTY_SLOTS	 tSlots = { };

		/* For. Part_Body */
		CPart_Body::PARTBODY_DESC tBodyDesc = { };
		tBodyDesc.tPartObjDesc.pParent = this;
		tBodyDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
		tBodyDesc.tCreatureID = NPCDesc.eObjectType;
		tBodyDesc.eType = CTY_NPC;

		CREATUREID eCreatureID = NPCDesc.eObjectType;
		pPartObject = CPart_Body::Create_Body(eCreatureID, tBodyDesc);
		if (pPartObject == nullptr)
			return E_FAIL;
		static_cast<CPart_Body*>(pPartObject)->Set_CreatureID(eCreatureID);

		tSlots.pPartySlot = pPartObject;
		m_Parts.push_back(tSlots);

		m_eNPCId = NPCDesc.eNPCType;
		m_eCreatureID = NPCDesc.eObjectType;
		m_vInitPos = NPCDesc.vInitPos;
		m_vInitPos.m128_f32[2] -= 0.1f;
	}
	
	switch (m_eCreatureID)
	{
	case NPC_SARA:
	{
		m_pInteractionBox = (CInteraction_Box*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Interaction_Box"))->Clone(nullptr);
		m_pInteractionBox->Add_InteractionBox(TEXT("사라"), CInteraction_Box::ICON_TALK);

		m_strNPCName = TEXT("사라");
		m_strNPCDetail = TEXT("<디어 헌터 웨이팅 스태프>");

		COrtho::ORTHO_DESC tOrthoDesc = { };
		tOrthoDesc.eTextureLevel = LEVEL_STATIC;
		tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Interaction_Icons");
		tOrthoDesc.fPosition = { 0.f,0.f };
		tOrthoDesc.fSize = { 65.f, 68.f };
		tOrthoDesc.ePage = PAGE_MAIN;
		tOrthoDesc.iDepth = 0;
		m_pOverHeadUI = (CAlphaRect*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_AlphaRect"))->Clone(&tOrthoDesc);
		m_pOverHeadUI->Set_Text(m_strNPCName, CAlphaRect::TEXT_DOWN);
		m_pOverHeadUI->Set_Detail_Text(m_strNPCDetail);
		m_pOverHeadUI->SetTextureUV(_float2{ 529.f / 1024.f,118.f / 1024.f }, _float2{ 50.f / 1024.f,45.f / 1024.f });

		break;
	}
	case NPC_BLANCHE:
	{
		m_pInteractionBox = (CInteraction_Box*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Interaction_Box"))->Clone(nullptr);
		m_pInteractionBox->Add_InteractionBox(TEXT("블랑키"), CInteraction_Box::ICON_TALK);

		m_strNPCName = TEXT("블랑키");
		m_strNPCDetail = TEXT("<몬드 잡화점 점원>");

		COrtho::ORTHO_DESC tOrthoDesc = { };
		tOrthoDesc.eTextureLevel = LEVEL_STATIC;
		tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Interaction_Icons");
		tOrthoDesc.fPosition = { 0.f,0.f };
		tOrthoDesc.fSize = { 65.f, 68.f };
		tOrthoDesc.ePage = PAGE_MAIN;
		tOrthoDesc.iDepth = 0;
		m_pOverHeadUI = (CAlphaRect*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_AlphaRect"))->Clone(&tOrthoDesc);
		m_pOverHeadUI->Set_Text(m_strNPCName, CAlphaRect::TEXT_DOWN);
		m_pOverHeadUI->Set_Detail_Text(m_strNPCDetail);
		m_pOverHeadUI->SetTextureUV(_float2{ 363.f / 1024.f,53.f / 1024.f }, _float2{ 42.f / 1024.f,48.f / 1024.f });
	}
	break;
	case NPC_TIMAEUS:
	{
		m_pInteractionBox = (CInteraction_Box*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Interaction_Box"))->Clone(nullptr);
		m_pInteractionBox->Add_InteractionBox(TEXT("데마로우스"), CInteraction_Box::ICON_TALK);

		m_strNPCName = TEXT("데마로우스");
		m_strNPCDetail = TEXT("<연금술사>");

		COrtho::ORTHO_DESC tOrthoDesc = { };
		tOrthoDesc.eTextureLevel = LEVEL_STATIC;
		tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Interaction_Icons");
		tOrthoDesc.fPosition = { 0.f,0.f };
		tOrthoDesc.fSize = { 65.f, 68.f };
		tOrthoDesc.ePage = PAGE_MAIN;
		tOrthoDesc.iDepth = 0;
		m_pOverHeadUI = (CAlphaRect*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_AlphaRect"))->Clone(&tOrthoDesc);
		m_pOverHeadUI->Set_Text(m_strNPCName, CAlphaRect::TEXT_DOWN);
		m_pOverHeadUI->Set_Detail_Text(m_strNPCDetail);
		m_pOverHeadUI->SetTextureUV(_float2{ 194.f / 1024.f,271.f / 1024.f }, _float2{ 52.f / 1024.f,48.f / 1024.f });
	}
	break;
	}

	return S_OK;
}

HRESULT CCommon_NPC::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCommon_NPC::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (FAILED(Add_Parts(pArg)))
		return E_FAIL;

	m_pFiniteStateMachineCom->Change_State(FSM_NPC_IDLE);

	return S_OK;
}

HRESULT CCommon_NPC::PreviousTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (FAILED(__super::PreviousTick(fTimeDelta)))
			return E_FAIL;

		for (auto& pPart : m_Parts)
		{
			pPart.pPartySlot->PreviousTick(fTimeDelta);
		}

		if (m_pOverHeadUI != nullptr && m_pInteractionBox != nullptr)
		{
			m_pOverHeadUI->PreviousTick(fTimeDelta);
			m_pInteractionBox->PreviousTick(fTimeDelta);
		}
	}

	return S_OK;
}

HRESULT CCommon_NPC::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (FAILED(__super::Tick(fTimeDelta)))
			return E_FAIL;
		
		if (FAILED(CCommon_NPC::NPC_Action(fTimeDelta)))
			return E_FAIL;

		for (auto& pPart : m_Parts)
		{
			pPart.pPartySlot->Tick(fTimeDelta);
		}

		//플레이어 위치 찾아서 3.f 안에 들어오면 상호작용 가능
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
		_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

		CTransform* pPlayerTransform =
			static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

		_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vToPlayer = vPlayerPos - vPosition;
		XMVector3Length(vToPlayer);
		XMVector3Normalize(vToPlayer);


		if (m_pOverHeadUI != nullptr)
		{

		_float2 vPosition2D;
		_vector vPos;
		_matrix ProjMatrix, ViewMatrix, WorldMatrix;
		ViewMatrix = m_pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW);
		ProjMatrix = m_pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJ);
		vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPos.m128_f32[1] += 2.4f;

		WorldMatrix = XMMatrixIdentity();

		XMStoreFloat2(&vPosition2D, XMVector3Project(vPos,
			0, 0, g_iWinSizeX, g_iWinSizeY, 0, 1, ProjMatrix, ViewMatrix, WorldMatrix));

		// 월드 좌표를 정규화된 장치 좌표로 변환
		XMMATRIX worldViewProjectionMatrix = WorldMatrix * ViewMatrix * ProjMatrix;
		XMFLOAT4 clipCoord;
		XMStoreFloat4(&clipCoord, XMVector3TransformCoord(vPos, worldViewProjectionMatrix));

		// 정규화된 장치 좌표를 화면 좌표로 변환
		float screenX = (clipCoord.x + 1.0f) * 0.5f * 1600.0f;
		float screenY = (1.0f - clipCoord.y) * 0.5f * 900.0f;

		//-400 = Height
		m_pOverHeadUI->Update_Matrix(screenX - g_iWinSizeX / 2, -(screenY - g_iWinSizeY / 2), 50.f, 50.f);
		}

		if (XMVector3Length(vToPlayer).m128_f32[0] <= 3.f)
		{
			if (m_pOverHeadUI != nullptr && m_pInteractionBox != nullptr)
			{
				m_pOverHeadUI->Tick(fTimeDelta);
				m_pInteractionBox->Tick(fTimeDelta);
			}

			if (m_pGameInstance->Key_Down('F'))
			{
				switch (m_eCreatureID)
				{
				case NPC_SARA:
				{
					m_pGameInstance->PlaySoundW(L"InteractionBox.mp3", 0.7f);
					CUI_Manager* UI_Manager = (CUI_Manager*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager"));
					UI_Manager->Show_Page(PAGE_COOK);
				}
					break;
				case NPC_BLANCHE:
				{
					m_pGameInstance->PlaySoundW(L"InteractionBox.mp3", 0.7f);
					CUI_Manager* UI_Manager = (CUI_Manager*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager"));
					UI_Manager->Show_Page(PAGE_SHOP);
				}
					break;
				case NPC_TIMAEUS:
				{
					m_pGameInstance->PlaySoundW(L"InteractionBox.mp3", 0.7f);
					CUI_Manager* UI_Manager = (CUI_Manager*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager"));
					UI_Manager->Show_Page(PAGE_ALCHEMY);
				}
				break;
				}
			}
			
		}
	}

	return S_OK;
}

HRESULT CCommon_NPC::LateTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_RenderValidity() == S_OK)
	{
		if (FAILED(__super::LateTick(fTimeDelta)))
			return E_FAIL;

		for (auto& pPart : m_Parts)
		{
			pPart.pPartySlot->LateTick(fTimeDelta);
		}

		//플레이어와 가까울 때만 LateTick
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
			if (m_pOverHeadUI != nullptr && m_pInteractionBox != nullptr)
			{
				m_pOverHeadUI->LateTick(fTimeDelta);
				m_pInteractionBox->LateTick(fTimeDelta);
			}
		}
	}

	return S_OK;
}

HRESULT CCommon_NPC::Render()
{
	return S_OK;
}

CCommon_NPC* CCommon_NPC::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCommon_NPC* pInstance = new CCommon_NPC(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CCommon_NPC"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCommon_NPC::Clone(void* pArg)
{
	CCommon_NPC* pInstance = new CCommon_NPC(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CCommon_NPC"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCommon_NPC::Free()
{
	__super::Free();
	
	for (auto& pPart : m_Parts)
	{
		pPart.pPartySlot->Release_ColliderID();
		Safe_Release(pPart.pPartySlot);
	}
	m_Parts.clear();

	Safe_Release(m_pInteractionBox);
	Safe_Release(m_pOverHeadUI);
}