#include "stdafx.h"
#include "Slime_Small.h"
#include "Part_Body.h"

#include "GameInstance.h"
#include "Player.h"

//UI
#include "Hp_Bar.h"
#include "UI_Manager.h"
#include "Page_Main.h"
#include "CharacterController.h"

#include "Effect.h"

CSlime_Small::CSlime_Small(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCreature(pDevice, pContext)
{
}

CSlime_Small::CSlime_Small(const CSlime_Small& rhs)
	: CCreature(rhs)
{
}

HRESULT CSlime_Small::Add_Parts(void* pArg)
{
	CPartObject* pPartObject = { };
	PARTY_SLOTS	 tSlots = { };

	/* For. Part_Body */
	CPart_Body::PARTBODY_DESC tBodyDesc = { };
	tBodyDesc.tPartObjDesc.pParent = this;
	tBodyDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
	tBodyDesc.tCreatureID = MONSTER_SLIME_WATER;
	tBodyDesc.pController = m_pController;
	tBodyDesc.eType = CTY_MONSTER;

	CREATUREID eCreatureID = MONSTER_SLIME_WATER;
	pPartObject = CPart_Body::Create_Body(eCreatureID, tBodyDesc);
	if (pPartObject == nullptr)
		return E_FAIL;
	static_cast<CPart_Body*>(pPartObject)->Set_CreatureID(eCreatureID);

	tSlots.pPartySlot = pPartObject;
	m_Parts.push_back(tSlots);

	return S_OK;
}

HRESULT CSlime_Small::Pattern(_float fTimeDelta)
{
	_bool bPerification = m_Parts[PART_BODY].pPartySlot->Get_Perification();
	if (bPerification == TRUE)
		return S_OK;

	FSM_SLIME_STATE eState = static_cast<FSM_SLIME_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());
	if (eState == FSM_SLIME_HIT_H || eState == FSM_SLIME_HIT_L || eState == FSM_SLIME_DIE1 || eState == FSM_SLIME_DIE2
		|| eState == FSM_SLIME_HIT_THROW_START || eState == FSM_SLIME_HIT_THROW || eState == FSM_SLIME_HIT_THROW_DOWN)
		return S_OK;

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector vPlayerPos; _vector vSetDir;
	_float fDistance;

	if (pPlayer->Get_CombatMode() == FALSE) /* Player 1명만 온필드 */
	{
		_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

		CTransform* pPlayerTransform =
			static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

		vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

		vSetDir = vPlayerPos - vPos;
		fDistance = XMVector3Length(vSetDir).m128_f32[0];
		vSetDir.m128_f32[1] = 0.f; vSetDir = XMVector3Normalize(vSetDir);
	}
	else /* Player 전체 */
	{
		_vector vNearPlayerPos; _vector vNearSetDir;
		_float  fNearDistance;
		for (_uint Index = 0; Index < pPlayer->Get_TotalPlayer(); Index++)
		{
			CTransform* pPlayerTransform = pPlayer->Get_PlayerInfo(Index).pTransform;
			vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

			vSetDir = vPlayerPos - vPos;
			fDistance = XMVector3Length(vSetDir).m128_f32[0];
			vSetDir.m128_f32[1] = 0.f; vSetDir = XMVector3Normalize(vSetDir);

			if (Index == 0)
			{
				vNearPlayerPos = vPlayerPos;
				vNearSetDir = vSetDir;
				fNearDistance = fDistance;
			}
			else if (fDistance <= fNearDistance)
			{
				vNearPlayerPos = vPlayerPos;
				vNearSetDir = vSetDir;
				fNearDistance = fDistance;
			}
		}

		vPlayerPos = vNearPlayerPos;
		vSetDir = vNearSetDir;
		fDistance = fNearDistance;
	}

	CSlime_Small::Return();

	if (m_bReturn /*&& !m_bWalk*/) // 원래 자리로 돌아감
	{
		_vector vSetDir = m_vInitPos - vPos;
		vSetDir.m128_f32[1] = 0.f;
		vSetDir = XMVector3Normalize(vSetDir);

		_float4 vDirection;
		XMStoreFloat4(&vDirection, vSetDir);

		m_pController->Movement(fTimeDelta, vDirection);

		m_pTransformCom->Reset_Look_At_Direction();
		m_pTransformCom->Set_Look_At_Direction(vSetDir);

		//m_pTransformCom->Move_To(m_vInitPos, fTimeDelta * 0.5f, 0.f);

		m_fAttackTime = 0.f;

		m_bInitWalk = FALSE;
		m_bWalk = FALSE;
		m_fWalkTime = 0.f;

		return S_OK;
	}

	if (fDistance < 10.f && fDistance > 3.f)
	{
		if (m_bPlayerLook == FALSE)
			m_bPlayerLook = TRUE;
		
		m_pFiniteStateMachineCom->Change_State(FSM_SLIME_TURN);
		m_pTransformCom->Reset_Look_At_Direction();
		m_pTransformCom->Set_Look_At_Direction(vSetDir);
		_float4 fDir; XMStoreFloat4(&fDir, vSetDir);
		m_pController->Movement(fTimeDelta, fDir);
		//m_pTransformCom->Move_To(vPlayerPos, fTimeDelta * 0.5f, 2.f);

		m_ePrevState = static_cast<FSM_SLIME_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());

		m_bInitWalk = FALSE;
		m_bWalk = FALSE;
		m_fWalkTime = 0.f;
		//m_fAttackTime = 0.f;
	}
	else if (fDistance <= 3.f)
	{
		m_fAttackTime += fTimeDelta;

		m_pTransformCom->Reset_Look_At_Direction();
		m_pTransformCom->Set_Look_At_Direction(vSetDir);

		if (m_ePrevState == FSM_SLIME_TURN
			&& m_fAttackTime >= 3.f)
		{
			m_pFiniteStateMachineCom->Change_ForceState(FSM_SLIME_ATTACK);
			m_ePrevState = static_cast<FSM_SLIME_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());
			
		}
		else if (m_ePrevState == FSM_SLIME_ATTACK
			&& static_cast<FSM_SLIME_STATE>(m_pFiniteStateMachineCom->Get_CurrentState() == FSM_SLIME_TURN))
		{
			m_ePrevState = static_cast<FSM_SLIME_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());
			m_fAttackTime = 0.f;
		}

		m_bInitWalk = FALSE;
		m_bWalk = FALSE;
		m_fWalkTime = 0.f;
	}
	else
	{
		m_fAttackTime = 0.f;
		m_fWalkTime += fTimeDelta;

		if (m_fWalkTime > 5.f && m_bWalk == FALSE)
		{
			m_bWalk = TRUE;
		}
		if (m_bWalk)
		{
			_vector vSetDir = vPos - m_vInitPos;
			vSetDir.m128_f32[1] = 0.f;
			_float fDistance = XMVector3Length(vSetDir).m128_f32[0];

			if (m_bInitWalk && fDistance <= 0.01f)
			{
				m_bInitWalk = FALSE;
				m_bWalk = FALSE;
				m_fWalkTime = 0.f;
				return S_OK;
			}

			if (m_fWalkTime > 10.f)
			{
				vSetDir = m_vInitPos - vPos;
				m_bInitWalk = TRUE;
			}
			
			m_pTransformCom->Reset_Look_At_Direction();
			m_pTransformCom->Set_Look_At_Direction(vSetDir);
			vSetDir.m128_f32[1] = 0.f; vSetDir = XMVector3Normalize(vSetDir);
			
			_float4 vDirection;
			XMStoreFloat4(&vDirection, vSetDir);
		
			//if (m_bInitWalk)
			//	m_pTransformCom->Move_To_Dir(vSetDir, fTimeDelta * 0.15f, 0.f);
			//else
			//	m_pTransformCom->Go_Straight(fTimeDelta * 0.15f, vSetDir);

			m_pController->Movement(fTimeDelta, vDirection);

			m_pFiniteStateMachineCom->Change_State(FSM_SLIME_WALK);
		}
		else
			m_pFiniteStateMachineCom->Change_State(FSM_SLIME_IDLE);
	}
	
	if (m_bPlayerLook)
	{
		// 플레이어 처음발견했을때 UI 떠야함
	}

	return S_OK;
}

HRESULT CSlime_Small::Return()
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vSetDir = vPos - m_vInitPos;
	vSetDir.m128_f32[1] = 0.f;
	_float fDistance = XMVector3Length(vSetDir).m128_f32[0];

	if (fDistance > 10.f)
	{
		m_bPlayerLook = FALSE;
		m_bReturn = TRUE;
	}
	else if (fDistance <= 0.1f)
	{
		m_pFiniteStateMachineCom->Change_State(FSM_SLIME_IDLE);
		m_bReturn = FALSE;
	}

	return S_OK;
}

void CSlime_Small::Collision_State()
{
	if (m_fHp <= 0.f)
		return;

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));

	FSM_ANDRIUS_STATE eState = static_cast<FSM_ANDRIUS_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());
	_int iWeaponID = -1;
	vector<CGameObject*> vecPlayer;

	if (pPlayer->Get_CombatMode() == FALSE) /* Player 한명만 온필드 */
	{
		if (static_cast<CPart_Body*>(m_Parts[PART_BODY].pPartySlot)->isFunctionTrue() == true) // 플레이어 충돌 해제
		{
			m_bHitPlayer = FALSE;
			_int iPlayerCollisionID = pPlayer->Get_BodyCollisionID();
			m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, iPlayerCollisionID, TRUE);
		}
	}
	else /* Player 전체 */
	{
		if (static_cast<CPart_Body*>(m_Parts[PART_BODY].pPartySlot)->isFunctionTrue() == true) // 플레이어 충돌 해제
		{
			m_bHitPlayer = FALSE;

			for (size_t i = 0; i < pPlayer->Get_TotalPlayer(); i++)
			{
				_int iPlayerCollisionID = pPlayer->Get_BodyCollisionID((PLAYERINDEX)i);
				m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, iPlayerCollisionID, TRUE);
			}
		}
	}

	/* 슬라임 상태에 따라 충돌 처리할 콜라이더를 정하고 플레이어와의 충돌 검사 */
	if (eState == FSM_SLIME_ATTACK)
	{
		iWeaponID = static_cast<CPart_Body*>(m_Parts[PART_BODY].pPartySlot)->Get_CollisionID();
		vecPlayer = m_pGameInstance->Find_Collision_Objects(COLL_MONSTER_BODY, iWeaponID, COLL_PLAYER_BODY);

	}
	else
		m_bHitPlayer = TRUE;

	if (pPlayer->Get_CombatMode() == FALSE) /* Player 한명만 온필드 */
	{
		/* 플레이어가 충돌했을 때 */
		if (m_bHitPlayer == FALSE && vecPlayer.size() != 0)
		{
			m_bHitPlayer = TRUE;

			_int iPlayerCollisionID = pPlayer->Get_BodyCollisionID();
			m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, iPlayerCollisionID, FALSE);

			pPlayer->Set_Damage((_float)m_pGameInstance->Get_Int_Random(50, 100));
		}
	}
	else /* Player 전체 */
	{
		/* 플레이어가 충돌했을 때 */
		if (m_bHitPlayer == FALSE && vecPlayer.size() != 0)
		{
			m_bHitPlayer = TRUE;

			for (auto& pPlayerBody : vecPlayer)
			{
				PLAYERINDEX eIndex = pPlayer->Get_PlayerIndex(static_cast<CPart_Body*>(pPlayerBody)->Get_CreatureId());
				_int iPlayerCollisionID = pPlayer->Get_BodyCollisionID(eIndex);
				m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, iPlayerCollisionID, FALSE);

				pPlayer->Set_AutoDamage(eIndex, (_float)m_pGameInstance->Get_Int_Random(50, 100));
			}
		}
	}
}

FSM_SLIME_STATE CSlime_Small::Get_State(FSM_OBJECT_STATE eForceState)
{
	FSM_SLIME_STATE eSlimeState = FSM_SLIME_END;

	switch (eForceState)
	{
	case FSM_OBJECT_HIT_H:
		eSlimeState = FSM_SLIME_HIT_H;
		break;
	case FSM_OBJECT_HIT_L:
		eSlimeState = FSM_SLIME_HIT_L;
		break;
	case FSM_OBJECT_HIT_THROW:
		eSlimeState = FSM_SLIME_HIT_THROW_START;
		break;
	}

	return eSlimeState;
}

void CSlime_Small::ElementalReaction(_float fDamage, ELEMENTAL_REACTION eReact)
{
	switch (eReact)
	{
	case Client::REACT_VAPORIZE:
		CSlime_Small::Set_Damage(fDamage * 2.f, ELEMENT_FIRE);
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_FIRE, CElement_OHD::OVERHEAD_WATER);
		break;
	case Client::REACT_VAPORIZE_REVERSE:
		CSlime_Small::Set_Damage(fDamage * 1.5f, ELEMENT_FIRE);
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_FIRE, CElement_OHD::OVERHEAD_WATER);
		break;
	case Client::REACT_MELT:
		
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_FIRE, CElement_OHD::OVERHEAD_ICE);
		break;
	case Client::REACT_MELT_REVERSE:
		
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_FIRE, CElement_OHD::OVERHEAD_ICE);
		break;
	case Client::REACT_OVERLOADED:
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_FIRE, CElement_OHD::OVERHEAD_LIGHTNING);
		break;
	case Client::REACT_SWIRL_FIRE:
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_WIND, CElement_OHD::OVERHEAD_FIRE);
		break;
	case Client::REACT_SWIRL_WATER:
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_WIND, CElement_OHD::OVERHEAD_WATER);
		break;
	case Client::REACT_SWIRL_ELECTRIC:
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_WIND, CElement_OHD::OVERHEAD_LIGHTNING);
		break;
	case Client::REACT_SWIRL_FROST:
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_WIND, CElement_OHD::OVERHEAD_ICE);
		break;
	case Client::REACT_SUPERCONDUCT:
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_ICE, CElement_OHD::OVERHEAD_LIGHTNING);
		break;
	case Client::REACT_ELECTROCHARGED:
		if (m_pGameInstance->IsProgress_StatusEffect(SE_ELECTRO_CHARGE, m_strStatusTag) == FALSE)
		{
			m_pGameInstance->Add_StatusEffect(SE_ELECTRO_CHARGE, m_strStatusTag, 0, 10.f);
			m_fElectroChargedTime = 1.f;
		}
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_WATER, CElement_OHD::OVERHEAD_LIGHTNING);
		break;
	case Client::REACT_FROZEN:
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_WATER, CElement_OHD::OVERHEAD_ICE);
		break;
	case Client::REACT_CRYSTALLIZE_FIRE:
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_EARTH, CElement_OHD::OVERHEAD_FIRE);
		break;
	case Client::REACT_CRYSTALLIZE_WATER:
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_EARTH, CElement_OHD::OVERHEAD_WATER);
		break;
	case Client::REACT_CRYSTALLIZE_ELECTIRC:
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_EARTH, CElement_OHD::OVERHEAD_LIGHTNING);
		break;
	case Client::REACT_CRYSTALLIZE_FROST:
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_EARTH, CElement_OHD::OVERHEAD_ICE);
		break;
	default:
		break;
	}

	if (eReact != REACT_END)
		__super::Create_ReactionFont(eReact);
}

void CSlime_Small::Set_ElementUI(ELEMENT eElement)
{
	switch (eElement)
	{
	case Client::ELEMENT_FIRE:
		m_pElement_OHD->Set_Element(CElement_OHD::OVERHEAD_FIRE, TRUE);
		break;
	case Client::ELEMENT_WATER:
		m_pElement_OHD->Set_Element(CElement_OHD::OVERHEAD_WATER, TRUE);
		break;
	case Client::ELEMENT_WIND:
		m_pElement_OHD->Set_Element(CElement_OHD::OVERHEAD_WIND, TRUE);
		break;
	case Client::ELEMENT_ELECTRIC:
		m_pElement_OHD->Set_Element(CElement_OHD::OVERHEAD_LIGHTNING, TRUE);
		break;
	case Client::ELEMENT_FROST:
		m_pElement_OHD->Set_Element(CElement_OHD::OVERHEAD_ICE, TRUE);
		break;
	case Client::ELEMENT_EARTH:
		m_pElement_OHD->Set_Element(CElement_OHD::OVERHEAD_EARTH, TRUE);
		break;
	default:
		break;
	}
}

void CSlime_Small::Set_Damage(_float fDamage, ELEMENT eElement)
{
	m_fHp -= fDamage;
	if (m_fHp < 0.f)
	{
		m_fHp = 0.f;
		for (auto iter = m_MonsterTarget.begin(); iter != m_MonsterTarget.end(); ++iter)
		{
			if (*iter == this)
			{
				m_MonsterTarget.erase(iter);
				break;
			}
		}
	}

	m_pHpBar->Set_CurrentHp(m_fHp);
	if(eElement != ELEMENT_END)
		__super::Create_DamageFont(fDamage, m_Parts[PART_BODY].pPartySlot->Get_ReceiveElement()); /* 데미지 폰트 */
	else
		__super::Create_DamageFont(fDamage, eElement); /* 데미지 폰트 */

	CEffect::EFFECT_DESC descc = {};
	descc.eEffectID = EFFECT_HITTING_STAR;
	descc.fLifeTime = 5.f;
	descc.vSpawnPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	descc.vSpawnPos.m128_f32[1] += 1.f;
	descc.vScale = { 5.f, 5.f, 1.f, 0.f };

	m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Hitting", L"Layer_Effect", &descc);
	//if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Hitting", L"Layer_Effect", &descc)))
	//	return E_FAIL;
}

HRESULT CSlime_Small::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSlime_Small::Initialize_Clone(void* pArg)
{
	OBJECT_DESC ObjDesc = *static_cast<OBJECT_DESC*>(pArg);

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale({ 2.f, 2.f, 2.f, 0.f });
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, ObjDesc.vInitPos);
	m_vInitPos = ObjDesc.vInitPos;

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
	CTransform* pPlayerTransform = static_cast<CTransform*>(pPlayer->Get_PlayerInfo(pPlayer->Get_CurrentIndex()).pTransform);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	_vector vSetDir = vPlayerPos - vPos;
	vSetDir.m128_f32[1] = 0.f;
	vSetDir = XMVector3Normalize(vSetDir);
	m_pTransformCom->Reset_Look_At_Direction();
	m_pTransformCom->Set_Look_At_Direction(vSetDir); //

	if (FAILED(Initialize_Controller(5.5f, 0.25f, 0.0f, m_pTransformCom)))
		return E_FAIL;

	_float4 fPos; XMStoreFloat4(&fPos, ObjDesc.vInitPos);
	m_pController->Set_Position(fPos);

	m_vInitPos.m128_f32[2] += 0.1f;

	if (FAILED(Add_Parts(pArg)))
		return E_FAIL;

	m_pFiniteStateMachineCom->Append_GeneralState(vector<_int>{ FSM_SLIME_ATTACK, FSM_HIT_L, FSM_HIT_H, 
																FSM_SLIME_HIT_THROW_START, FSM_SLIME_HIT_THROW, FSM_SLIME_HIT_THROW_DOWN, FSM_SLIME_DIE1});
	COrtho::ORTHO_DESC tOrthoDesc;

	tOrthoDesc.eTextureLevel = LEVEL_STATIC;
	tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_HPBar");
	tOrthoDesc.fPosition = { static_cast<_float>(0.f - g_iWinSizeX / 2), static_cast<_float>(-(0.f - g_iWinSizeY / 2)) };
	tOrthoDesc.fSize = { 100.f,5.f };
	tOrthoDesc.ePage = PAGE_MAIN;

	m_pHpBar = static_cast<CHp_Bar*>(m_pGameInstance->Find_Object_Prototype(L"Prototype_GameObject_Ui_Hp_Bar")->Clone(&tOrthoDesc));
	m_pHpBar->Set_MaxHp(m_fHp);
	m_pHpBar->Set_BarType(CHp_Bar::BAR_TYPE_MONSTER);
	m_pHpBar->Set_CurrentHp(m_fHp);
	m_pHpBar->Set_OwnerInfo(TEXT("LV.12"), TEXT(""), 0);

	m_pElement_OHD = (CElement_OHD*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Element_OHD"))->Clone(&tOrthoDesc);

	m_strStatusTag = Get_ParticleTag(L"Slime_Small");

	return S_OK;
}

HRESULT CSlime_Small::PreviousTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (FAILED(__super::PreviousTick(fTimeDelta)))
			return E_FAIL;
		
		_float4x4 CombinedWorldMatrix = static_cast<CPart_Body*>(m_Parts[PART_BODY].pPartySlot)->Get_CombinedMatrix();
		if (m_pGameInstance->PointInFrustum_InWorld(XMLoadFloat4x4(&CombinedWorldMatrix).r[3], 5.f) && m_fHp > 0.f)
			Add_Target(this);

		for (auto& pPart : m_Parts)
		{
			pPart.pPartySlot->PreviousTick(fTimeDelta);
		}

		if (m_fSelfDamage != 0.f)
		{
			Set_Damage(m_fSelfDamage, ELEMENT_ELECTRIC);
			m_fSelfDamage = 0.f;
		}

		/* 데미지 받았을 때 */
		_float fDamage = m_Parts[PART_BODY].pPartySlot->Get_Damage();
		_bool bPerification = m_Parts[PART_BODY].pPartySlot->Get_Perification();
		FSM_OBJECT_STATE eReceiveState = m_Parts[PART_BODY].pPartySlot->Get_ReceiveState();

		if (fDamage != 0.f)
		{
			ELEMENT eElement = m_Parts[PART_BODY].pPartySlot->Get_ReceiveElement();
			Set_ElementUI(eElement);
			ELEMENTAL_REACTION eReaction = m_Parts[PART_BODY].pPartySlot->Get_ElementalReaction(); /* 원소반응 */
			ElementalReaction(fDamage, eReaction);

			if (bPerification == TRUE && m_bInitPerification == TRUE)
			{
				m_Parts[PART_BODY].pPartySlot->Set_Petrification(FALSE);
			}

			CSlime_Small::Set_Damage(fDamage, eElement);

			if (eReceiveState != FSM_OBJECT_END && m_fHp > 0.f)
			{
				m_pFiniteStateMachineCom->Change_ForceState(CSlime_Small::Get_State(eReceiveState));

				CModel* pModel = static_cast<CModel*>(m_Parts[PART_BODY].pPartySlot->Find_Component(TEXT("Com_Model")));
				m_pGameInstance->Play_Particle(TEXT("Hit_Big"), Get_ParticleTag(TEXT("Hit_Big")), PARTICLE_DEFAULT, TEX_PARTICLE_DEFAULT, pModel->Get_CombinedMatrix("HitBox"), m_pTransformCom, 0.5f);
			}
			
		}

		if (m_Parts[PART_BODY].pPartySlot->Get_Dead() == TRUE)
		{
			if (FAILED(CCreature::Drop_Item(124, 1000)))
				return E_FAIL;

			if (FAILED(CCreature::Drop_Item(135, 1)))
				return E_FAIL;

			m_pGameInstance->Add_InteractionCount(L"BattleBGM", -1);
			Set_Destroy(TRUE);
		}

		FSM_SLIME_STATE eState = static_cast<FSM_SLIME_STATE>(m_pFiniteStateMachineCom->Get_CurrentState());

		if (m_fHp <= 0.f)
		{
			if (eState == FSM_SLIME_HIT_H || eState == FSM_SLIME_HIT_L
				|| eState == FSM_SLIME_HIT_THROW_START || eState == FSM_SLIME_HIT_THROW || eState == FSM_SLIME_HIT_THROW_DOWN)
			{
				m_Parts[PART_BODY].pPartySlot->Set_Petrification(FALSE);
				m_Parts[PART_BODY].pPartySlot->Release_ColliderID();
				m_pFiniteStateMachineCom->Change_ForceState(FSM_SLIME_DIE1);
			}
			else
			{
				m_Parts[PART_BODY].pPartySlot->Set_Petrification(FALSE);
				m_Parts[PART_BODY].pPartySlot->Release_ColliderID();
				m_pFiniteStateMachineCom->Change_ForceState(FSM_SLIME_DIE1);
			}
			return S_OK;
		}
		m_bInitPerification = bPerification;
	}

	m_pHpBar->PreviousTick(fTimeDelta);
	m_pElement_OHD->PreviousTick(fTimeDelta);

	return S_OK;
}

HRESULT CSlime_Small::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (FAILED(CSlime_Small::Pattern(fTimeDelta)))
			return E_FAIL;
		
		Collision_State();

		if (FAILED(__super::Tick(fTimeDelta)))
			return E_FAIL;

		for (auto& pPart : m_Parts)
		{
			pPart.pPartySlot->Tick(fTimeDelta);
		}

		m_pHpBar->Update_Matrix(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.f, 0.f);
		m_pHpBar->Tick(fTimeDelta);
		_float2 vPos = m_pHpBar->Get_WindowPos();
		m_pElement_OHD->Update_Matrix(vPos.x, vPos.y + 45.f, 30.f, 30.f);
		m_pElement_OHD->Tick(fTimeDelta);

		m_pController->Apply_Gravity(fTimeDelta);
	}

	return S_OK;
}

HRESULT CSlime_Small::LateTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_RenderValidity() == S_OK)
	{
		Clear_Target();

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
		if (XMVector3Dot(XMVector3Normalize(vToPlayer), XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK))).m128_f32[0] > 0 &&
			XMVector3Length(vToPlayer).m128_f32[0] <= 10.f)
		{

			CUI_Manager* pUIManager = (CUI_Manager*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager"), 0);
			CPage_Main* m_pPage_Main = (CPage_Main*)pUIManager->Get_UI_Page(PAGE_MAIN);

			m_pPage_Main->Register_On_MiniMap(m_pTransformCom, 0);
			m_pHpBar->LateTick(fTimeDelta);
			m_pElement_OHD->LateTick(fTimeDelta);
		}

		Release_Target();
	}

	return S_OK;
}

HRESULT CSlime_Small::Render()
{
	return S_OK;
}

CSlime_Small* CSlime_Small::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSlime_Small* pInstance = new CSlime_Small(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CSlime_Small"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSlime_Small::Clone(void* pArg)
{
	CSlime_Small* pInstance = new CSlime_Small(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CSlime_Small"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSlime_Small::Free()
{
	__super::Free();

	if (m_bCloned == true)
	{
		Safe_Release(m_pHpBar);
		Safe_Release(m_pElement_OHD);
	}

	for (auto& pPart : m_Parts)
	{
		Safe_Release(pPart.pPartySlot);
	}
	m_Parts.clear();
}