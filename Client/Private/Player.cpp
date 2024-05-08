#include "Elemental_Manager.h"

#include "stdafx.h"
#include "Player.h"
#include "Ground_Item.h"

#include "Part_Body.h"
//#include "Part_Weapon.h"
#include "Claymore_Wolfmound.h"
#include "Part_Glider.h"
#include "Part_Status.h"
#include "Inventory.h"

#include "UI_Manager.h"
#include "Page_Main.h"

#include "GameInstance.h"

#include "CharacterController.h"

#include "BabaraBasicAttack.h"
#include "Raiden_ElementAttack.h"
#include "Eff_Impact_Glow.h"
#include "Slash_Trail.h"
#include "FlameSlash_Trail.h"
#include "Piercing_Trail.h"
#include "Shougun_Trail_TEST.h"

#include "Skill_Claw.h"
#include "Level_Loading.h"

#define INVINCIBILITY 0.3f

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCreature(pDevice, pContext)
{
} 

CPlayer::CPlayer(const CPlayer& rhs)
	: CCreature(rhs)
{
}

HRESULT CPlayer::Add_Parts(void* pArg)
{
	CPartObject* pPartObject = { };
	PARTY_SLOTS		tSlots = { };

#pragma region Part_Body
	/* For. Part_Body */
	CPart_Body::PARTBODY_DESC tBodyDesc = { };
	tBodyDesc.tPartObjDesc.pParent = this;
	tBodyDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
	tBodyDesc.pTransform = PlayerInfo[PLAYER_ONE].pTransform;
	tBodyDesc.pFiniteStateMachineCom = PlayerInfo[PLAYER_ONE].pFiniteStateMachineCom;
	tBodyDesc.pController = PlayerInfo[PLAYER_ONE].pController;
	tBodyDesc.eType = CTY_PLAYER;

	CREATUREID eCreatureID = CHARACTER_DILUC;
	pPartObject = CPart_Body::Create_Body(eCreatureID, tBodyDesc);
	if (pPartObject == nullptr)
		return E_FAIL;
	static_cast<CPart_Body*>(pPartObject)->Set_CreatureID(eCreatureID);

	tSlots.pPartySlot[PLAYER_ONE] = pPartObject;
	m_Parts.push_back(tSlots);
	m_vecPlayerIndex.push_back(CHARACTER_DILUC);

	eCreatureID = CHARACTER_ZHONGLI;
	tBodyDesc.pTransform = PlayerInfo[PLAYER_TWO].pTransform;
	tBodyDesc.pFiniteStateMachineCom = PlayerInfo[PLAYER_TWO].pFiniteStateMachineCom;
	tBodyDesc.pController = PlayerInfo[PLAYER_TWO].pController;
	pPartObject = CPart_Body::Create_Body(eCreatureID, tBodyDesc);
	if (pPartObject == nullptr)
		return E_FAIL;
	static_cast<CPart_Body*>(pPartObject)->Set_CreatureID(eCreatureID);

	tSlots.pPartySlot[PLAYER_TWO] = pPartObject;
	m_Parts[PART_BODY].pPartySlot[PLAYER_TWO] = pPartObject;
	m_vecPlayerIndex.push_back(CHARACTER_ZHONGLI);

	eCreatureID = CHARACTER_BARBARA;
	tBodyDesc.pTransform = PlayerInfo[PLAYER_THREE].pTransform;
	tBodyDesc.pFiniteStateMachineCom = PlayerInfo[PLAYER_THREE].pFiniteStateMachineCom;
	tBodyDesc.pController = PlayerInfo[PLAYER_THREE].pController;
	pPartObject = CPart_Body::Create_Body(eCreatureID, tBodyDesc);
	if (pPartObject == nullptr)
		return E_FAIL;
	static_cast<CPart_Body*>(pPartObject)->Set_CreatureID(eCreatureID);

	tSlots.pPartySlot[PLAYER_THREE] = pPartObject;
	m_Parts[PART_BODY].pPartySlot[PLAYER_THREE] = pPartObject;
	m_vecPlayerIndex.push_back(CHARACTER_BARBARA);

	eCreatureID = CHARACTER_RAIDEN;
	tBodyDesc.pTransform = PlayerInfo[PLAYER_FOUR].pTransform;
	tBodyDesc.pFiniteStateMachineCom = PlayerInfo[PLAYER_FOUR].pFiniteStateMachineCom;
	tBodyDesc.pController = PlayerInfo[PLAYER_FOUR].pController;
	pPartObject = CPart_Body::Create_Body(eCreatureID, tBodyDesc);
	if (pPartObject == nullptr)
		return E_FAIL;
	static_cast<CPart_Body*>(pPartObject)->Set_CreatureID(eCreatureID);

	tSlots.pPartySlot[PLAYER_FOUR] = pPartObject;
	m_Parts[PART_BODY].pPartySlot[PLAYER_FOUR] = pPartObject;
	m_vecPlayerIndex.push_back(CHARACTER_RAIDEN);
#pragma endregion

#pragma region Part_Weapon
	tSlots = { };
	/* For. Part_Weapon */
	CModel* pBody = static_cast<CModel*>(m_Parts[PART_BODY].pPartySlot[PLAYER_ONE]->Find_Component(TEXT("Com_Model")));

	CPart_Weapon::PARTWEAPON_DESC tWeaponDesc = { };
	tWeaponDesc.pBody = pBody;
	tWeaponDesc.tPartObjDesc.pParent = this;
	tWeaponDesc.pTransform = PlayerInfo[PLAYER_ONE].pTransform;
	tWeaponDesc.pFiniteStateMachineCom = PlayerInfo[PLAYER_ONE].pFiniteStateMachineCom;
	tWeaponDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
	tWeaponDesc.eType = CTY_PLAYER;

	pPartObject = CPart_Weapon::Create_Weapon(WEAPON_WOLFMOUND, tWeaponDesc);
	if (pPartObject == nullptr)
		return E_FAIL;

	tSlots.pPartySlot[PLAYER_ONE] = pPartObject;
	m_Parts.push_back(tSlots);

	pBody = static_cast<CModel*>(m_Parts[PART_BODY].pPartySlot[PLAYER_TWO]->Find_Component(TEXT("Com_Model")));

	tWeaponDesc = { };
	tWeaponDesc.pBody = pBody;
	tWeaponDesc.tPartObjDesc.pParent = this;
	tWeaponDesc.pTransform = PlayerInfo[PLAYER_TWO].pTransform;
	tWeaponDesc.pFiniteStateMachineCom = PlayerInfo[PLAYER_TWO].pFiniteStateMachineCom;
	tWeaponDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
	tWeaponDesc.eType = CTY_PLAYER;

	pPartObject = CPart_Weapon::Create_Weapon(WEAPON_KUNWU, tWeaponDesc);
	if (pPartObject == nullptr)
		return E_FAIL;

	tSlots.pPartySlot[PLAYER_TWO] = pPartObject;
	m_Parts[PART_WEAPON].pPartySlot[PLAYER_TWO] = pPartObject;

	pBody = static_cast<CModel*>(m_Parts[PART_BODY].pPartySlot[PLAYER_THREE]->Find_Component(TEXT("Com_Model")));

	tWeaponDesc = { };
	tWeaponDesc.pBody = pBody;
	tWeaponDesc.tPartObjDesc.pParent = this;
	tWeaponDesc.pTransform = PlayerInfo[PLAYER_THREE].pTransform;
	tWeaponDesc.pFiniteStateMachineCom = PlayerInfo[PLAYER_THREE].pFiniteStateMachineCom;
	tWeaponDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
	tWeaponDesc.eType = CTY_PLAYER;

	pPartObject = CPart_Weapon::Create_Weapon(WEAPON_ZEPHYRUS, tWeaponDesc);
	if (pPartObject == nullptr)
		return E_FAIL;

	tSlots.pPartySlot[PLAYER_THREE] = pPartObject;
	m_Parts[PART_WEAPON].pPartySlot[PLAYER_THREE] = pPartObject;

	pBody = static_cast<CModel*>(m_Parts[PART_BODY].pPartySlot[PLAYER_FOUR]->Find_Component(TEXT("Com_Model")));

	tWeaponDesc = { };
	tWeaponDesc.pBody = pBody;
	tWeaponDesc.tPartObjDesc.pParent = this;
	tWeaponDesc.pTransform = PlayerInfo[PLAYER_FOUR].pTransform;
	tWeaponDesc.pFiniteStateMachineCom = PlayerInfo[PLAYER_FOUR].pFiniteStateMachineCom;
	tWeaponDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;
	tWeaponDesc.eType = CTY_PLAYER;

	pPartObject = CPart_Weapon::Create_Weapon(WEAPON_NARUKAMI, tWeaponDesc);
	if (pPartObject == nullptr)
		return E_FAIL;

	tSlots.pPartySlot[PLAYER_FOUR] = pPartObject;
	m_Parts[PART_WEAPON].pPartySlot[PLAYER_FOUR] = pPartObject;
#pragma endregion

#pragma region Part_Glider
	tSlots = { };
	/* For. Part_Glider */
	pBody = static_cast<CModel*>(m_Parts[PART_BODY].pPartySlot[PLAYER_ONE]->Find_Component(TEXT("Com_Model")));

	CPart_Glider::PARTGLIDER_DESC tGliderDesc = { };
	tGliderDesc.pBody = pBody;
	tGliderDesc.tPartObjDesc.pParent = this;
	tGliderDesc.pTransform = PlayerInfo[PLAYER_ONE].pTransform;
	tGliderDesc.pFiniteStateMachineCom = PlayerInfo[PLAYER_ONE].pFiniteStateMachineCom;
	tGliderDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;

	pPartObject = CPart_Glider::Create_Glider(GLIDER_ORIGIN, tGliderDesc);
	if (pPartObject == nullptr)
		return E_FAIL;
	
	tSlots.pPartySlot[PLAYER_ONE] = pPartObject;
	m_Parts.push_back(tSlots);

	pBody = static_cast<CModel*>(m_Parts[PART_BODY].pPartySlot[PLAYER_TWO]->Find_Component(TEXT("Com_Model")));

	tGliderDesc = { };
	tGliderDesc.pBody = pBody;
	tGliderDesc.tPartObjDesc.pParent = this;
	tGliderDesc.pTransform = PlayerInfo[PLAYER_TWO].pTransform;
	tGliderDesc.pFiniteStateMachineCom = PlayerInfo[PLAYER_TWO].pFiniteStateMachineCom;
	tGliderDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;

	pPartObject = CPart_Glider::Create_Glider(GLIDER_LIYUE, tGliderDesc);
	if (pPartObject == nullptr)
		return E_FAIL;

	tSlots.pPartySlot[PLAYER_TWO] = pPartObject;
	m_Parts[PART_GLIDER].pPartySlot[PLAYER_TWO] = pPartObject;

	pBody = static_cast<CModel*>(m_Parts[PART_BODY].pPartySlot[PLAYER_THREE]->Find_Component(TEXT("Com_Model")));

	tGliderDesc = { };
	tGliderDesc.pBody = pBody;
	tGliderDesc.tPartObjDesc.pParent = this;
	tGliderDesc.pTransform = PlayerInfo[PLAYER_THREE].pTransform;
	tGliderDesc.pFiniteStateMachineCom = PlayerInfo[PLAYER_THREE].pFiniteStateMachineCom;
	tGliderDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;

	pPartObject = CPart_Glider::Create_Glider(GLIDER_PSALMUS, tGliderDesc);
	if (pPartObject == nullptr)
		return E_FAIL;

	tSlots.pPartySlot[PLAYER_THREE] = pPartObject;
	m_Parts[PART_GLIDER].pPartySlot[PLAYER_THREE] = pPartObject;

	pBody = static_cast<CModel*>(m_Parts[PART_BODY].pPartySlot[PLAYER_FOUR]->Find_Component(TEXT("Com_Model")));

	tGliderDesc = { };
	tGliderDesc.pBody = pBody;
	tGliderDesc.tPartObjDesc.pParent = this;
	tGliderDesc.pTransform = PlayerInfo[PLAYER_FOUR].pTransform;
	tGliderDesc.pFiniteStateMachineCom = PlayerInfo[PLAYER_FOUR].pFiniteStateMachineCom;
	tGliderDesc.tModelComponent.iComponentLevel = LEVEL_STATIC;

	pPartObject = CPart_Glider::Create_Glider(GLIDER_THUNDERCLAP, tGliderDesc);
	if (pPartObject == nullptr)
		return E_FAIL;

	tSlots.pPartySlot[PLAYER_FOUR] = pPartObject;
	m_Parts[PART_GLIDER].pPartySlot[PLAYER_FOUR] = pPartObject;
#pragma endregion

#pragma region Part_Status
	tSlots = { };
	/* For. Part_Status */
	CPart_Status::PARTSTATUS_DESC tStatusDesc = { };
	tStatusDesc.tPartObjDesc.pParent = this;

	pPartObject = CPart_Status::Create_Status(CHARACTER_DILUC, tStatusDesc);
	if (pPartObject == nullptr)
		return E_FAIL;

	tSlots.pPartySlot[PLAYER_ONE] = pPartObject;
	m_Parts.push_back(tSlots);

	tStatusDesc = { };
	tStatusDesc.tPartObjDesc.pParent = this;

	pPartObject = CPart_Status::Create_Status(CHARACTER_ZHONGLI, tStatusDesc);
	if (pPartObject == nullptr)
		return E_FAIL;

	tSlots.pPartySlot[PLAYER_TWO] = pPartObject;
	m_Parts[PART_STATUS].pPartySlot[PLAYER_TWO] = pPartObject;

	tStatusDesc = { };
	tStatusDesc.tPartObjDesc.pParent = this;

	pPartObject = CPart_Status::Create_Status(CHARACTER_BARBARA, tStatusDesc);
	if (pPartObject == nullptr)
		return E_FAIL;

	tSlots.pPartySlot[PLAYER_THREE] = pPartObject;
	m_Parts[PART_STATUS].pPartySlot[PLAYER_THREE] = pPartObject;

	tStatusDesc = { };
	tStatusDesc.tPartObjDesc.pParent = this;

	pPartObject = CPart_Status::Create_Status(CHARACTER_RAIDEN, tStatusDesc);
	if (pPartObject == nullptr)
		return E_FAIL;

	tSlots.pPartySlot[PLAYER_FOUR] = pPartObject;
	m_Parts[PART_STATUS].pPartySlot[PLAYER_FOUR] = pPartObject;
#pragma endregion

	m_pGameInstance->Set_IsCollision(COLL_EVENTREACT_PLAYER, m_Parts[PART_BODY].pPartySlot[m_ePlayerNum]->Get_CollisionID() + 1, TRUE);

	CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager")));
	m_pPageMain = static_cast<CPage_Main*>(pUIManager->Get_UI_Page(PAGE_MAIN));
	Safe_AddRef(m_pPageMain);

	for (_uint i = 0; i < PLAYER_END; ++i) // 다이루크, 종려, 바바라, 라이덴
	{
		_float fCurrentMaxHp = static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[i])->Get_MaxHp();
		_float fCurrentHp = static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[i])->Get_CurrentHp();

		m_pPageMain->Set_MaxHp(i, fCurrentMaxHp);
		m_pPageMain->Set_Hp(i, fCurrentHp);
	}

	/*For.Inventory*/
	m_pInventory = static_cast<CInventory*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Inventory")));

	return S_OK;
}

HRESULT CPlayer::Add_ExtraComponent()
{
	/* For.Com_Transform */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform2P"), (CComponent**)&m_pTransformCom2P)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform3P"), (CComponent**)&m_pTransformCom3P)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform4P"), (CComponent**)&m_pTransformCom4P)))
		return E_FAIL;

	/* For.Com_FiniteStateMachine */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_FiniteStateMachine"),
		TEXT("Com_FiniteStateMachine2P"), (CComponent**)&m_pFiniteStateMachineCom2P)))
		return E_FAIL;

	/* For.Com_FiniteStateMachine */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_FiniteStateMachine"),
		TEXT("Com_FiniteStateMachine3P"), (CComponent**)&m_pFiniteStateMachineCom3P)))
		return E_FAIL;

	/* For.Com_FiniteStateMachine */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_FiniteStateMachine"),
		TEXT("Com_FiniteStateMachine4P"), (CComponent**)&m_pFiniteStateMachineCom4P)))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Set_Damage(_float fDamage, ELEMENT eElement, FSM_STATE eForceState, _float fLevitate, _float fKnockback, _float4 vKnockbackDir, _float fInvincibility)
{
	if (m_fInvincibility[m_ePlayerNum] > 0.f)
		return;

	FSM_STATE eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Get_CurrentState());
	if (eState == FSM_SHOW || 
		eState == FSM_ELEMENTAL_BURST || eState == FSM_ELEMENTAL_BURST_IDLE || eState == FSM_DASH || 
		eState == FSM_DEAD1 || eState == FSM_DEAD2)
		return; /* 무적 */

	if (m_Parts[PART_BODY].pPartySlot[m_ePlayerNum]->Get_Freezing() == TRUE && fDamage > 0.f && eForceState != FSM_END)
		m_Parts[PART_BODY].pPartySlot[m_ePlayerNum]->Set_Freezing(FALSE);


	if (eElement != ELEMENT_END)
	{
		CPlayer::Set_ElementUI(eElement);
		static_cast<CPart_Body*>(m_Parts[PART_BODY].pPartySlot[m_ePlayerNum])->Set_HostElement(eElement);
		ELEMENTAL_REACTION eReaction = m_Parts[PART_BODY].pPartySlot[m_ePlayerNum]->Get_ElementalReaction(); /* 원소반응 */
		CPlayer::ElementalReaction(fDamage, eReaction);
	}


	_int iShield = m_pGameInstance->Get_Count_StatusEffect(SE_ZHONGLI_SHIELD, L"Zhongli");
	_bool bSuperArmor = FALSE;

	if (iShield != -1) /* 쉴드가 있을 때 */
	{
		iShield -= (_int)fDamage;
		if (0 < iShield)
		{
			m_pGameInstance->Add_Count_StatusEffect(SE_ZHONGLI_SHIELD, L"Zhongli", -(_int)fDamage);
			bSuperArmor = TRUE;
		}
		else
		{
			m_pGameInstance->Add_Count_StatusEffect(SE_ZHONGLI_SHIELD, L"Zhongli", -(_int)fDamage);
			static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[m_ePlayerNum])->Set_Damage(abs((_float)iShield));
			bSuperArmor = TRUE;
		}
	}
	else /* 없으면 그냥 피까기 */
	{
		static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[m_ePlayerNum])->Set_Damage(fDamage);
	}

	_float fCurrentHp = static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[m_ePlayerNum])->Get_CurrentHp();

	if (fDamage != 0.f)
	{
		if (fInvincibility == 0.f)
		{
			m_fInvincibility[m_ePlayerNum] = INVINCIBILITY;
			m_bInvincibility[m_ePlayerNum] = TRUE;
		}
		else
		{
			m_fInvincibility[m_ePlayerNum] = fInvincibility;
			m_bInvincibility[m_ePlayerNum] = TRUE;
		}

		CModel* pModel = static_cast<CModel*>(m_Parts[PART_BODY].pPartySlot[m_ePlayerNum]->Find_Component(TEXT("Com_Model")));
		m_pGameInstance->Play_Particle(TEXT("Hit_Big"), Get_ParticleTag(TEXT("Hit_Big")), PARTICLE_DEFAULT, TEX_PARTICLE_DEFAULT, pModel->Get_CombinedMatrix("HitObject2"), PlayerInfo[m_ePlayerNum].pTransform, 0.5f);
	}
	
	m_pPageMain->Set_Hp(m_ePlayerNum, fCurrentHp);

	if (bSuperArmor == TRUE) /* 슈퍼 아머 */
	{
		fLevitate = 0.f;
		vKnockbackDir = {};
		eForceState = FSM_END;
	}
	else if (fKnockback != 0.f)
	{
		PlayerInfo[m_ePlayerNum].fKnockback = fLevitate;
		PlayerInfo[m_ePlayerNum].vKnockbackDir = vKnockbackDir;

		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr)
		{
			pCamera->Set_ShakingMode(0.1f, 5.f, 1);
		}
	}

	if (m_isUndead == FALSE)
	{
		if (fCurrentHp > 0.f)
		{
			if (eForceState != FSM_END)
			{
				PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_ForceState(eForceState);
				m_iExtraAttackCount = 0;
			}

			if (fLevitate != 0.f)
				PlayerInfo[m_ePlayerNum].pController->Ready_Jump(fLevitate);
		}
		else
		{
			m_Parts[PART_WEAPON].pPartySlot[m_ePlayerNum]->Set_Dissolve(TRUE);
			PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_ForceState(FSM_DEAD1);
			m_iExtraAttackCount = 0;
		}
	}
	else
	{
		if (eForceState != FSM_END)
		{
			PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_ForceState(eForceState);
			m_iExtraAttackCount = 0;
		}

		if (fLevitate != 0.f)
			PlayerInfo[m_ePlayerNum].pController->Ready_Jump(fLevitate);
	}

	if (eForceState == FSM_HIT_L)
		m_pGameInstance->PlaySoundW(L"PlayerHit.mp3", 1.f);
	else if (eForceState == FSM_HIT_H || eForceState == FSM_HIT_THROW)
		m_pGameInstance->PlaySoundW(L"PlayerHitF.mp3", 1.f);

	__super::Create_DamageFont(fDamage, eElement, CPlayer::Get_Position());

}

void CPlayer::Set_AutoDamage(PLAYERINDEX eIndex, _float fDamage, ELEMENT eElement, FSM_STATE eForceState, _float fLevitate, _float fKnockback, _float4 vKnockbackDir, _float fInvincibility)
{
	if (m_fInvincibility[eIndex] > 0.f)
		return;

	FSM_STATE eState = static_cast<FSM_STATE>(PlayerInfo[eIndex].pFiniteStateMachineCom->Get_CurrentState());
	if (eState == FSM_SHOW ||
		eState == FSM_ELEMENTAL_BURST || eState == FSM_ELEMENTAL_BURST_IDLE || eState == FSM_DASH ||
		eState == FSM_DEAD1 || eState == FSM_DEAD2)
		return; /* 무적 */

	if (m_Parts[PART_BODY].pPartySlot[eIndex]->Get_Freezing() == TRUE && fDamage > 0.f && eForceState != FSM_END)
		m_Parts[PART_BODY].pPartySlot[eIndex]->Set_Freezing(FALSE);

	if (eElement != ELEMENT_END)
	{
		CPlayer::Set_ElementUI(eElement);
		static_cast<CPart_Body*>(m_Parts[PART_BODY].pPartySlot[eIndex])->Set_HostElement(eElement);
		ELEMENTAL_REACTION eReaction = m_Parts[PART_BODY].pPartySlot[eIndex]->Get_ElementalReaction(); /* 원소반응 */
		CPlayer::ElementalReaction(fDamage, eReaction, eIndex);
	}

	_int iShield = m_pGameInstance->Get_Count_StatusEffect(SE_ZHONGLI_SHIELD, L"Zhongli");
	_bool bSuperArmor = FALSE;
	if (eIndex == PLAYER_TWO && iShield != -1) /* 종려가 쉴드가 있을 때 */
	{
		iShield -= (_int)fDamage;
		if (0 < iShield)
		{
			m_pGameInstance->Add_Count_StatusEffect(SE_ZHONGLI_SHIELD, L"Zhongli", -(_int)fDamage);
			bSuperArmor = TRUE;
		}
		else
		{
			m_pGameInstance->Add_Count_StatusEffect(SE_ZHONGLI_SHIELD, L"Zhongli", -(_int)fDamage);
			static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[m_ePlayerNum])->Set_Damage(abs((_float)iShield));
			bSuperArmor = TRUE;
		}
	}
	else /* 종려가 아니거나 쉴드가 없을때 */
	{
		static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[eIndex])->Set_Damage(fDamage);
	}
	
	_float fCurrentHp = static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[eIndex])->Get_CurrentHp();

	if (fDamage != 0.f)
	{
		if (fInvincibility == 0.f)
		{
			m_fInvincibility[eIndex] = INVINCIBILITY;
			m_bInvincibility[eIndex] = TRUE;
		}
		else
		{
			m_fInvincibility[eIndex] = fInvincibility;
			m_bInvincibility[eIndex] = TRUE;
		}

		CModel* pModel = static_cast<CModel*>(m_Parts[PART_BODY].pPartySlot[eIndex]->Find_Component(TEXT("Com_Model")));
		m_pGameInstance->Play_Particle(TEXT("Hit_Big"), Get_ParticleTag(TEXT("Hit_Big")), PARTICLE_DEFAULT, TEX_PARTICLE_DEFAULT, pModel->Get_CombinedMatrix("HitObject2"), PlayerInfo[eIndex].pTransform, 0.5f);
	}

	m_pPageMain->Set_Hp(eIndex, fCurrentHp);

	if (eIndex == PLAYER_TWO && bSuperArmor == TRUE) /* 슈퍼 아머 */
	{
		fLevitate = 0.f;
		vKnockbackDir = {};
		eForceState = FSM_END;
	}
	else if (fKnockback != 0.f)
	{
		PlayerInfo[eIndex].fKnockback = fLevitate;
		PlayerInfo[eIndex].vKnockbackDir = vKnockbackDir;

		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr)
		{
			pCamera->Set_ShakingMode(0.1f, 5.f, 1);
		}
	}

	if (m_isUndead == FALSE)
	{
		if (fCurrentHp > 0.f)
		{
			if (eForceState != FSM_END)
			{
				PlayerInfo[eIndex].pFiniteStateMachineCom->Change_ForceState(eForceState);
				m_iExtraAttackCount = 0;
			}

			if (fLevitate != 0.f)
				PlayerInfo[eIndex].pController->Ready_Jump(fLevitate);
		}
		else
		{
			m_Parts[PART_WEAPON].pPartySlot[eIndex]->Set_Dissolve(TRUE);
			PlayerInfo[eIndex].pFiniteStateMachineCom->Change_ForceState(FSM_DEAD1);
			m_iExtraAttackCount = 0;
		}
	}
	else
	{
		if (eForceState != FSM_END)
		{
			PlayerInfo[eIndex].pFiniteStateMachineCom->Change_ForceState(eForceState);
			m_iExtraAttackCount = 0;
		}

		if (fLevitate != 0.f)
			PlayerInfo[eIndex].pController->Ready_Jump(fLevitate);
	}

	if (eForceState == FSM_HIT_L)
		m_pGameInstance->PlaySoundW(L"PlayerHit.mp3", 1.f);
	else if (eForceState == FSM_HIT_H || eForceState == FSM_HIT_THROW)
		m_pGameInstance->PlaySoundW(L"PlayerHitF.mp3", 1.f);

	__super::Create_DamageFont(fDamage, eElement, CPlayer::Get_Position(eIndex));
}

void CPlayer::Set_MoveSpeed(_float fSpeed, PLAYERINDEX eIndex, _float fDuration)
{
	m_fMoveSpeed[eIndex] = fSpeed;
	m_fMoveSpeedDuration[eIndex] = fDuration;
}

void CPlayer::Set_Heal(PLAYERINDEX eIndex, _float fHeal)
{
	static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[eIndex])->Set_Heal(fHeal);
	m_pPageMain->Set_Hp(eIndex, static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[eIndex])->Get_CurrentHp());
	CCreature::Create_HealFont(fHeal, CPlayer::Get_Position());
}

void CPlayer::Set_AllHeal(_float fHeal)
{
	for (_uint i = 0; i < PLAYER_END; ++i)
	{
		if (m_Parts[PART_BODY].pPartySlot[i]->Get_Dead() == FALSE && PlayerInfo[i].bDead == FALSE)
		{
			if (m_bCombatMode == TRUE)
				CCreature::Create_HealFont(fHeal, CPlayer::Get_Position((PLAYERINDEX)i));
			else if (m_ePlayerNum == i)
				CCreature::Create_HealFont(fHeal, CPlayer::Get_Position((PLAYERINDEX)i));

			static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[i])->Set_Heal(fHeal), CPlayer::Get_Position((PLAYERINDEX)i);
			m_pPageMain->Set_Hp(i, static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[i])->Get_CurrentHp());
		}
	}
}

void CPlayer::Set_RatioHeal(PLAYERINDEX eIndex, _float fHpRatio, _float fAddHeal)
{
	static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[eIndex])->Set_HpRatioHeal(fHpRatio, fAddHeal);

	CCreature::Create_HealFont(static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[eIndex])->Set_HpRatioHeal(fHpRatio, fAddHeal), CPlayer::Get_Position(eIndex));
	m_pPageMain->Set_Hp(eIndex, static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[eIndex])->Get_CurrentHp());
}

void CPlayer::Set_AllRatioHeal(_float fHpRatio, _float fAddHeal)
{
	for (_uint i = 0; i < PLAYER_END; ++i)
	{
		if (m_Parts[PART_BODY].pPartySlot[i]->Get_Dead() == FALSE && PlayerInfo[i].bDead == FALSE)
		{
			_float fHeal = static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[i])->Set_HpRatioHeal(fHpRatio, fAddHeal);
			if (m_bCombatMode == TRUE)
				CCreature::Create_HealFont(fHeal, CPlayer::Get_Position((PLAYERINDEX)i));
			else if (m_ePlayerNum == i)
				CCreature::Create_HealFont(fHeal, CPlayer::Get_Position((PLAYERINDEX)i));
			m_pPageMain->Set_Hp(i, static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[i])->Get_CurrentHp());
		}
	}
	
}

void CPlayer::Set_MaxHp()
{
	for (_uint i = 0; i < PLAYER_END; ++i)
	{
		m_Parts[PART_BODY].pPartySlot[i]->Set_Dead(FALSE);
		PlayerInfo[i].bDead = FALSE;
		static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[i])->Set_MaxHP();
		m_pPageMain->Set_Hp(i, static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[i])->Get_CurrentHp());
	}
}

void CPlayer::Set_MaxEnergy()
{
	for (_uint i = 0; i < PLAYER_END; ++i)
	{
		m_Parts[PART_BODY].pPartySlot[i]->Set_Dead(FALSE);
		PlayerInfo[i].bDead = FALSE;
		static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[i])->Set_MaxEnergy();
		m_pPageMain->Set_Hp(i, static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[i])->Get_CurrentHp());
		m_pPageMain->Set_BurstReady((PLAYERINDEX)i, TRUE);
	}
}

void CPlayer::Update_CurrentHp(PLAYERINDEX eIndex)
{
	_float fCurrentHp = static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[m_ePlayerNum])->Get_CurrentHp();
	m_pPageMain->Set_Hp(eIndex, fCurrentHp);
}

PLAYERINDEX CPlayer::Get_PlayerIndex(CREATUREID eId)
{
	_int eIndex = 0;

	for (auto& PlayerIndex : m_vecPlayerIndex)
	{
		if (PlayerIndex == eId)
			return static_cast<PLAYERINDEX>(eIndex);
		else
			eIndex += 1;
	}
	return PLAYER_END;
}

_int CPlayer::Get_BodyCollisionID()
{
	return m_Parts[PART_BODY].pPartySlot[m_ePlayerNum]->Get_CollisionID();
}

_int CPlayer::Get_BodyCollisionID(PLAYERINDEX eIndex)
{
	return m_Parts[PART_BODY].pPartySlot[eIndex]->Get_CollisionID();
}

_vector CPlayer::Get_Position()
{
	return PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_POSITION);
}

_vector CPlayer::Get_Position(PLAYERINDEX eIndex)
{
	return PlayerInfo[eIndex].pTransform->Get_State(CTransform::STATE_POSITION);
}

_vector CPlayer::Get_Look(PLAYERINDEX eIndex)
{
	return PlayerInfo[eIndex].pTransform->Get_State(CTransform::STATE_LOOK);
}

_matrix CPlayer::Get_WorldMatrix()
{
	return PlayerInfo[m_ePlayerNum].pTransform->Get_WorldMatrix();
}

_float CPlayer::Get_ElementDamage()
{
	return static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[m_ePlayerNum])->Get_Attack(CPart_Status::ATK_TYPE::ATK_ELEMENTAL);
}

_float CPlayer::Get_ElementDamage(PLAYERINDEX eIndex)
{
	return static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[eIndex])->Get_Attack(CPart_Status::ATK_TYPE::ATK_ELEMENTAL);
}

_float CPlayer::Get_BurstDamage()
{
	return static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[m_ePlayerNum])->Get_Attack(CPart_Status::ATK_TYPE::ATK_BURST);
}

FSM_STATE CPlayer::Get_State()
{
	return static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Get_CurrentState());
}

_bool CPlayer::Get_InAir()
{
	return PlayerInfo[m_ePlayerNum].pController->Get_InAir();
}

HRESULT CPlayer::Combat_Mode(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down('H')) /* 자동 전투 모드 */
	{
		m_bCombatMode = !m_bCombatMode;

		if (m_bCombatMode == TRUE) /* 자동 전투 모드 On */
		{
			PLAYERINDEX eCurIndex = m_ePlayerNum;
			_int		iPosSet = 0;

			for (_uint i = 0; i < PLAYER_END; ++i)
			{
				if (i != eCurIndex && PlayerInfo[(PLAYERINDEX)i].bDead != TRUE)
				{
					_vector vPlayerLook = PlayerInfo[eCurIndex].pTransform->Get_State(CTransform::STATE_LOOK);
					vPlayerLook.m128_f32[1] = 0.f; vPlayerLook = XMVector4Normalize(vPlayerLook);
					_vector vPlayerRight = PlayerInfo[eCurIndex].pTransform->Get_State(CTransform::STATE_RIGHT);
					vPlayerRight.m128_f32[1] = 0.f; vPlayerRight = XMVector4Normalize(vPlayerRight);

					_vector vPos;  _float4 fPos;
					vPos = PlayerInfo[eCurIndex].pTransform->Get_State(CTransform::STATE_POSITION);

					if (iPosSet == 0)
					{
						vPos += vPlayerLook * 3.f;
						vPos += vPlayerRight * 3.f;
					}
					else if (iPosSet == 1)
					{
						vPos += vPlayerLook * 3.f;
					}
					else
					{
						vPos += vPlayerLook * 3.f;
						vPos -= vPlayerRight * 3.f;
					}
					vPos.m128_f32[1] += 5.f;
					XMStoreFloat4(&fPos, vPos);

					PlayerInfo[(PLAYERINDEX)i].pTransform->Reset_Look_At_Direction();
					PlayerInfo[(PLAYERINDEX)i].pTransform->Set_Look_At_Direction(vPlayerLook, 100);
					PlayerInfo[(PLAYERINDEX)i].pController->Set_Position(fPos);
					PlayerInfo[(PLAYERINDEX)i].pController->Set_InGround(FALSE);
					PlayerInfo[(PLAYERINDEX)i].pFiniteStateMachineCom->Change_ForceState(FSM_FALL_ATTACK);
					PlayerInfo[(PLAYERINDEX)i].bAutoMode = TRUE;

					iPosSet++;
				}
			}
		}
		else /* 자동 전투 모드 Off */
		{
			PLAYERINDEX eCurIndex = m_ePlayerNum;

			for (_uint i = 0; i < PLAYER_END; ++i)
			{
				if (i != eCurIndex)
				{
					PlayerInfo[(PLAYERINDEX)i].bAutoMode = FALSE;
					PlayerInfo[(PLAYERINDEX)i].bRun = FALSE;
					PlayerInfo[(PLAYERINDEX)i].fKnockback = 0.f;
					PlayerInfo[(PLAYERINDEX)i].vKnockbackDir = { };
					PlayerInfo[(PLAYERINDEX)i].pController->Set_Position({ 0.f,-100.f,0.f,1.f });

					m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, m_Parts[PART_BODY].pPartySlot[(PLAYERINDEX)i]->Get_CollisionID(), FALSE);
					m_pGameInstance->Set_IsCollision(COLL_EVENTREACT_PLAYER, m_Parts[PART_BODY].pPartySlot[(PLAYERINDEX)i]->Get_CollisionID() + 1, FALSE);
				}
			}
		}
	}

	if (m_bCombatMode == TRUE)
	{
		PLAYERINDEX eCurIndex = m_ePlayerNum;

		for (_uint i = 0; i < PLAYER_END; ++i)
		{
			if (i != eCurIndex && PlayerInfo[(PLAYERINDEX)i].bDead != TRUE)
			{
				if (FAILED(CPlayer::Auto_Combat(fTimeDelta, (PLAYERINDEX)i)))
					return E_FAIL;
			}
		}
	}
	else
	{
		PLAYERINDEX eCurIndex = m_ePlayerNum;

		for (_uint i = 0; i < PLAYER_END; ++i)
		{
			if (PlayerInfo[(PLAYERINDEX)i].bDead != TRUE)
			{
				PlayerInfo[(PLAYERINDEX)i].bAutoMode = FALSE;
			}
		}
	}

	return S_OK;
}

HRESULT CPlayer::Auto_Combat(_float fTimeDelta, PLAYERINDEX eIndex)
{
	FSM_STATE eState = static_cast<FSM_STATE>(PlayerInfo[eIndex].pFiniteStateMachineCom->Get_CurrentState());

	if (eState == FSM_ATTACK_IDLE || eState == FSM_IDLE) /* 일정시간 지나면 자동으로 사라지게 */
	{
		m_fIdleTime += fTimeDelta;
		if (m_fIdleTime > 7.f)
		{
			PlayerInfo[eIndex].bAutoMode = FALSE;
			m_bCombatMode = FALSE;
			m_fIdleTime = 0.f;
		}
	}
	else
		m_fIdleTime = 0.f;

	if (eState >= FSM_FALL_ATTACK && eState <= FSM_FALL_ATTACK_END2)
		return S_OK;

	if (m_MonsterTarget.size() == 0)
	{
		if (m_ePlayerNum == PLAYER_THREE)
			XMStoreFloat4(&m_vTargetMonsterPos, PlayerInfo[eIndex].pTransform->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(PlayerInfo[eIndex].pTransform->Get_State(CTransform::STATE_LOOK)) * 3.f);

		if(eIndex == PLAYER_ONE || eIndex == PLAYER_THREE)
			PlayerInfo[eIndex].pFiniteStateMachineCom->Change_ForceState(FSM_ATTACK_IDLE);
		else
			PlayerInfo[eIndex].pFiniteStateMachineCom->Change_ForceState(FSM_IDLE);

		PlayerInfo[eIndex].bRun = FALSE;

		return S_OK;
	}

	if (eIndex == PLAYER_TWO && eState >= FSM_ATTACK_CHARGED1 && eState <= FSM_ATTACK_CHARGED_END)
		return S_OK;

	CTransform* pMonsterTransform = nullptr;
	_vector vCurrentPos = {};
	_vector vSetDir = {};
	_float  fCurrentDistance = 0.f, fMinDistance = 0.f;
	_vector vPlayerPos = PlayerInfo[eIndex].pTransform->Get_State(CTransform::STATE_POSITION);
	_int iIndex = 0, iMinIndex = 0;

	for (auto& pMonsterTarget : m_MonsterTarget)
	{
		if (pMonsterTarget->Get_Destroy() == TRUE)
			continue;

		pMonsterTransform = static_cast<CTransform*>(pMonsterTarget->Find_Component(TEXT("Com_Transform")));
		vCurrentPos = pMonsterTransform->Get_State(CTransform::STATE_POSITION);
		vSetDir = vCurrentPos - vPlayerPos;
		fCurrentDistance = XMVector3Length(vSetDir).m128_f32[0];

		if (iIndex == 0)
		{
			fMinDistance = fCurrentDistance;
			iMinIndex = iIndex;
		}

		if (fMinDistance >= fCurrentDistance)
		{
			iMinIndex = iIndex;
			fMinDistance = fCurrentDistance;
		}

		iIndex++;
	}

	pMonsterTransform = static_cast<CTransform*>(m_MonsterTarget[iMinIndex]->Find_Component(TEXT("Com_Transform")));
	vCurrentPos = pMonsterTransform->Get_State(CTransform::STATE_POSITION);
	XMStoreFloat4(&m_vTargetMonsterPos, vCurrentPos);
	vSetDir = vCurrentPos - vPlayerPos;
	vSetDir.m128_f32[1] = 0.f; vSetDir = XMVector3Normalize(vSetDir);
	_float4 fDir;
	XMStoreFloat4(&fDir, vSetDir);

	if (eState < FSM_HIT_L)
	{
		if (fMinDistance < 3.f && (eState < FSM_ATTACK_READY || eState > FSM_ATTACK6))
		{
			PlayerInfo[eIndex].bRun = FALSE;
			PlayerInfo[eIndex].pFiniteStateMachineCom->Change_ForceState(FSM_ATTACK_READY);
		}
		else if (fMinDistance > 3.f)
		{
			PlayerInfo[eIndex].bRun = TRUE;
		}
	}
	else
		PlayerInfo[eIndex].bRun = FALSE;

	if (fMinDistance > 1.f)
	{
		PlayerInfo[eIndex].pTransform->Reset_Look_At_Direction();
		PlayerInfo[eIndex].pTransform->Set_Look_At_Direction(vSetDir);
	}

	if (eState == FSM_RUN)
		PlayerInfo[eIndex].pController->Movement(fTimeDelta, fDir, 3.5f);

	return S_OK;
}

HRESULT CPlayer::Input_State(_float fTimeDelta)
{
	_bool bInput = { FALSE };
	FSM_STATE eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Get_CurrentState());
	CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager")));

	if (pUIManager->GetCurrentPageName() == UI_PAGE::PAGE_DIALOGUE)
	{
		PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Set_IdleState();
		return S_OK;
	}

	CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
	if (pCamera != nullptr)
	{
		if (pCamera->Get_CamScene() > CAMSCENEID_RAIDEN && pCamera->Get_CamScene() < CAMSCENEID_DVALIN
			&& pCamera->End_Scene(CAMSCENEID_DVALIN) == FALSE)
		{
			PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Set_IdleState();
			return S_OK;
		}
	}

	if (eState == FSM_IDLE || eState == FSM_ATTACK_IDLE)
	{
		m_pPageMain->Set_InputState(true);
	}
	else
	{
		m_pPageMain->Set_InputState(false);
	}

	if (FAILED(CPlayer::Change_Player()))
		return E_FAIL;

	if (m_pGameInstance->Key_Down('Z')) // 전체 부활
	{
		for (_uint i = 0; i < PLAYER_END; ++i)
		{
			if (m_Parts[PART_BODY].pPartySlot[i]->Get_Dead() == TRUE && PlayerInfo[i].bDead == TRUE)
			{
				m_Parts[PART_BODY].pPartySlot[i]->Set_Dead(FALSE);
				m_Parts[PART_BODY].pPartySlot[i]->Set_DissolveRatio(0.f);
				PlayerInfo[i].bDead = FALSE;
				PlayerInfo[i].pFiniteStateMachineCom->Change_State(FSM_IDLE);

				_float fMaxHp = static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[i])->Get_MaxHp();
				static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[i])->Set_Heal(fMaxHp);
				m_pPageMain->Set_Hp(i, static_cast<_float>(static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[i])->Get_Hp()));
			}
		}
	}

	if (m_pGameInstance->Key_Down('L'))
	{
		CGround_Item::DROP_ITEM_DESC	titemdesc = { };

		titemdesc.iItemIndex = 128;
		titemdesc.iItemQuantity = 1;
		titemdesc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Ground_Item"), L"Layer_Item", &titemdesc)))
			return E_FAIL;

		titemdesc.iItemIndex	= 127;
		titemdesc.iItemQuantity = 1;
		titemdesc.vPos			= m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Ground_Item"), L"Layer_Item", &titemdesc)))
			return E_FAIL;

		titemdesc.iItemIndex	= 124;
		titemdesc.iItemQuantity = 120000;
		titemdesc.vPos			= m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Ground_Item"), L"Layer_Item", &titemdesc)))
			return E_FAIL;

		titemdesc.iItemIndex	= 125;
		titemdesc.iItemQuantity = 500;
		titemdesc.vPos			= m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Ground_Item"), L"Layer_Item", &titemdesc)))
			return E_FAIL;

		titemdesc.iItemIndex	= 129;
		titemdesc.iItemQuantity = 1;
		titemdesc.vPos			= m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Ground_Item"), L"Layer_Item", &titemdesc)))
			return E_FAIL;

		for (_int iIndex = 131; iIndex < 147; ++iIndex)
		{
			titemdesc.iItemIndex	= iIndex;
			titemdesc.iItemQuantity = m_pGameInstance->Get_Int_Random(1, 10);
			titemdesc.vPos			= m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Ground_Item"), L"Layer_Item", &titemdesc)))
				return E_FAIL;
		}

		for (_int iIndex = 0; iIndex < 10; ++iIndex)
		{
			titemdesc.iItemIndex	= m_pGameInstance->Get_Int_Random(147, 172);
			titemdesc.iItemQuantity = m_pGameInstance->Get_Int_Random(1, 10);
			titemdesc.vPos			= m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Ground_Item"), L"Layer_Item", &titemdesc)))
				return E_FAIL;
		}

	}

#ifdef _DEBUG
	if (m_pGameInstance->Key_Down('M'))
	{
		if (m_isUndead == FALSE)
		{
			m_isUndead = TRUE;
			cout << "Undead Apply" << endl;
		}
		else
		{
			m_isUndead = FALSE;
			cout << "Undead Cancle" << endl;
		}
	}

	if (m_pGameInstance->Key_Down(VK_OEM_PERIOD)) /* .(점) 키*/
	{
		++m_iElement;
		if (m_iElement > ELEMENT_END)
			m_iElement = ELEMENT_FIRE;

		switch (m_iElement)
		{
		case 0:
			cout << "Basic Attack Element : Fire" << endl;
			break;
		case 1:
			cout << "Basic Attack Element : Water" << endl;
			break;
		case 2:
			cout << "Basic Attack Element : Wind" << endl;
			break;
		case 3:
			cout << "Basic Attack Element : Electric" << endl;
			break;
		case 4:
			cout << "Basic Attack Element : Frost" << endl;
			break;
		case 5:
			cout << "Basic Attack Element : Earth" << endl;
			break;
		case 6:
			cout << "Basic Attack Element : None" << endl;
			break;
		default:
			break;
		}
	}

	if (m_pGameInstance->Key_Down(VK_OEM_MINUS))
		m_pGameInstance->StopAll();

#endif
	/* Toggle Splint */
	if (m_pGameInstance->Key_Down(VK_CONTROL))
		m_bMoveRun = !m_bMoveRun;

#pragma region Jump State
	if (eState != FSM_FLY_NORMAL && eState != FSM_FLY_START && eState != FSM_JUMP && eState != FSM_HIT_THROW && eState != FSM_HIT_THROW_FLY && eState != FSM_HIT_ROLLING && m_pGameInstance->Key_Down(VK_SPACE))
	{
		if ((eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_JUMP))) == FSM_JUMP)
		{
			bInput = TRUE;
			//m_pGameInstance->Play_Particle(TEXT("Death"), TEXT("Death"), PARTICLE_DEFAULT, TEX_PARTICLE_DEFAULT, nullptr, PlayerInfo[m_ePlayerNum].pTransform, 2.f);
			//m_pGameInstance->Play_Particle(TEXT("i"), TEXT("i"), nullptr, PlayerInfo[m_ePlayerNum].pTransform, 1.f);

			_vector vCamLook = m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_VIEW).r[2];
			vCamLook.m128_f32[1] = 0.f; vCamLook = XMVector4Normalize(vCamLook);

			_vector vCamRight = m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_VIEW).r[0];
			vCamRight.m128_f32[1] = 0.f; vCamRight = XMVector4Normalize(vCamRight);

			_vector vDirection = { };
			if (m_pGameInstance->Key_Pressing('W'))
				vDirection += vCamLook;
			if (m_pGameInstance->Key_Pressing('S'))
				vDirection -= vCamLook;
			if (m_pGameInstance->Key_Pressing('A'))
				vDirection -= vCamRight;
			if (m_pGameInstance->Key_Pressing('D'))
				vDirection += vCamRight;

			_float4 vJumpDir;
			XMStoreFloat4(&vJumpDir, vDirection);

			PlayerInfo[m_ePlayerNum].pController->Ready_Jump(5.f);

			if (m_ePlayerNum == PLAYER_ONE)
			{
				_int iRandomVoice = m_pGameInstance->Get_Int_Random(1, 5);
				wstring soundFileName = L"VO_KO_Diluc_Jumping_0" + to_wstring(iRandomVoice) + L".ogg";
				m_pGameInstance->PlaySoundW(soundFileName.c_str(), 1.f);
			}
			else if (m_ePlayerNum == PLAYER_TWO)
			{
				_int iRandomVoice = m_pGameInstance->Get_Int_Random(1, 5);
				wstring soundFileName = L"VO_KO_Zhongli_Jumping_0" + to_wstring(iRandomVoice) + L".ogg";
				m_pGameInstance->PlaySoundW(soundFileName.c_str(), 1.f);
			}
			else if (m_ePlayerNum == PLAYER_THREE)
			{
				_int iRandomVoice = m_pGameInstance->Get_Int_Random(1, 5);
				wstring soundFileName = L"VO_KO_Barbara_Jumping_0" + to_wstring(iRandomVoice) + L".ogg";
				m_pGameInstance->PlaySoundW(soundFileName.c_str(), 1.f);
			}
			else if (m_ePlayerNum == PLAYER_FOUR)
			{
				_int iRandomVoice = m_pGameInstance->Get_Int_Random(1, 5);
				wstring soundFileName = L"VO_KO_Raiden_Shogun_Jumping_0" + to_wstring(iRandomVoice) + L".ogg";
				m_pGameInstance->PlaySoundW(soundFileName.c_str(), 1.f);
			}
		}
	}
	if (eState == FSM_JUMP)
	{
		if (bInput == FALSE)
			PlayerInfo[m_ePlayerNum].pTransform->Reset_Look_At_Direction();

		_vector vCamLook = m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_VIEW).r[2];
		vCamLook.m128_f32[1] = 0.f; vCamLook = XMVector4Normalize(vCamLook);

		_vector vCamRight = m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_VIEW).r[0];
		vCamRight.m128_f32[1] = 0.f; vCamRight = XMVector4Normalize(vCamRight);

		_vector vDirection = { };
		_float4 Dir;
		if (m_pGameInstance->Key_Pressing('W'))
		{
			vDirection += vCamLook;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamLook);
			XMStoreFloat4(&Dir, vCamLook);
			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 6.0f);
		}
		if (m_pGameInstance->Key_Pressing('S'))
		{
			vDirection -= vCamLook;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamLook * -1.f);
			XMStoreFloat4(&Dir, vCamLook * -1.f);
			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 6.0f);
		}
		if (m_pGameInstance->Key_Pressing('A'))
		{
			vDirection -= vCamRight;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamRight * -1.f);
			XMStoreFloat4(&Dir, vCamRight * -1.f);
			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 6.0f);
		}
		if (m_pGameInstance->Key_Pressing('D'))
		{
			vDirection += vCamRight;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamRight);
			XMStoreFloat4(&Dir, vCamRight);
			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 6.0f);
		}
	}
	if (PlayerInfo[m_ePlayerNum].pController->Get_InGround() && eState == FSM_JUMP && !PlayerInfo[m_ePlayerNum].pController->isRise())
		PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_ForceState(FSM_FALL_LOW);
#pragma endregion

#pragma region Fly State
	_vector vPlayerPos = PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_POSITION);
	//_float fPlayerPosY = vPlayerPos.m128_f32[1];
	_float fPlayerPosY = PlayerInfo[m_ePlayerNum].pController->Compute_Height();

	if (fPlayerPosY >= 2.f) // 임시 코드
	{
		if (eState != FSM_FLY_START && eState != FSM_FLY_LEFT && eState != FSM_FLY_RIGHT && eState != FSM_FLY_NORMAL/* && eState != FSM_DROP_DOWN*/
			&& m_pGameInstance->Key_Down(VK_SPACE))
		{
    		static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_ForceState(FSM_FLY_START));

			if (m_ePlayerNum == PLAYER_ONE)
			{
				_int iRandomVoice = m_pGameInstance->Get_Int_Random(1, 3);
				wstring soundFileName = L"VO_KO_Diluc_Deploying_Wind_Glider_0" + to_wstring(iRandomVoice) + L".ogg";
				m_pGameInstance->PlaySoundW(soundFileName.c_str(), 1.f);
			}
			else if (m_ePlayerNum == PLAYER_TWO)
			{
				_int iRandomVoice = m_pGameInstance->Get_Int_Random(1, 3);
				wstring soundFileName = L"VO_KO_Zhongli_Deploying_Wind_Glider_0" + to_wstring(iRandomVoice) + L".ogg";
				m_pGameInstance->PlaySoundW(soundFileName.c_str(), 1.f);
			}
			else if (m_ePlayerNum == PLAYER_THREE)
			{
				_int iRandomVoice = m_pGameInstance->Get_Int_Random(1, 3);
				wstring soundFileName = L"VO_KO_Barbara_Deploying_Wind_Glider_0" + to_wstring(iRandomVoice) + L".ogg";
				m_pGameInstance->PlaySoundW(soundFileName.c_str(), 1.f);
			}
			else if (m_ePlayerNum == PLAYER_FOUR)
			{
				_int iRandomVoice = m_pGameInstance->Get_Int_Random(1, 3);
				wstring soundFileName = L"VO_KO_Raiden_Shogun_Deploying_Wind_Glider_0" + to_wstring(iRandomVoice) + L".ogg";
				m_pGameInstance->PlaySoundW(soundFileName.c_str(), 1.f);
			}
		}
	}
	if (eState == FSM_FLY_NORMAL || eState == FSM_FLY_RIGHT || eState == FSM_FLY_LEFT || eState == FSM_DROP_DOWN
		|| eState == FSM_FALL_ATTACK_START1 || eState == FSM_FALL_ATTACK_START2 || eState == FSM_FALL_ATTACK)
	{
		if (bInput == FALSE)
			PlayerInfo[m_ePlayerNum].pTransform->Reset_Look_At_Direction();

		_vector vCamLook = m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_VIEW).r[2];
		vCamLook.m128_f32[1] = 0.f; vCamLook = XMVector4Normalize(vCamLook);

		_vector vCamRight = m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_VIEW).r[0];
		vCamRight.m128_f32[1] = 0.f; vCamRight = XMVector4Normalize(vCamRight);

		_vector vDirection = { };
		_float4 Dir;
		if (eState != FSM_DROP_DOWN && eState != FSM_FALL_ATTACK_START1 && eState != FSM_FALL_ATTACK_START2 && eState != FSM_FALL_ATTACK
			&& m_pGameInstance->Key_Pressing('W'))
		{
			vDirection += vCamLook;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamLook);
			static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_ForceState(FSM_FLY_NORMAL));
			XMStoreFloat4(&Dir, vCamLook);
			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 5.0f);
			
		}
		if (eState != FSM_DROP_DOWN && eState != FSM_FALL_ATTACK_START1 && eState != FSM_FALL_ATTACK_START2 && eState != FSM_FALL_ATTACK
			&& m_pGameInstance->Key_Pressing('S'))
		{
			vDirection -= vCamLook;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamLook * -1.f);
			static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_ForceState(FSM_FLY_NORMAL));
			XMStoreFloat4(&Dir, vCamLook * -1.f);
			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 5.0f);
			
		}
		if (eState != FSM_DROP_DOWN && eState != FSM_FALL_ATTACK_START1 && eState != FSM_FALL_ATTACK_START2 && eState != FSM_FALL_ATTACK
			&& m_pGameInstance->Key_Pressing('D'))
		{
			vDirection += vCamRight;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamRight);
			static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_ForceState(FSM_FLY_RIGHT));
			XMStoreFloat4(&Dir, vCamRight);
			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 5.0f);
			
		}
		if (eState != FSM_DROP_DOWN && eState != FSM_FALL_ATTACK_START1 && eState != FSM_FALL_ATTACK_START2 && eState != FSM_FALL_ATTACK
			&& m_pGameInstance->Key_Pressing('A'))
		{
			vDirection -= vCamRight;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamRight * -1.f);
			static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_ForceState(FSM_FLY_LEFT));
			XMStoreFloat4(&Dir, vCamRight * -1.f);
			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 5.0f);
			
		}

		if ((eState == FSM_FALL_ATTACK || eState == FSM_FALL_ATTACK_START2) 
			&& PlayerInfo[m_ePlayerNum].pController->CanFly() == FALSE)
			PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_ForceState(FSM_FALL_ATTACK_END1);
		else if ((eState == FSM_FALL_ATTACK_START1 )
			&& PlayerInfo[m_ePlayerNum].pController->CanFly() == FALSE)
			PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_ForceState(FSM_FALL_ATTACK_END1);
		else if(eState == FSM_DROP_DOWN && PlayerInfo[m_ePlayerNum].pController->CanFly() == FALSE)
			PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_ForceState(FSM_FALL_LOW);
		else if(eState != FSM_FALL_ATTACK && eState != FSM_FALL_ATTACK_START2 && 
			PlayerInfo[m_ePlayerNum].pController->CanFly() == FALSE)
			PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_ForceState(FSM_FALL_LOW);
	}
#pragma endregion

#pragma region Move State
	if (eState != FSM_FLY_NORMAL && eState != FSM_FLY_LEFT && eState != FSM_FLY_RIGHT && eState != FSM_FLY_START
		&& eState != FSM_DROP_DOWN && eState != FSM_FALL_ATTACK_START1 && eState != FSM_FALL_ATTACK_START2 && eState != FSM_FALL_ATTACK && eState != FSM_FALL_ATTACK_END1
		&& eState != FSM_ELEMENTAL_SKILL1 && eState != FSM_ELEMENTAL_SKILL2 && eState != FSM_ELEMENTAL_SKILL3 && eState != FSM_ELEMENTAL_BURST
		&& eState != FSM_JUMP && eState != FSM_HIT_H && eState != FSM_HIT_L && eState != FSM_HIT_THROW && eState != FSM_HIT_THROW_FLY 
		&& eState != FSM_DEAD1 && eState != FSM_DEAD2 && eState != FSM_SPLINT && eState != FSM_SHOW && eState != FSM_DASH && m_pGameInstance->Key_Down(VK_RBUTTON)) // 대시
	{
		m_fInvincibility[m_ePlayerNum] = INVINCIBILITY;
		bInput = TRUE;
		eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_ForceState(FSM_DASH));
		CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
		if (pCamera != nullptr && pCamera->Get_SceneId(CAMSCENEID_DVALIN) != SCENE_FOUR)
		{
			pCamera->Set_SceneMode(CAMSCENEID_END, SCENE_END, 0.f, 0.f, FALSE);
		}
	}
	if (eState == FSM_DASH)
	{
		_vector vPlayerLook = PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_LOOK);
		vPlayerLook.m128_f32[1] = 0.f; vPlayerLook = XMVector4Normalize(vPlayerLook);

		_float4 PlayerLook;
		XMStoreFloat4(&PlayerLook, vPlayerLook);

		PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vPlayerLook);
		if (m_ePrevState == FSM_DASH)
		{
			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, PlayerLook, 7.0f);
		}
		else
		{
			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, PlayerLook, 5.f);
		}
		m_ePrevState = eState;
	}


	if (eState != FSM_JUMP && (eState != FSM_FLY_NORMAL) && (eState != FSM_FLY_LEFT) && (eState != FSM_FLY_RIGHT)
		&& (eState != FSM_ATTACK_READY)
		&& m_pGameInstance->Key_Pressing('W') == TRUE && m_pGameInstance->Key_Pressing('S') == FALSE
		&&eState != FSM_ATTACK_READY && eState != FSM_ATTACK_CHARGED2 && eState != FSM_ATTACK_CHARGED_READY
		&& eState != FSM_ATTACK1 && eState != FSM_ATTACK2 && eState != FSM_ATTACK3 && eState != FSM_ATTACK4 && eState != FSM_ATTACK5
		&& eState !=FSM_ATTACK6 && eState !=FSM_ATTACK_CHARGED1 && eState != FSM_ELEMENTAL_BURST)
	{
		if (bInput == FALSE)
			PlayerInfo[m_ePlayerNum].pTransform->Reset_Look_At_Direction();

		_vector vCamLook = m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_VIEW).r[2];
		vCamLook.m128_f32[1] = 0.f; vCamLook = XMVector4Normalize(vCamLook);

		if (m_pGameInstance->Key_Pressing(VK_SHIFT) == TRUE) // 전력질주
		{
			eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_SPLINT));
			m_ePrevState = eState;
			bInput = TRUE;
		}
		FSM_STATE ePrevState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Get_PrevState());
		if (eState == FSM_SPLINT)
		{
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamLook);

			_float4 Dir;
			XMStoreFloat4(&Dir, vCamLook);
			if (ePrevState == FSM_SPLINT || static_cast<CPart_Body*>(m_Parts[PART_BODY].pPartySlot[m_ePlayerNum])->Get_ChangeAnim() == FALSE)
			{
				PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 10.0f);
			}
			else if (m_ePrevState == FSM_WALK)
			{
				PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 3.0f);
			}
			else
			{
				PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 5.0f);
			}
		}

		if (eState != FSM_DASH && eState != FSM_SPLINT && (m_bMoveRun == FALSE) && m_ePrevState == FSM_RUN && m_ePrevState != FSM_IDLE && m_ePrevState != FSM_RUN_STOP && eState != FSM_WALK)  // 달리다 걷기
		{
			eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_RUN_WALK));
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamLook);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamLook);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 4.0f);
		}
		else if (eState != FSM_DASH && m_pGameInstance->Key_Pressing(VK_SHIFT) == FALSE && m_ePrevState == FSM_SPLINT && eState != FSM_WALK && (m_bMoveRun == FALSE)) // 전력질주하다 달리다 걷기
		{
			eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_RUN_WALK));
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamLook);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamLook);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 4.0f);
		}
		else if (m_ePrevState == FSM_DASH && m_ePrevState != FSM_SPLINT && eState != FSM_WALK && (m_bMoveRun == FALSE)) // 대쉬하고 달리다 걷기
		{
			eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_RUN_WALK));
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamLook);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamLook);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 4.0f);
		}
		else if (eState != FSM_DASH && eState != FSM_SPLINT && (m_bMoveRun == FALSE) // 걷기
			&& (eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_WALK))) == FSM_WALK)
		{
			m_ePrevState = eState;
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamLook);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamLook);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 3.0f);
		}

		if (eState != FSM_RUN_WALK && eState != FSM_DASH && m_pGameInstance->Key_Pressing(VK_SHIFT) == FALSE && eState != FSM_RUN && m_ePrevState == FSM_SPLINT && (m_bMoveRun == TRUE)) // 전력질주하다 달리기
		{
			eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_SPLINT_RUN));
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamLook);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamLook);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 6.0f);
		}
		else if (eState != FSM_DASH && eState != FSM_SPLINT && (m_bMoveRun == TRUE) && m_ePrevState == FSM_WALK && m_ePrevState != FSM_IDLE && m_ePrevState != FSM_RUN_STOP && eState != FSM_RUN) // 걷다가 달리기
		{
			eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_WALK_RUN));
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamLook);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamLook);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 4.0f);
		}
		else if (m_ePrevState == FSM_DASH && eState != FSM_RUN && m_ePrevState != FSM_SPLINT && eState != FSM_WALK && (m_bMoveRun == TRUE)) // 대쉬하다 달리기
		{
			eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_SPLINT_RUN));
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamLook);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamLook);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 5.0f);
		}
		else if (eState != FSM_DASH && eState != FSM_SPLINT && (m_bMoveRun == TRUE) // 달리기
			&& (eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_RUN))) == FSM_RUN)
		{
			m_ePrevState = eState;
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamLook);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamLook);
			
			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 5.0f);
		}
	}

	if (eState != FSM_JUMP && (eState != FSM_FLY_NORMAL) && (eState != FSM_FLY_LEFT) && (eState != FSM_FLY_RIGHT)
		&& (eState != FSM_ATTACK_READY)
		&& m_pGameInstance->Key_Pressing('W') == FALSE && m_pGameInstance->Key_Pressing('S') == TRUE
		&& eState != FSM_ATTACK_READY && eState != FSM_ATTACK_CHARGED2 && eState != FSM_ATTACK_CHARGED_READY
		&& eState != FSM_ATTACK1 && eState != FSM_ATTACK2 && eState != FSM_ATTACK3 && eState != FSM_ATTACK4 && eState != FSM_ATTACK5
		&& eState != FSM_ATTACK6 && eState != FSM_ATTACK_CHARGED1 && eState != FSM_ELEMENTAL_BURST)
	{
		if (bInput == FALSE)
			PlayerInfo[m_ePlayerNum].pTransform->Reset_Look_At_Direction();

		_vector vCamLook = m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_VIEW).r[2];
		vCamLook.m128_f32[1] = 0.f; vCamLook = XMVector4Normalize(vCamLook);

		if (m_pGameInstance->Key_Pressing(VK_SHIFT) == TRUE) // 전력질주
		{
			eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_SPLINT));
			m_ePrevState = eState;
			bInput = TRUE;
		}
		FSM_STATE ePrevState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Get_PrevState());
		if (eState == FSM_SPLINT)
		{
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamLook * -1.f);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamLook * -1);
			if (ePrevState == FSM_SPLINT || static_cast<CPart_Body*>(m_Parts[PART_BODY].pPartySlot[m_ePlayerNum])->Get_ChangeAnim() == FALSE)
			{
				PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 10.0f);
			}
			else if (m_ePrevState == FSM_WALK)
			{
				PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 3.0f);
			}
			else
			{
				PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 5.0f);
			}
		}

		if (eState != FSM_DASH && eState != FSM_SPLINT && (m_bMoveRun == FALSE) && m_ePrevState == FSM_RUN && m_ePrevState != FSM_IDLE && m_ePrevState != FSM_RUN_STOP && eState != FSM_WALK)  // 달리다 걷기
		{
			eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_RUN_WALK));
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamLook * -1.f);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamLook * -1);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 4.0f);
		}
		else if (eState != FSM_DASH && m_pGameInstance->Key_Pressing(VK_SHIFT) == FALSE && m_ePrevState == FSM_SPLINT && eState != FSM_WALK && (m_bMoveRun == FALSE)) // 전력질주하다 달리다 걷기
		{
			eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_RUN_WALK));
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamLook * -1.f);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamLook * -1);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 4.0f);
		}
		else if (m_ePrevState == FSM_DASH && m_ePrevState != FSM_SPLINT && eState != FSM_WALK && (m_bMoveRun == FALSE)) // 대쉬하고 달리다 걷기
		{
			eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_RUN_WALK));
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamLook * -1.f);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamLook * -1);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 4.0f);
		}
		else if (eState != FSM_DASH && eState != FSM_SPLINT && (m_bMoveRun == FALSE) // 걷기
			&& (eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_WALK))) == FSM_WALK)
		{
			m_ePrevState = eState;
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamLook * -1.f);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamLook * -1);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 3.0f);
		}

		if (eState != FSM_RUN_WALK && eState != FSM_DASH && m_pGameInstance->Key_Pressing(VK_SHIFT) == FALSE && eState != FSM_RUN && m_ePrevState == FSM_SPLINT && (m_bMoveRun == TRUE)) // 전력질주하다 달리기
		{
			eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_SPLINT_RUN));
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamLook * -1.f);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamLook * -1);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 6.0f);
		}
		else if (eState != FSM_DASH && eState != FSM_SPLINT && (m_bMoveRun == TRUE) && m_ePrevState == FSM_WALK && m_ePrevState != FSM_IDLE && m_ePrevState != FSM_RUN_STOP && eState != FSM_RUN) // 걷다가 달리기
		{
			eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_WALK_RUN));
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamLook * -1.f);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamLook * -1);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 4.0f);
		}
		else if (m_ePrevState == FSM_DASH && eState != FSM_RUN && m_ePrevState != FSM_SPLINT && eState != FSM_WALK && (m_bMoveRun == TRUE)) // 대쉬하다 달리기
		{
			eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_SPLINT_RUN));
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamLook * -1.f);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamLook * -1);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 5.0f);
		}
		else if (eState != FSM_DASH && eState != FSM_SPLINT && (m_bMoveRun == TRUE) // 달리기
			&& (eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_RUN))) == FSM_RUN)
		{
			m_ePrevState = eState;
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamLook * -1.f);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamLook * -1);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 5.0f);
		}
	}

	if (eState != FSM_JUMP && (eState != FSM_FLY_NORMAL) && (eState != FSM_FLY_LEFT) && (eState != FSM_FLY_RIGHT)
		&& (eState != FSM_ATTACK_READY)
		&& m_pGameInstance->Key_Pressing('A') == TRUE && m_pGameInstance->Key_Pressing('D') == FALSE
		&& eState != FSM_ATTACK_READY && eState != FSM_ATTACK_CHARGED2 && eState != FSM_ATTACK_CHARGED_READY
		&& eState != FSM_ATTACK1 && eState != FSM_ATTACK2 && eState != FSM_ATTACK3 && eState != FSM_ATTACK4 && eState != FSM_ATTACK5
		&& eState != FSM_ATTACK6 && eState != FSM_ATTACK_CHARGED1 && eState != FSM_ELEMENTAL_BURST)
	{
		if (bInput == FALSE)
			PlayerInfo[m_ePlayerNum].pTransform->Reset_Look_At_Direction();

		_vector vCamRight = m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_VIEW).r[0];
		vCamRight.m128_f32[1] = 0.f; vCamRight = XMVector4Normalize(vCamRight);

		if (m_pGameInstance->Key_Pressing(VK_SHIFT) == TRUE) // 전력질주
		{
			eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_SPLINT));
			m_ePrevState = eState;
			bInput = TRUE;
		}

		FSM_STATE ePrevState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Get_PrevState());
		if (eState == FSM_SPLINT)
		{
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamRight * -1.f);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamRight * -1);
			if (ePrevState == FSM_SPLINT || static_cast<CPart_Body*>(m_Parts[PART_BODY].pPartySlot[m_ePlayerNum])->Get_ChangeAnim() == FALSE)
			{
				PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 10.0f);
			}
			else if (m_ePrevState == FSM_WALK)
			{
				PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 3.0f);
			}
			else
			{
				PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 5.0f);
			}
		}

		if (eState != FSM_DASH && eState != FSM_SPLINT && (m_bMoveRun == FALSE) && m_ePrevState == FSM_RUN && m_ePrevState != FSM_IDLE && m_ePrevState != FSM_RUN_STOP && eState != FSM_WALK)  // 달리다 걷기
		{
			eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_RUN_WALK));
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamRight * -1.f);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamRight * -1);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 4.0f);
		}
		else if (eState != FSM_DASH && m_pGameInstance->Key_Pressing(VK_SHIFT) == FALSE && m_ePrevState == FSM_SPLINT && eState != FSM_WALK && (m_bMoveRun == FALSE)) // 전력질주하다 달리다 걷기
		{
			eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_RUN_WALK));
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamRight * -1.f);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamRight * -1);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 4.0f);
		}
		else if (m_ePrevState == FSM_DASH && m_ePrevState != FSM_SPLINT && eState != FSM_WALK && (m_bMoveRun == FALSE)) // 대쉬하고 달리다 걷기
		{
			eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_RUN_WALK));
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamRight * -1.f);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamRight * -1);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 4.0f);
		}
		else if (eState != FSM_DASH && eState != FSM_SPLINT && (m_bMoveRun == FALSE) // 걷기
			&& (eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_WALK))) == FSM_WALK)
		{
			m_ePrevState = eState;
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamRight * -1.f);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamRight * -1);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 3.0f);
		}

		if (eState != FSM_RUN_WALK && eState != FSM_DASH && m_pGameInstance->Key_Pressing(VK_SHIFT) == FALSE && eState != FSM_RUN && m_ePrevState == FSM_SPLINT && (m_bMoveRun == TRUE)) // 전력질주하다 달리기
		{
			eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_SPLINT_RUN));
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamRight * -1.f);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamRight * -1);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 6.0f);
			
		}
		else if (eState != FSM_DASH && eState != FSM_SPLINT && (m_bMoveRun == TRUE) && m_ePrevState == FSM_WALK && m_ePrevState != FSM_IDLE && m_ePrevState != FSM_RUN_STOP && eState != FSM_RUN) // 걷다가 달리기
		{
			eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_WALK_RUN));
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamRight * -1.f);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamRight * -1);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 4.0f);
			
		}
		else if (m_ePrevState == FSM_DASH && eState != FSM_RUN && m_ePrevState != FSM_SPLINT && eState != FSM_WALK && (m_bMoveRun == TRUE)) // 대쉬하다 달리기
		{
			eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_SPLINT_RUN));
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamRight * -1.f);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamRight * -1);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 5.0f);
			
		}
		else if (eState != FSM_DASH && eState != FSM_SPLINT && (m_bMoveRun == TRUE) // 달리기
			&& (eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_RUN))) == FSM_RUN)
		{
			m_ePrevState = eState;
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamRight * -1.f);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamRight * -1);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 5.0f);
			
		}
	}

	if (eState != FSM_JUMP && (eState != FSM_FLY_NORMAL) && (eState != FSM_FLY_LEFT) && (eState != FSM_FLY_RIGHT)
		&& (eState != FSM_ATTACK_READY)
		&& m_pGameInstance->Key_Pressing('A') == FALSE && m_pGameInstance->Key_Pressing('D') == TRUE
		&& eState != FSM_ATTACK_READY && eState != FSM_ATTACK_CHARGED2 && eState != FSM_ATTACK_CHARGED_READY
		&& eState != FSM_ATTACK1 && eState != FSM_ATTACK2 && eState != FSM_ATTACK3 && eState != FSM_ATTACK4 && eState != FSM_ATTACK5
		&& eState != FSM_ATTACK6 && eState != FSM_ATTACK_CHARGED1 && eState != FSM_ELEMENTAL_BURST)
	{
		if (bInput == FALSE)
			PlayerInfo[m_ePlayerNum].pTransform->Reset_Look_At_Direction();

		_vector vCamRight = m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_VIEW).r[0];
		vCamRight.m128_f32[1] = 0.f; vCamRight = XMVector4Normalize(vCamRight);

		if (m_pGameInstance->Key_Pressing(VK_SHIFT) == TRUE) // 전력질주
		{
			eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_SPLINT));
			m_ePrevState = eState;
			bInput = TRUE;
		}

		FSM_STATE ePrevState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Get_PrevState());
		if (eState == FSM_SPLINT)
		{
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamRight);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamRight);
			if (ePrevState == FSM_SPLINT || static_cast<CPart_Body*>(m_Parts[PART_BODY].pPartySlot[m_ePlayerNum])->Get_ChangeAnim() == FALSE)
			{
				PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 10.0f);
			}
			else if (m_ePrevState == FSM_WALK)
			{
				PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 3.0f);
			}
			else
			{
				PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 5.0f);
			}
		}

		if (eState != FSM_DASH && eState != FSM_SPLINT && (m_bMoveRun == FALSE) && m_ePrevState == FSM_RUN && m_ePrevState != FSM_IDLE && m_ePrevState != FSM_RUN_STOP && eState != FSM_WALK)  // 달리다 걷기
		{
			eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_RUN_WALK));
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamRight);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamRight);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 4.0f);
			
		}
		else if (eState != FSM_DASH && m_pGameInstance->Key_Pressing(VK_SHIFT) == FALSE && m_ePrevState == FSM_SPLINT && eState != FSM_WALK && (m_bMoveRun == FALSE)) // 전력질주하다 달리다 걷기
		{
			eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_RUN_WALK));
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamRight);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamRight);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 4.0f);
			
		}
		else if (m_ePrevState == FSM_DASH && m_ePrevState != FSM_SPLINT && eState != FSM_WALK && (m_bMoveRun == FALSE)) // 대쉬하고 달리다 걷기
		{
			eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_RUN_WALK));
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamRight);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamRight);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 4.0f);
			
		}
		else if (eState != FSM_DASH && eState != FSM_SPLINT && (m_bMoveRun == FALSE) // 걷기
			&& (eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_WALK))) == FSM_WALK)
		{
			m_ePrevState = eState;
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamRight);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamRight);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 3.0f);
			
		}

		if (eState != FSM_RUN_WALK && eState != FSM_DASH && m_pGameInstance->Key_Pressing(VK_SHIFT) == FALSE && eState != FSM_RUN && m_ePrevState == FSM_SPLINT && (m_bMoveRun == TRUE)) // 전력질주하다 달리기
		{
			eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_SPLINT_RUN));
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamRight);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamRight);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 6.0f);
			
		}
		else if (eState != FSM_DASH && eState != FSM_SPLINT && (m_bMoveRun == TRUE) && m_ePrevState == FSM_WALK && m_ePrevState != FSM_IDLE && m_ePrevState != FSM_RUN_STOP && eState != FSM_RUN) // 걷다가 달리기
		{
			eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_WALK_RUN));
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamRight);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamRight);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 4.0f);
			
		}
		else if (m_ePrevState == FSM_DASH && eState != FSM_RUN && m_ePrevState != FSM_SPLINT && eState != FSM_WALK && (m_bMoveRun == TRUE)) // 대쉬하다 달리기
		{
			eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_SPLINT_RUN));
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamRight);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamRight);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 5.0f);
			
		}
		else if (eState != FSM_DASH && eState != FSM_SPLINT && (m_bMoveRun == TRUE) // 달리기
			&& (eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_RUN))) == FSM_RUN)
		{
			m_ePrevState = eState;
			bInput = TRUE;
			PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamRight);
			_float4 Dir;
			XMStoreFloat4(&Dir, vCamRight);

			PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, Dir, 5.0f);
			
		}
	}
#pragma endregion

#pragma region Attack State
	
	if (FAILED(CPlayer::Attack_Skill(fTimeDelta)))
		return E_FAIL;

#pragma endregion

	/* 걷다가 멈춤 (어색함)
	if ((eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Get_CurrentState())) == FSM_WALK)
	{
		if (bInput == FALSE)
			m_ePrevState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_WALK_IDLE));
	} */

	/*CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
	if (pCamera != nullptr && bInput == TRUE)
	{
		pCamera->Set_SceneMode(CAMSCENEID_END, SCENE_END, 0.f, 0.f, false);
	}*/

	if ((eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Get_CurrentState())) == FSM_RUN)
	{
		if (bInput == FALSE)
			m_ePrevState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_ForceState(FSM_RUN_STOP));
	}

	if ((eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Get_CurrentState())) == FSM_SPLINT || eState == FSM_SPLINT_RUN) // 전력질주하다 멈추기
	{
		if (bInput == FALSE)
			m_ePrevState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_ForceState(FSM_RUN_STOP));
	}

	if ((eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Get_CurrentState())) != FSM_ATTACK_IDLE && eState != FSM_ATTACK_READY
		&& eState != FSM_ATTACK_CHARGED_READY && eState != FSM_DASH
		&& eState != FSM_ATTACK_END && eState != FSM_ATTACK_END2 && eState != FSM_ATTACK_CHARGED_END
		&& eState != FSM_WALK_IDLE && eState != FSM_RUN_STOP
		&& eState != FSM_ELEMENTAL_SKILL1_IDLE && eState != FSM_ELEMENTAL_SKILL2_IDLE && eState != FSM_ELEMENTAL_SKILL3_IDLE
		&& eState != FSM_ELEMENTAL_BURST_IDLE && eState != FSM_FALL_LOW)
	{
		if (bInput == FALSE)
			m_ePrevState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_IDLE));
	}

	if ((eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Get_CurrentState())) == FSM_ATTACK_IDLE)
	{
		m_fAttackReadyTime += fTimeDelta;
		if (m_fAttackReadyTime > 4.f && bInput == FALSE)
		{
			static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_ATTACK_END));
			m_fAttackReadyTime = 0.f;
		}
	}
	else
		m_fAttackReadyTime = 0.f;

	if (eState == FSM_RUN_STOP)
		m_ePrevState = eState;

	
	if (bInput == FALSE)
	{
		m_bSplintSound = FALSE;
	}

	return S_OK;
}

HRESULT CPlayer::Change_Player()
{
	FSM_STATE eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Get_CurrentState());

	if (m_pGameInstance->Key_Down('1'))
	{
		if (m_Parts[PART_BODY].pPartySlot[PLAYER_ONE]->Get_Dead() == FALSE && PlayerInfo[PLAYER_ONE].bDead == FALSE)
		{
			if (m_ePlayerNum != PLAYER_ONE && 
				(eState == FSM_IDLE || eState == FSM_ATTACK_IDLE || eState == FSM_WALK_IDLE || eState == FSM_RUN_STOP))
			{
				
				m_pElement_OHD->Zero_Elements();
				for (_uint i = 0; i < ELEMENT_END; ++i)
				{
					if (m_Parts[PART_BODY].pPartySlot[PLAYER_ONE]->isHostElement((ELEMENT)i) == TRUE)
						CPlayer::Set_ElementUI((ELEMENT)i);
				}
				if (m_bCombatMode == TRUE) /* 자동 전투 */
				{
					//_vector vCurLook = PlayerInfo[PLAYER_ONE].pTransform->Get_State(CTransform::STATE_LOOK);
					//vCurLook.m128_f32[1] = 0.f; vCurLook = XMVector4Normalize(vCurLook);
					//_float4 fCurPos;
					//XMStoreFloat4(&fCurPos, PlayerInfo[PLAYER_ONE].pTransform->Get_State(CTransform::STATE_POSITION));

					///* 바뀔 Player */
					//_vector vPlayerLook = PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_LOOK);
					//vPlayerLook.m128_f32[1] = 0.f; vPlayerLook = XMVector4Normalize(vPlayerLook);
					//PlayerInfo[PLAYER_ONE].pTransform->Reset_Look_At_Direction();
					//PlayerInfo[PLAYER_ONE].pTransform->Set_Look_At_Direction(vPlayerLook, 100);
					//_float4 fPos;
					//XMStoreFloat4(&fPos, PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_POSITION));
					//PlayerInfo[PLAYER_ONE].pController->Set_Position(fPos);
					PlayerInfo[PLAYER_ONE].bAutoMode = FALSE;

					/* 이전 Player */
					//PlayerInfo[m_ePlayerNum].pTransform->Reset_Look_At_Direction();
					//PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCurLook, 100);
					//PlayerInfo[m_ePlayerNum].pController->Set_Position(fCurPos);
					PlayerInfo[m_ePlayerNum].bAutoMode = TRUE;
				}
				else
				{
					_vector vPlayerLook = PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_LOOK);
					vPlayerLook.m128_f32[1] = 0.f; vPlayerLook = XMVector4Normalize(vPlayerLook);
					PlayerInfo[PLAYER_ONE].pTransform->Reset_Look_At_Direction();
					PlayerInfo[PLAYER_ONE].pTransform->Set_Look_At_Direction(vPlayerLook, 100);
					_float4 fPos;
					XMStoreFloat4(&fPos, PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_POSITION));
					PlayerInfo[PLAYER_ONE].pController->Set_Position(fPos);

					PlayerInfo[m_ePlayerNum].pController->Set_Position({ 0.f,-100.f,0.f,1.f });
					m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, m_Parts[PART_BODY].pPartySlot[m_ePlayerNum]->Get_CollisionID(), FALSE);
					m_pGameInstance->Set_IsCollision(COLL_EVENTREACT_PLAYER, m_Parts[PART_BODY].pPartySlot[m_ePlayerNum]->Get_CollisionID() + 1, FALSE);
				}

				m_ePlayerNum = PLAYER_ONE;
				m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, m_Parts[PART_BODY].pPartySlot[m_ePlayerNum]->Get_CollisionID(), TRUE);
				m_pGameInstance->Set_IsCollision(COLL_EVENTREACT_PLAYER, m_Parts[PART_BODY].pPartySlot[m_ePlayerNum]->Get_CollisionID() + 1, TRUE);
				PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_ForceState(FSM_SHOW);
				m_pPageMain->Set_MemberIndex(PLAYER_ONE);

				_int iRandomVoice = m_pGameInstance->Get_Int_Random(1, 3);
				wstring soundFileName = L"VO_KO_Diluc_Joining_Party_0" + to_wstring(iRandomVoice) + L".ogg";
				m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
				m_pGameInstance->PlaySoundW(soundFileName.c_str(), 1.f, CHANNELID::SOUND_VOICE);
			}
			else
			{

			}
		}
	}
	else if (m_pGameInstance->Key_Down('2'))
	{
		if (m_Parts[PART_BODY].pPartySlot[PLAYER_TWO]->Get_Dead() == FALSE && PlayerInfo[PLAYER_TWO].bDead == FALSE)
		{
			if (m_ePlayerNum != PLAYER_TWO && 
				(eState == FSM_IDLE || eState == FSM_ATTACK_IDLE || eState == FSM_WALK_IDLE || eState == FSM_RUN_STOP))
			{
				m_pElement_OHD->Zero_Elements();
				for (_uint i = 0; i < ELEMENT_END; ++i)
				{
					if (m_Parts[PART_BODY].pPartySlot[PLAYER_ONE]->isHostElement((ELEMENT)i) == TRUE)
						CPlayer::Set_ElementUI((ELEMENT)i);
				}
				if (m_bCombatMode == TRUE) /* 자동 전투 */
				{
					//_vector vCurLook = PlayerInfo[PLAYER_TWO].pTransform->Get_State(CTransform::STATE_LOOK);
					//vCurLook.m128_f32[1] = 0.f; vCurLook = XMVector4Normalize(vCurLook);
					//_float4 fCurPos;
					//XMStoreFloat4(&fCurPos, PlayerInfo[PLAYER_TWO].pTransform->Get_State(CTransform::STATE_POSITION));

					///* 바뀔 Player */
					//_vector vPlayerLook = PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_LOOK);
					//vPlayerLook.m128_f32[1] = 0.f; vPlayerLook = XMVector4Normalize(vPlayerLook);
					//PlayerInfo[PLAYER_TWO].pTransform->Reset_Look_At_Direction();
					//PlayerInfo[PLAYER_TWO].pTransform->Set_Look_At_Direction(vPlayerLook, 100);
					//_float4 fPos;
					//XMStoreFloat4(&fPos, PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_POSITION));
					//PlayerInfo[PLAYER_TWO].pController->Set_Position(fPos);
					PlayerInfo[PLAYER_TWO].bAutoMode = FALSE;

					/* 이전 Player */
					//PlayerInfo[m_ePlayerNum].pTransform->Reset_Look_At_Direction();
					//PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCurLook, 100);
					//PlayerInfo[m_ePlayerNum].pController->Set_Position(fCurPos);
					PlayerInfo[m_ePlayerNum].bAutoMode = TRUE;
				}
				else
				{
					_vector vPlayerLook = PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_LOOK);
					vPlayerLook.m128_f32[1] = 0.f; vPlayerLook = XMVector4Normalize(vPlayerLook);
					PlayerInfo[PLAYER_TWO].pTransform->Reset_Look_At_Direction();
					PlayerInfo[PLAYER_TWO].pTransform->Set_Look_At_Direction(vPlayerLook, 100);
					_float4 fPos;
					XMStoreFloat4(&fPos, PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_POSITION));
					PlayerInfo[PLAYER_TWO].pController->Set_Position(fPos);

					PlayerInfo[m_ePlayerNum].pController->Set_Position({ 0.f,-100.f,0.f,1.f });
					m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, m_Parts[PART_BODY].pPartySlot[m_ePlayerNum]->Get_CollisionID(), FALSE);
					m_pGameInstance->Set_IsCollision(COLL_EVENTREACT_PLAYER, m_Parts[PART_BODY].pPartySlot[m_ePlayerNum]->Get_CollisionID() + 1, FALSE);
				}

				m_ePlayerNum = PLAYER_TWO;
				m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, m_Parts[PART_BODY].pPartySlot[m_ePlayerNum]->Get_CollisionID(), TRUE);
				m_pGameInstance->Set_IsCollision(COLL_EVENTREACT_PLAYER, m_Parts[PART_BODY].pPartySlot[m_ePlayerNum]->Get_CollisionID() + 1, TRUE);
				PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_ForceState(FSM_SHOW);
				m_pPageMain->Set_MemberIndex(PLAYER_TWO);

				_int iRandomVoice = m_pGameInstance->Get_Int_Random(1, 3);
				wstring soundFileName = L"VO_KO_Zhongli_Joining_Party_0" + to_wstring(iRandomVoice) + L".ogg";
				m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
				m_pGameInstance->PlaySoundW(soundFileName.c_str(), 1.f, CHANNELID::SOUND_VOICE);
			}
			else
			{

				// 캐릭터 바꿀 수 없다는 경고문 떠야함
			}
		}
	}
	else if (m_pGameInstance->Key_Down('3'))
	{
		if (m_Parts[PART_BODY].pPartySlot[PLAYER_THREE]->Get_Dead() == FALSE && PlayerInfo[PLAYER_THREE].bDead == FALSE)
		{
			if (m_ePlayerNum != PLAYER_THREE && 
				(eState == FSM_IDLE || eState == FSM_ATTACK_IDLE || eState == FSM_WALK_IDLE || eState == FSM_RUN_STOP))
			{
				m_pElement_OHD->Zero_Elements();
				for (_uint i = 0; i < ELEMENT_END; ++i)
				{
					if (m_Parts[PART_BODY].pPartySlot[PLAYER_ONE]->isHostElement((ELEMENT)i) == TRUE)
						CPlayer::Set_ElementUI((ELEMENT)i);
				}
				if (m_bCombatMode == TRUE) /* 자동 전투 */
				{
					//_vector vCurLook = PlayerInfo[PLAYER_THREE].pTransform->Get_State(CTransform::STATE_LOOK);
					//vCurLook.m128_f32[1] = 0.f; vCurLook = XMVector4Normalize(vCurLook);
					//_float4 fCurPos;
					//XMStoreFloat4(&fCurPos, PlayerInfo[PLAYER_THREE].pTransform->Get_State(CTransform::STATE_POSITION));

					///* 바뀔 Player */
					//_vector vPlayerLook = PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_LOOK);
					//vPlayerLook.m128_f32[1] = 0.f; vPlayerLook = XMVector4Normalize(vPlayerLook);
					//PlayerInfo[PLAYER_THREE].pTransform->Reset_Look_At_Direction();
					//PlayerInfo[PLAYER_THREE].pTransform->Set_Look_At_Direction(vPlayerLook, 100);
					//_float4 fPos;
					//XMStoreFloat4(&fPos, PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_POSITION));
					//PlayerInfo[PLAYER_THREE].pController->Set_Position(fPos);
					PlayerInfo[PLAYER_THREE].bAutoMode = FALSE;

					/* 이전 Player */
					//PlayerInfo[m_ePlayerNum].pTransform->Reset_Look_At_Direction();
					//PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCurLook, 100);
					//PlayerInfo[m_ePlayerNum].pController->Set_Position(fCurPos);
					PlayerInfo[m_ePlayerNum].bAutoMode = TRUE;
				}
				else
				{
					_vector vPlayerLook = PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_LOOK);
					vPlayerLook.m128_f32[1] = 0.f; vPlayerLook = XMVector4Normalize(vPlayerLook);
					PlayerInfo[PLAYER_THREE].pTransform->Reset_Look_At_Direction();
					PlayerInfo[PLAYER_THREE].pTransform->Set_Look_At_Direction(vPlayerLook, 100);
					_float4 fPos;
					XMStoreFloat4(&fPos, PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_POSITION));
					PlayerInfo[PLAYER_THREE].pController->Set_Position(fPos);

					PlayerInfo[m_ePlayerNum].pController->Set_Position({ 0.f,-100.f,0.f,1.f });
					m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, m_Parts[PART_BODY].pPartySlot[m_ePlayerNum]->Get_CollisionID(), FALSE);
					m_pGameInstance->Set_IsCollision(COLL_EVENTREACT_PLAYER, m_Parts[PART_BODY].pPartySlot[m_ePlayerNum]->Get_CollisionID() + 1, FALSE);
				}

				m_ePlayerNum = PLAYER_THREE;
				m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, m_Parts[PART_BODY].pPartySlot[m_ePlayerNum]->Get_CollisionID(), TRUE);
				m_pGameInstance->Set_IsCollision(COLL_EVENTREACT_PLAYER, m_Parts[PART_BODY].pPartySlot[m_ePlayerNum]->Get_CollisionID() + 1, TRUE);
				PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_ForceState(FSM_SHOW);
				m_pPageMain->Set_MemberIndex(PLAYER_THREE);

				_int iRandomVoice = m_pGameInstance->Get_Int_Random(1, 3);
				wstring soundFileName = L"VO_KO_Barbara_Joining_Party_0" + to_wstring(iRandomVoice) + L".ogg";
				m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
				m_pGameInstance->PlaySoundW(soundFileName.c_str(), 1.f, CHANNELID::SOUND_VOICE);
			}
			else
			{

				// 캐릭터 바꿀 수 없다는 경고문 떠야함
			}
		}
	}
	else if (m_pGameInstance->Key_Down('4'))
	{
		if (m_Parts[PART_BODY].pPartySlot[PLAYER_FOUR]->Get_Dead() == FALSE && PlayerInfo[PLAYER_FOUR].bDead == FALSE)
		{
			if (m_ePlayerNum != PLAYER_FOUR &&
				(eState == FSM_IDLE || eState == FSM_ATTACK_IDLE || eState == FSM_WALK_IDLE || eState == FSM_RUN_STOP))
			{
				m_pElement_OHD->Zero_Elements();
				for (_uint i = 0; i < ELEMENT_END; ++i)
				{
					if (m_Parts[PART_BODY].pPartySlot[PLAYER_ONE]->isHostElement((ELEMENT)i) == TRUE)
						CPlayer::Set_ElementUI((ELEMENT)i);
				}
				if (m_bCombatMode == TRUE) /* 자동 전투 */
				{
					//_vector vCurLook = PlayerInfo[PLAYER_THREE].pTransform->Get_State(CTransform::STATE_LOOK);
					//vCurLook.m128_f32[1] = 0.f; vCurLook = XMVector4Normalize(vCurLook);
					//_float4 fCurPos;
					//XMStoreFloat4(&fCurPos, PlayerInfo[PLAYER_THREE].pTransform->Get_State(CTransform::STATE_POSITION));

					///* 바뀔 Player */
					//_vector vPlayerLook = PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_LOOK);
					//vPlayerLook.m128_f32[1] = 0.f; vPlayerLook = XMVector4Normalize(vPlayerLook);
					//PlayerInfo[PLAYER_THREE].pTransform->Reset_Look_At_Direction();
					//PlayerInfo[PLAYER_THREE].pTransform->Set_Look_At_Direction(vPlayerLook, 100);
					//_float4 fPos;
					//XMStoreFloat4(&fPos, PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_POSITION));
					//PlayerInfo[PLAYER_THREE].pController->Set_Position(fPos);
					PlayerInfo[PLAYER_FOUR].bAutoMode = FALSE;

					/* 이전 Player */
					//PlayerInfo[m_ePlayerNum].pTransform->Reset_Look_At_Direction();
					//PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCurLook, 100);
					//PlayerInfo[m_ePlayerNum].pController->Set_Position(fCurPos);
					PlayerInfo[m_ePlayerNum].bAutoMode = TRUE;
				}
				else
				{
					_vector vPlayerLook = PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_LOOK);
					vPlayerLook.m128_f32[1] = 0.f; vPlayerLook = XMVector4Normalize(vPlayerLook);
					PlayerInfo[PLAYER_FOUR].pTransform->Reset_Look_At_Direction();
					PlayerInfo[PLAYER_FOUR].pTransform->Set_Look_At_Direction(vPlayerLook, 100);
					_float4 fPos;
					XMStoreFloat4(&fPos, PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_POSITION));
					PlayerInfo[PLAYER_FOUR].pController->Set_Position(fPos);

					PlayerInfo[m_ePlayerNum].pController->Set_Position({ 0.f,-100.f,0.f,1.f });
					m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, m_Parts[PART_BODY].pPartySlot[m_ePlayerNum]->Get_CollisionID(), FALSE);
					m_pGameInstance->Set_IsCollision(COLL_EVENTREACT_PLAYER, m_Parts[PART_BODY].pPartySlot[m_ePlayerNum]->Get_CollisionID() + 1, FALSE);
				}

				m_ePlayerNum = PLAYER_FOUR;
				m_pGameInstance->Set_IsCollision(COLL_EVENTREACT_PLAYER, m_Parts[PART_BODY].pPartySlot[m_ePlayerNum]->Get_CollisionID() + 1, TRUE);
				PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_ForceState(FSM_SHOW);
				m_pPageMain->Set_MemberIndex(PLAYER_FOUR);

				_int iRandomVoice = m_pGameInstance->Get_Int_Random(1, 3);
				wstring soundFileName = L"VO_KO_Raiden_Shogun_Joining_Party_0" + to_wstring(iRandomVoice) + L".ogg";
				m_pGameInstance->StopSound(CHANNELID::SOUND_VOICE);
				m_pGameInstance->PlaySoundW(soundFileName.c_str(), 1.f, CHANNELID::SOUND_VOICE);
			}
			else
			{

				// 캐릭터 바꿀 수 없다는 경고문 떠야함
			}
		}
		}

	return S_OK;
}

HRESULT CPlayer::Attack_Skill(_float fTimeDelta)
{
	_bool bInput = { FALSE };
	FSM_STATE eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Get_CurrentState());

	if ((eState != FSM_ATTACK_READY) &&
		(eState != FSM_ATTACK_CHARGED_READY) &&
		(eState != FSM_ATTACK1) &&
		(eState != FSM_ATTACK2) &&
		(eState != FSM_ATTACK3) &&
		(eState != FSM_ATTACK4) &&
		(eState != FSM_ATTACK5) &&
		(eState != FSM_ATTACK6) &&
		(eState != FSM_ATTACK_CHARGED1) &&
		(eState != FSM_ATTACK_CHARGED2) &&
		(eState != FSM_ELEMENTAL_SKILL1) &&
		(eState != FSM_ELEMENTAL_SKILL2) &&
		(eState != FSM_ELEMENTAL_SKILL3) &&
		(eState != FSM_ELEMENTAL_SKILL1_IDLE) &&
		(eState != FSM_ELEMENTAL_SKILL2_IDLE) &&
		(eState != FSM_ELEMENTAL_SKILL3_IDLE) &&
		(eState != FSM_ELEMENTAL_BURST) &&
		(eState != FSM_FLY_START) &&
		(eState != FSM_FLY_NORMAL) &&
		(eState != FSM_FLY_LEFT) &&
		(eState != FSM_FLY_RIGHT) &&
		(eState != FSM_FALL_ATTACK) &&
		(eState != FSM_DASH) &&
		(eState != FSM_SPLINT_RUN))
	{
		if (m_pGameInstance->Key_Down(VK_LBUTTON))
		{
			bInput = TRUE;
			m_iExtraAttackCount = 0;
			static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_ATTACK_READY));
		}
	}

	if ((eState != FSM_ATTACK_READY) &&
		(eState != FSM_ATTACK_CHARGED_READY) &&
		(eState != FSM_ATTACK1) &&
		(eState != FSM_ATTACK2) &&
		(eState != FSM_ATTACK3) &&
		(eState != FSM_ATTACK4) &&
		(eState != FSM_ATTACK_CHARGED1) &&
		(eState != FSM_ATTACK_CHARGED2) &&
		(eState != FSM_ELEMENTAL_SKILL1) &&
		(eState != FSM_ELEMENTAL_SKILL2) &&
		(eState != FSM_ELEMENTAL_SKILL3) &&
		(eState != FSM_ELEMENTAL_SKILL1_IDLE) &&
		(eState != FSM_ELEMENTAL_SKILL2_IDLE) &&
		(eState != FSM_ELEMENTAL_SKILL3_IDLE) &&
		(eState != FSM_ELEMENTAL_BURST))
	{
		if (m_pGameInstance->Key_Down('E')&& 
			static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[m_ePlayerNum])->Get_CurSkillTime() == 0.f)
		{
			bInput = TRUE;
			static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_ELEMENTAL_SKILL1));
		}
	}

	if ((eState != FSM_ATTACK_READY) &&
		(eState != FSM_ATTACK_CHARGED_READY) &&
		(eState != FSM_ATTACK1) &&
		(eState != FSM_ATTACK2) &&
		(eState != FSM_ATTACK3) &&
		(eState != FSM_ATTACK4) &&
		(eState != FSM_ATTACK_CHARGED1) &&
		(eState != FSM_ATTACK_CHARGED2) &&
		(eState != FSM_ELEMENTAL_SKILL1) &&
		(eState != FSM_ELEMENTAL_SKILL2) &&
		(eState != FSM_ELEMENTAL_SKILL3) &&
		(eState != FSM_ELEMENTAL_SKILL1_IDLE) &&
		(eState != FSM_ELEMENTAL_SKILL2_IDLE) &&
		(eState != FSM_ELEMENTAL_SKILL3_IDLE) &&
		(eState != FSM_ELEMENTAL_BURST) &&
		(eState != FSM_ELEMENTAL_BURST_IDLE))
	{
		if (m_pGameInstance->Key_Down('R') /*&&
			static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[m_ePlayerNum])->Get_CurBurstTime() == 0.f &&
			static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[m_ePlayerNum])->Get_CurEnergy() 
			== static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[m_ePlayerNum])->Get_ElementalEnergy()*/)
		{
			bInput = TRUE;
			static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_State(FSM_ELEMENTAL_BURST));

			/* 스탯 */
			_float fMaxTime = static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[m_ePlayerNum])->Get_ElementalBurstTime();
			static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[m_ePlayerNum])->Set_CurBurstTime(fMaxTime);
			static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[m_ePlayerNum])->Set_CurEnergy(); /* 원소에너지 0 */

			switch (m_ePlayerNum)
			{
			case Client::PLAYER_ONE:
				break;
			case Client::PLAYER_TWO:
				m_pGameInstance->PlaySoundW(L"Zhongli_Burst.mp3", 1.f);
				break;
			default:
				break;
			}
		}
	}

	for (_uint i = 0; i < PLAYER_END; ++i)
	{
		if (static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[(PLAYERINDEX)i])->Get_CurBurstTime() == 0.f &&
			static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[(PLAYERINDEX)i])->Get_CurEnergy()
			== static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[(PLAYERINDEX)i])->Get_ElementalEnergy())
		{
			m_pPageMain->Set_BurstReady((PLAYERINDEX)i, TRUE);
		}
		else
			m_pPageMain->Set_BurstReady((PLAYERINDEX)i, FALSE);
	}

	/* 모든캐릭터 쿨타임 갱신 */
	for (size_t Index = 0; Index < PLAYER_END; Index++)
	{
		_float fCurSkillTime = static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[Index])->Get_CurSkillTime();
		_float fCurBurstTime = static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[Index])->Get_CurBurstTime();

		if (fCurSkillTime > 0.f)
		{
			fCurSkillTime -= fTimeDelta;
			static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[Index])->Set_CurSkillTime(fCurSkillTime);
		}
		if (fCurSkillTime <= 0.f)
			static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[Index])->Set_CurSkillTime(0.f);

		if(fCurBurstTime > 0.f)
		{
			fCurBurstTime -= fTimeDelta;
			static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[Index])->Set_CurBurstTime(fCurBurstTime);
		}
		if (fCurBurstTime <= 0.f)
			static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[Index])->Set_CurBurstTime(0.f);
	}

	return S_OK;
}

HRESULT CPlayer::Character_Dead(_float fTimeDelta)
{
	if (m_bCombatMode == FALSE) /* Player 1명만 온필드 */
	{
		if (m_Parts[PART_BODY].pPartySlot[m_ePlayerNum]->Get_Dead() == TRUE)
		{
			PLAYERINDEX eCurIndex = m_ePlayerNum;

			for (_uint i = 0; i < PLAYER_END; ++i)
			{
				if (m_Parts[PART_BODY].pPartySlot[i]->Get_Dead() == FALSE) // UI도 바뀌게 해야함
				{
					_vector vPlayerLook = PlayerInfo[eCurIndex].pTransform->Get_State(CTransform::STATE_LOOK);
					vPlayerLook.m128_f32[1] = 0.f; vPlayerLook = XMVector4Normalize(vPlayerLook);
					_float4 fPos;
					XMStoreFloat4(&fPos, PlayerInfo[eCurIndex].pTransform->Get_State(CTransform::STATE_POSITION));

					m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, m_Parts[PART_BODY].pPartySlot[eCurIndex]->Get_CollisionID(), FALSE);
					m_pGameInstance->Set_IsCollision(COLL_EVENTREACT_PLAYER, m_Parts[PART_BODY].pPartySlot[eCurIndex]->Get_CollisionID() + 1, FALSE);
					PlayerInfo[eCurIndex].bDead = TRUE;
					PlayerInfo[eCurIndex].pController->Set_Position({ 0.f,-100.f,0.f,1.f });
					m_ePlayerNum = (PLAYERINDEX)i;
					PlayerInfo[m_ePlayerNum].bAutoMode = FALSE;
					PlayerInfo[m_ePlayerNum].pTransform->Reset_Look_At_Direction();
					PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vPlayerLook, 100);
					PlayerInfo[m_ePlayerNum].pController->Set_Position(fPos);
					m_pGameInstance->Set_IsCollision(COLL_EVENTREACT_PLAYER, m_Parts[PART_BODY].pPartySlot[m_ePlayerNum]->Get_CollisionID() + 1, TRUE);
					PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_ForceState(FSM_IDLE);

					m_pPageMain->Set_MemberIndex(i);
					return S_OK;
				}
			}
		}
	}
	else
	{
		if (m_Parts[PART_BODY].pPartySlot[m_ePlayerNum]->Get_Dead())
		{
			PLAYERINDEX eCurIndex = (PLAYERINDEX)m_ePlayerNum;

			for (_uint i = 0; i < PLAYER_END; ++i)
			{
				if (m_Parts[PART_BODY].pPartySlot[i]->Get_Dead() == FALSE) // UI도 바뀌게 해야함
				{
					//_vector vPlayerLook = PlayerInfo[eCurIndex].pTransform->Get_State(CTransform::STATE_LOOK);
					//vPlayerLook.m128_f32[1] = 0.f; vPlayerLook = XMVector4Normalize(vPlayerLook);
					//_float4 fPos;
					//XMStoreFloat4(&fPos, PlayerInfo[eCurIndex].pTransform->Get_State(CTransform::STATE_POSITION));

					m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, m_Parts[PART_BODY].pPartySlot[eCurIndex]->Get_CollisionID(), FALSE);
					m_pGameInstance->Set_IsCollision(COLL_EVENTREACT_PLAYER, m_Parts[PART_BODY].pPartySlot[eCurIndex]->Get_CollisionID() + 1, FALSE);
					PlayerInfo[eCurIndex].bDead = TRUE;
					PlayerInfo[eCurIndex].pController->Set_Position({ 0.f,-100.f,0.f,1.f });
					m_ePlayerNum = (PLAYERINDEX)i;
					PlayerInfo[m_ePlayerNum].bAutoMode = FALSE;
					//PlayerInfo[m_ePlayerNum].pTransform->Reset_Look_At_Direction();
					//PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vPlayerLook, 100);
					//PlayerInfo[m_ePlayerNum].pController->Set_Position(fPos);
					m_pGameInstance->Set_IsCollision(COLL_EVENTREACT_PLAYER, m_Parts[PART_BODY].pPartySlot[m_ePlayerNum]->Get_CollisionID() + 1, TRUE);
					//PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Change_ForceState(FSM_IDLE);

					m_pPageMain->Set_MemberIndex(i);
					return S_OK;
				}
			}
		}
	}
	return S_OK;
}

HRESULT CPlayer::Hit_Collision_State(_float fTimeDelta)
{
	FSM_STATE eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Get_CurrentState());
	_int iWeaponID = -1;
	vector<CGameObject*> vecHitMonsters;

	if (eState >= FSM_ATTACK_READY && eState <= FSM_ATTACK_CHARGED2) /* 일반, 차지 공격 */
	{
		if (eState != FSM_ATTACK5)
		{
			_vector vDirection = { };
			_float4 Dir;
			_vector vCamLook = m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_VIEW).r[2];
			vCamLook.m128_f32[1] = 0.f; vCamLook = XMVector4Normalize(vCamLook);

			_vector vCamRight = m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_VIEW).r[0];
			vCamRight.m128_f32[1] = 0.f; vCamRight = XMVector4Normalize(vCamRight);
			if (m_pGameInstance->Key_Pressing('W'))
			{
				vDirection += vCamLook;
				PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamLook);
				XMStoreFloat4(&Dir, vCamLook);
				PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta * 20, Dir, 0.0f);
			}
			if (m_pGameInstance->Key_Pressing('S'))
			{
				vDirection -= vCamLook;
				PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamLook * -1.f);
				XMStoreFloat4(&Dir, vCamLook * -1.f);
				PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta * 20, Dir, 0.0f);
			}
			if (m_pGameInstance->Key_Pressing('A'))
			{
				vDirection -= vCamRight;
				PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamRight * -1.f);
				XMStoreFloat4(&Dir, vCamRight * -1.f);
				PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta * 20, Dir, 0.0f);
			}
			if (m_pGameInstance->Key_Pressing('D'))
			{
				vDirection += vCamRight;
				PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vCamRight);
				XMStoreFloat4(&Dir, vCamRight);
				PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta * 20, Dir, 0.0f);
			}
		}

		if (CPlayer::isFunctionTrue() == TRUE) // Function Event
		{
	
			m_MonsterHitCollider[m_ePlayerNum].clear();
			m_bHit[m_ePlayerNum] = TRUE;

			if (m_ePlayerNum == PLAYER_ONE)
			{
				if (eState == FSM_ATTACK_CHARGED1)
				{
					if (m_iExtraAttackCount == 3)
						m_iExtraAttackCount = 0;

					++m_iExtraAttackCount;
					if (m_iExtraAttackCount > 3)
						m_iExtraAttackCount = 1;
				}
				else if (eState == FSM_ATTACK_CHARGED2)
					m_iExtraAttackCount = 4;
				else if (eState == FSM_ATTACK_CHARGED_READY)
				{
					if(m_iExtraAttackCount == 2)
						m_iExtraAttackCount = 3;
				}
			}
			CPlayer::Create_Trail(eState);
		}

		if (m_ePlayerNum == PLAYER_TWO && eState == FSM_ATTACK_CHARGED2) // 종려 차지 어택 마무리 때 충돌 예외처리, 안하면 창 돌릴때 충돌됨
			;
		else if (m_bHit[m_ePlayerNum] == TRUE)
		{
			if (m_ePlayerNum == PLAYER_THREE) // 바바라
			{
				if(m_ePlayerNum == PLAYER_THREE)
				m_bHit[m_ePlayerNum] = FALSE;
				CBabaraBasicAttack::BARBARA_ATK_DESC tDesc = {};
				_vector vTarget = XMLoadFloat4(&m_vTargetMonsterPos);
				_vector vDir = XMVector3Normalize(PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_POSITION) - vTarget);
				tDesc.tSkillDesc.vSpawnPos = vTarget + XMVectorSet(0.f, 0.5f, 0.f, 0.f) + vDir * 0.1f;
				tDesc.tSkillDesc.vDir = vDir;

				if (eState == FSM_ATTACK_CHARGED2)
				{
					tDesc.tSkillDesc.vScale = XMVectorSet(2.4f, 0.f, 0.f, 0.f);
					tDesc.tSkillDesc.fDamage = static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[PLAYER_THREE])->Get_Attack(CPart_Status::ATK_CHARGED);
				}
				else
				{
					tDesc.tSkillDesc.vScale = XMVectorSet(1.f, 0.f, 0.f, 0.f);
					tDesc.tSkillDesc.fDamage = static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[PLAYER_THREE])->Get_Attack(CPart_Status::ATK_NORMAL);
				}
				
				if (eState >= FSM_ATTACK1 && eState <= FSM_ATTACK3)
					tDesc.eBarbaraAtkType = BARBARA_ATK_BASE;
				else if (eState == FSM_ATTACK4)
					tDesc.eBarbaraAtkType = BARBARA_ATK_BASE_F;
				else
					tDesc.eBarbaraAtkType = BARBARA_ATK_FORCE;

				if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Skill_BabaraBasicAttack", L"Layer_Skill", &tDesc)))
					return E_FAIL;
			}
			else // 다이루크, 종려, 라이덴
			{
				iWeaponID = m_Parts[PART_WEAPON].pPartySlot[m_ePlayerNum]->Get_CollisionID();
				vecHitMonsters = m_pGameInstance->Find_Collision_Objects(COLL_PLAYER_WEAPON, iWeaponID, COLL_MONSTER_BODY);

				for (auto& HitMonsters : vecHitMonsters)
				{
					_int iHitID = static_cast<CPartObject*>(HitMonsters)->Get_CollisionID();
					if (CPlayer::isHitCollider(m_MonsterHitCollider[m_ePlayerNum], iHitID) == TRUE)
						continue;
					m_MonsterHitCollider[m_ePlayerNum].push_back(iHitID); // 맞은 몬스터


					_float fDamage;
					if (eState >= FSM_ATTACK_CHARGED1) /* 차지 공격*/
						fDamage = static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[m_ePlayerNum])->Get_Attack(CPart_Status::ATK_TYPE::ATK_CHARGED);
					else /* 일반 공격 */
						fDamage = static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[m_ePlayerNum])->Get_Attack(CPart_Status::ATK_TYPE::ATK_NORMAL);

					if(m_ePlayerNum == PLAYER_FOUR && m_bRaidenUltOn == TRUE)
						static_cast<CPartObject*>(HitMonsters)->Set_Damage(fDamage*1.5f, ELEMENT_ELECTRIC); // fDamage
					else
						static_cast<CPartObject*>(HitMonsters)->Set_Damage(fDamage, (ELEMENT)m_iElement); // fDamage

					if (m_ePlayerNum == PLAYER_ONE || m_ePlayerNum == PLAYER_TWO)
					{
						CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
						if (pCamera != nullptr)
						{
							
							pCamera->Set_ShakingMode(0.1f, 5.f, 1);
						}
					}

					//_vector vSelfPos = PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_POSITION);
					//_vector vSelfLook = PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_LOOK);
					//CEffect::EFFECT_DESC descc = {};
					//descc.eEffectID = EFFECT_HITTING_STAR;
					//descc.fLifeTime = 5.f;
					//vSelfPos.m128_f32[1] += 1.f;
					//vSelfPos += vSelfLook * 2.f;
					//descc.vSpawnPos = vSelfPos;
					//descc.vScale = { 5.f, 5.f, 1.f, 0.f };

					//if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Hitting", L"Layer_Effect", &descc)))
					//	return E_FAIL;
				}
			}
		}
	}
	else if (eState >= FSM_ELEMENTAL_SKILL1 && eState <= FSM_ELEMENTAL_SKILL3) /* E 스킬 */
	{
		switch (m_ePlayerNum)
		{
		case Client::PLAYER_ONE: /* 다이루크 */
			if (CPlayer::isFunctionTrue() == TRUE) // Function Event
			{
				m_MonsterHitCollider[m_ePlayerNum].clear();
				m_bHit[m_ePlayerNum] = TRUE;

				CPlayer::Create_Trail(eState);
			}

			if (m_bHit[m_ePlayerNum] == TRUE)
			{
				iWeaponID = m_Parts[PART_WEAPON].pPartySlot[m_ePlayerNum]->Get_CollisionID();
				vecHitMonsters = m_pGameInstance->Find_Collision_Objects(COLL_PLAYER_WEAPON, iWeaponID, COLL_MONSTER_BODY);

				for (auto& HitMonsters : vecHitMonsters)
				{
					_int iHitID = static_cast<CPartObject*>(HitMonsters)->Get_CollisionID();
					if (CPlayer::isHitCollider(m_MonsterHitCollider[m_ePlayerNum], iHitID) == TRUE)
						continue;
					m_MonsterHitCollider[m_ePlayerNum].push_back(iHitID); // 맞은 몬스터

					static_cast<CPartObject*>(HitMonsters)->
						Set_Damage(static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[m_ePlayerNum])->Get_Attack(CPart_Status::ATK_TYPE::ATK_BURST), ELEMENT_FIRE);

					CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
					if (pCamera != nullptr)
					{
						pCamera->Set_ShakingMode(0.1f, 5.f, 1);
					}

					_vector vSelfPos = PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_POSITION);
					_vector vSelfLook = PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_LOOK);
					CEffect::EFFECT_DESC descc = {};
					descc.eEffectID = EFFECT_HITTING_STAR;
					descc.fLifeTime = 5.f;
					vSelfPos.m128_f32[1] += 1.f;
					vSelfPos += vSelfLook * 2.f;
					descc.vSpawnPos = vSelfPos;
					descc.vScale = { 5.f, 5.f, 1.f, 0.f };

					if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Hitting", L"Layer_Effect", &descc)))
						return E_FAIL;
				}
			}
			break;

		case Client::PLAYER_TWO: /* 종려 */
			if (CPlayer::isFunctionTrue() == TRUE) // Function Event
			{
				m_MonsterHitCollider[m_ePlayerNum].clear();
				m_bHit[m_ePlayerNum] = TRUE;
			}

			break;

		case Client::PLAYER_THREE:
			break;

		case Client::PLAYER_FOUR: /* 라이덴 */
		{
			if (CPlayer::isFunctionTrue() == TRUE) // Function Event
			{
				m_MonsterHitCollider[m_ePlayerNum].clear();
				m_bHit[m_ePlayerNum] = TRUE;

				CRaiden_ElementAttack::RAIDEN_SKILL_DESC tDesc = {};
				tDesc.eRaidenSkillType = RAIDEN_ELEMENTAL_SKILL;

				_vector vPos = PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_POSITION);
				_vector vLook = PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_LOOK);
				tDesc.tSkillDesc.vSpawnPos = vPos + vLook;
				tDesc.tSkillDesc.vScale = XMVectorSet(3.f, 0.f, 0.f, 0.f);

				if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Skill_RaidenElementAttack", L"Layer_Skill", &tDesc)))
					return E_FAIL;
			}
			break;
		}

		default:
			break;
		}

	}
	else if (eState == FSM_ELEMENTAL_BURST)
	{
		if (CPlayer::isFunctionTrue() == TRUE) // Function Event
		{
			m_MonsterHitCollider[m_ePlayerNum].clear();
			m_bHit[m_ePlayerNum] = TRUE;

			if (m_ePlayerNum == PLAYER_THREE) /* 바바라 */
			{
				if (m_iElementalBurstCount == 0)
				{
					m_pGameInstance->PlaySoundW(L"Barbara_ElementalBurst.mp3");
					++m_iElementalBurstCount;
					CEffect::EFFECT_DESC descc = {};
					descc.eEffectID = EFFECT_ETC;
					descc.fLifeTime = 1.f;
					_vector vSelfPos = PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_POSITION);
					_vector vSelfLook = PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_LOOK);
					vSelfPos.m128_f32[1] += 0.1f;
					vSelfPos += vSelfLook * 1.f;
					descc.vSpawnPos = vSelfPos;
					descc.vScale = { 1.f, 1.f, 1.f, 0.f };
					if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Barbara_Burst_Circle", L"Layer_Effect", &descc)))
						return E_FAIL;
				}
				else if (m_iElementalBurstCount == 1) // 손가락
				{
					++m_iElementalBurstCount;
				}
				else if (m_iElementalBurstCount == 2) // 점프
				{
					CPlayer::Set_AllRatioHeal(0.3f, (_float)m_pGameInstance->Get_Float_Random(2682, 3727));
					m_iElementalBurstCount = 0;
				}
			}
			else if (m_ePlayerNum == PLAYER_FOUR) /* 라이덴 */
			{
				m_MonsterHitCollider[m_ePlayerNum].clear();
				m_bHit[m_ePlayerNum] = TRUE;

				CRaiden_ElementAttack::RAIDEN_SKILL_DESC tDesc = {};
				tDesc.eRaidenSkillType = RAIDEN_ELEMENTAL_BURST;

				_vector vPos = PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_POSITION);
				tDesc.tSkillDesc.vSpawnPos = vPos;
				tDesc.tSkillDesc.vScale = XMVectorSet(5.f, 0.f, 0.f, 0.f);

				_vector vUp = PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_UP);
				vPos += vUp;

				for (int i = 0; i < 20; i++)
				{
					CShougun_Trail_TEST::SHOGUN_TRAIL_DESC Desc;
					Desc.vTrailPos = vPos;
					Desc.iTrailIndex = m_pGameInstance->Get_Int_Random(0,9);
					Desc.pParentTransform = PlayerInfo[m_ePlayerNum].pTransform;
					if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, L"Prototype_GameObject_Effect_Shougun_Trail", L"Layer_Effect", &Desc)))
						return E_FAIL;
				}

				if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Skill_RaidenElementAttack", L"Layer_Skill", &tDesc)))
					return E_FAIL;

				m_bRaidenUltOn = true;
				m_fRaidenUltTime = 60.f;

			}
		}
	}
	else if (eState >= FSM_FALL_ATTACK_START1 && eState <= FSM_FALL_ATTACK_END2) /* 공중 찍기 */
	{
		if (CPlayer::isFunctionTrue() == TRUE) // Function Event
		{
			m_MonsterHitCollider[m_ePlayerNum].clear();
			m_bHit[m_ePlayerNum] = TRUE;
		}

		if (m_bHit[m_ePlayerNum] == TRUE)
		{
			iWeaponID = m_Parts[PART_WEAPON].pPartySlot[m_ePlayerNum]->Get_CollisionID();
			vecHitMonsters = m_pGameInstance->Find_Collision_Objects(COLL_PLAYER_WEAPON, iWeaponID, COLL_MONSTER_BODY);

			for (auto& HitMonsters : vecHitMonsters)
			{
				_int iHitID = static_cast<CPartObject*>(HitMonsters)->Get_CollisionID();
				if (CPlayer::isHitCollider(m_MonsterHitCollider[m_ePlayerNum], iHitID) == TRUE)
					continue;
				m_MonsterHitCollider[m_ePlayerNum].push_back(iHitID); // 맞은 몬스터

				_float fDamage;
				fDamage = static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[m_ePlayerNum])->Get_Attack(CPart_Status::ATK_TYPE::ATK_CHARGED);

				static_cast<CPartObject*>(HitMonsters)->Set_Damage(fDamage, (ELEMENT)m_iElement); // fDamage
				m_bHit[m_ePlayerNum] = FALSE;

				CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
				if (pCamera != nullptr)
				{
					pCamera->Set_ShakingMode(0.1f, 5.f, 1);
				}

				_vector vSelfPos = PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_POSITION);
				_vector vSelfLook = PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_LOOK);
				CEffect::EFFECT_DESC descc = {};
				descc.eEffectID = EFFECT_HITTING_STAR;
				descc.fLifeTime = 5.f;
				vSelfPos.m128_f32[1] += 1.f;
				vSelfPos += vSelfLook * 2.f;
				descc.vSpawnPos = vSelfPos;
				descc.vScale = { 5.f, 5.f, 1.f, 0.f };

				if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Hitting", L"Layer_Effect", &descc)))
					return E_FAIL;
			}
		}
	}
	else
		m_bHit[m_ePlayerNum] = FALSE;

	if (m_pGameInstance->Key_Pressing(VK_LSHIFT) && m_pGameInstance->Key_Down(VK_HOME)) /* 테스트용, 상하로 빠르게 이동*/
	{
		_vector vPos = {-97.f, 0.f, 3.9f, 1.f};		// 드발린 시작 위치

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	}
	else if (m_pGameInstance->Key_Down(VK_HOME))
	{
		_float4 vPosition;
		XMStoreFloat4(&vPosition, PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_POSITION));

		if (m_pGameInstance->Key_Pressing(VK_CONTROL))
			vPosition.y -= 10.0f;
		else
			vPosition.y += 10.0f;

		PlayerInfo[m_ePlayerNum].pController->Set_Position(vPosition);
	}

	if (m_pGameInstance->Key_Down(VK_PRIOR)) /* Page Up Key */
	{
		m_fSpeedCheat += 1.f;
		cout << m_fSpeedCheat + 1.f << "x Speed" << endl;
	}

	if (m_pGameInstance->Key_Pressing(VK_NEXT)) /* Page Down Key */
	{
		if (m_fSpeedCheat > 0.f)
		{
			m_fSpeedCheat -= 1.f;
			cout << m_fSpeedCheat + 1.f << "x Speed" << endl;
		}
	}

	return S_OK;
}

HRESULT CPlayer::Hit_Collision_State(_float fTimeDelta, PLAYERINDEX eIndex)
{
	FSM_STATE eState = static_cast<FSM_STATE>(PlayerInfo[eIndex].pFiniteStateMachineCom->Get_CurrentState());
	_int iWeaponID = -1;
	vector<CGameObject*> vecHitMonsters;

	if (eState >= FSM_ATTACK_READY && eState <= FSM_ATTACK_CHARGED2) /* 일반, 차지 공격 */
	{

		if (eState != FSM_ATTACK5)
		{
			_vector vDirection = { };
		_float4 Dir;
		_vector vCamLook = m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_VIEW).r[2];
		vCamLook.m128_f32[1] = 0.f; vCamLook = XMVector4Normalize(vCamLook);

		_vector vCamRight = m_pGameInstance->Get_Transform_Inverse(CPipeLine::D3DTS_VIEW).r[0];
		vCamRight.m128_f32[1] = 0.f; vCamRight = XMVector4Normalize(vCamRight);
		if (m_pGameInstance->Key_Pressing('W'))
		{
			vDirection += vCamLook;
			PlayerInfo[eIndex].pTransform->Set_Look_At_Direction(vCamLook);
			XMStoreFloat4(&Dir, vCamLook);
			PlayerInfo[eIndex].pController->Movement(fTimeDelta, Dir, 0.0f);
		}
		if (m_pGameInstance->Key_Pressing('S'))
		{
			vDirection -= vCamLook;
			PlayerInfo[eIndex].pTransform->Set_Look_At_Direction(vCamLook * -1.f);
			XMStoreFloat4(&Dir, vCamLook * -1.f);
			PlayerInfo[eIndex].pController->Movement(fTimeDelta, Dir, 0.0f);
		}
		if (m_pGameInstance->Key_Pressing('A'))
		{
			vDirection -= vCamRight;
			PlayerInfo[eIndex].pTransform->Set_Look_At_Direction(vCamRight * -1.f);
			XMStoreFloat4(&Dir, vCamRight * -1.f);
			PlayerInfo[eIndex].pController->Movement(fTimeDelta, Dir, 0.0f);
		}
		if (m_pGameInstance->Key_Pressing('D'))
		{
			vDirection += vCamRight;
			PlayerInfo[eIndex].pTransform->Set_Look_At_Direction(vCamRight);
			XMStoreFloat4(&Dir, vCamRight);
			PlayerInfo[eIndex].pController->Movement(fTimeDelta, Dir, 0.0f);
		}

		if (CPlayer::isFunctionTrue(eIndex) == TRUE) // Function Event
		{
			m_MonsterHitCollider[eIndex].clear();
			m_bHit[eIndex] = TRUE;

			if (eIndex == PLAYER_ONE)
			{
				if (eState == FSM_ATTACK_CHARGED1)
				{
					if (m_iExtraAttackCount == 3)
						m_iExtraAttackCount = 0;

					++m_iExtraAttackCount;
					if (m_iExtraAttackCount > 3)
						m_iExtraAttackCount = 1;
				}
				else if (eState == FSM_ATTACK_CHARGED2)
					m_iExtraAttackCount = 4;
				else if (eState == FSM_ATTACK_CHARGED_READY)
				{
					if (m_iExtraAttackCount == 2)
						m_iExtraAttackCount = 3;
				}
			}
			CPlayer::Create_Trail(eState, eIndex);
		}
		}

		if (eIndex == PLAYER_TWO && eState == FSM_ATTACK_CHARGED2) // 종려 차지 어택 마무리 때 충돌 예외처리, 안하면 창 돌릴때 충돌됨
			;
		else if (m_bHit[eIndex] == TRUE)
		{
			if (eIndex == PLAYER_THREE) // 바바라
			{
				m_bHit[eIndex] = FALSE;
				CBabaraBasicAttack::BARBARA_ATK_DESC tDesc = {};
				_vector vTarget = XMLoadFloat4(&m_vTargetMonsterPos);
				_vector vDir = XMVector3Normalize(PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_POSITION) - vTarget);
				tDesc.tSkillDesc.vSpawnPos = vTarget + XMVectorSet(0.f, 0.5f, 0.f, 0.f) + vDir * 0.1f;
				tDesc.tSkillDesc.vDir = vDir;

				if (eState == FSM_ATTACK_CHARGED2)
				{
					tDesc.tSkillDesc.vScale = XMVectorSet(2.4f, 0.f, 0.f, 0.f);
					tDesc.tSkillDesc.fDamage = static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[PLAYER_THREE])->Get_Attack(CPart_Status::ATK_CHARGED);
				}
				else
				{
					tDesc.tSkillDesc.vScale = XMVectorSet(1.f, 0.f, 0.f, 0.f);
					tDesc.tSkillDesc.fDamage = static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[PLAYER_THREE])->Get_Attack(CPart_Status::ATK_NORMAL);
				}
				
				if (eState >= FSM_ATTACK1 && eState <= FSM_ATTACK3)
					tDesc.eBarbaraAtkType = BARBARA_ATK_BASE;
				else if (eState == FSM_ATTACK4)
					tDesc.eBarbaraAtkType = BARBARA_ATK_BASE_F;
				else
					tDesc.eBarbaraAtkType = BARBARA_ATK_FORCE;

				if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Skill_BabaraBasicAttack", L"Layer_Skill", &tDesc)))
					return E_FAIL;
			}
			else // 다이루크, 종려, 라이덴
			{
				iWeaponID = m_Parts[PART_WEAPON].pPartySlot[eIndex]->Get_CollisionID();
				vecHitMonsters = m_pGameInstance->Find_Collision_Objects(COLL_PLAYER_WEAPON, iWeaponID, COLL_MONSTER_BODY);

				for (auto& HitMonsters : vecHitMonsters)
				{
					_int iHitID = static_cast<CPartObject*>(HitMonsters)->Get_CollisionID();
					if (CPlayer::isHitCollider(m_MonsterHitCollider[eIndex], iHitID) == TRUE)
						continue;
					m_MonsterHitCollider[eIndex].push_back(iHitID); // 맞은 몬스터


					_float fDamage;
					if (eState >= FSM_ATTACK_CHARGED1) /* 차지 공격*/
						fDamage = static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[eIndex])->Get_Attack(CPart_Status::ATK_TYPE::ATK_CHARGED);
					else /* 일반 공격 */
						fDamage = static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[eIndex])->Get_Attack(CPart_Status::ATK_TYPE::ATK_NORMAL);

					if (eIndex == PLAYER_FOUR && m_bRaidenUltOn == TRUE)
						static_cast<CPartObject*>(HitMonsters)->Set_Damage(fDamage * 1.5f, ELEMENT_ELECTRIC); // fDamage
					else
						static_cast<CPartObject*>(HitMonsters)->Set_Damage(fDamage, (ELEMENT)m_iElement); // fDamage

					if (eIndex == PLAYER_ONE || eIndex == PLAYER_TWO)
					{
						CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
						if (pCamera != nullptr && m_ePlayerNum != m_ePlayerNum)
						{
							pCamera->Set_ShakingMode(0.1f, 5.f, 1);
						}
					}

					_vector vSelfPos = PlayerInfo[eIndex].pTransform->Get_State(CTransform::STATE_POSITION);
					_vector vSelfLook = PlayerInfo[eIndex].pTransform->Get_State(CTransform::STATE_LOOK);
					CEffect::EFFECT_DESC descc = {};
					descc.eEffectID = EFFECT_HITTING_STAR;
					descc.fLifeTime = 5.f;
					vSelfPos.m128_f32[1] += 1.f;
					vSelfPos += vSelfLook * 2.f;
					descc.vSpawnPos = vSelfPos;
					descc.vScale = { 5.f, 5.f, 1.f, 0.f };

					if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Hitting", L"Layer_Effect", &descc)))
						return E_FAIL;
				}
			}
		}
	}
	else if (eState >= FSM_ELEMENTAL_SKILL1 && eState <= FSM_ELEMENTAL_SKILL3) /* E 스킬 */
	{
		switch (eIndex)
		{
		case Client::PLAYER_ONE: /* 다이루크 */
			if (CPlayer::isFunctionTrue(eIndex) == TRUE) // Function Event
			{
				m_MonsterHitCollider[eIndex].clear();
				m_bHit[eIndex] = TRUE;

				CPlayer::Create_Trail(eState, eIndex);
			}

			if (m_bHit[eIndex] == TRUE)
			{
				iWeaponID = m_Parts[PART_WEAPON].pPartySlot[eIndex]->Get_CollisionID();
				vecHitMonsters = m_pGameInstance->Find_Collision_Objects(COLL_PLAYER_WEAPON, iWeaponID, COLL_MONSTER_BODY);

				for (auto& HitMonsters : vecHitMonsters)
				{
					_int iHitID = static_cast<CPartObject*>(HitMonsters)->Get_CollisionID();
					if (CPlayer::isHitCollider(m_MonsterHitCollider[eIndex], iHitID) == TRUE)
						continue;
					m_MonsterHitCollider[eIndex].push_back(iHitID); // 맞은 몬스터

					static_cast<CPartObject*>(HitMonsters)->
						Set_Damage(static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[eIndex])->Get_Attack(CPart_Status::ATK_TYPE::ATK_BURST), ELEMENT_FIRE);

					CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
					if (pCamera != nullptr && m_ePlayerNum != m_ePlayerNum)
					{
						pCamera->Set_ShakingMode(0.1f, 5.f, 1);
					}

					_vector vSelfPos = PlayerInfo[eIndex].pTransform->Get_State(CTransform::STATE_POSITION);
					_vector vSelfLook = PlayerInfo[eIndex].pTransform->Get_State(CTransform::STATE_LOOK);
					CEffect::EFFECT_DESC descc = {};
					descc.eEffectID = EFFECT_HITTING_STAR;
					descc.fLifeTime = 5.f;
					vSelfPos.m128_f32[1] += 1.f;
					vSelfPos += vSelfLook * 2.f;
					descc.vSpawnPos = vSelfPos;
					descc.vScale = { 5.f, 5.f, 1.f, 0.f };

					if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Hitting", L"Layer_Effect", &descc)))
						return E_FAIL;
				}
			}
			break;

		case Client::PLAYER_TWO: /* 종려 */
			if (CPlayer::isFunctionTrue(eIndex) == TRUE) // Function Event
			{
				m_MonsterHitCollider[eIndex].clear();
				m_bHit[eIndex] = TRUE;
			}

			break;

		case Client::PLAYER_THREE:
			break;

		case Client::PLAYER_FOUR: /* 라이덴 */
		{
			if (CPlayer::isFunctionTrue(eIndex) == TRUE) // Function Event
			{
				m_MonsterHitCollider[eIndex].clear();
				m_bHit[eIndex] = TRUE;

				CRaiden_ElementAttack::RAIDEN_SKILL_DESC tDesc = {};
				tDesc.eRaidenSkillType = RAIDEN_ELEMENTAL_SKILL;

				_vector vPos = PlayerInfo[eIndex].pTransform->Get_State(CTransform::STATE_POSITION);
				_vector vLook = PlayerInfo[eIndex].pTransform->Get_State(CTransform::STATE_LOOK);
				tDesc.tSkillDesc.vSpawnPos = vPos + vLook;
				tDesc.tSkillDesc.vScale = XMVectorSet(3.f, 0.f, 0.f, 0.f);

				if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Skill_RaidenElementAttack", L"Layer_Skill", &tDesc)))
					return E_FAIL;
			}
			break;
		}

		default:
			break;
		}

	}
	else if (eState == FSM_ELEMENTAL_BURST)
	{
		if (CPlayer::isFunctionTrue(eIndex) == TRUE)
		{
			m_MonsterHitCollider[eIndex].clear();
			m_bHit[eIndex] = TRUE;

			if (eIndex == PLAYER_THREE) /* 바바라 */
			{
				if (m_iElementalBurstCount == 0)
				{
					++m_iElementalBurstCount;
					CEffect::EFFECT_DESC descc = {};
					descc.eEffectID = EFFECT_ETC;
					descc.fLifeTime = 1.f;
					_vector vSelfPos = PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_POSITION);
					_vector vSelfLook = PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_LOOK);
					vSelfPos.m128_f32[1] += 0.1f;
					vSelfPos += vSelfLook * 1.f;
					descc.vSpawnPos = vSelfPos;
					descc.vScale = { 1.f, 1.f, 1.f, 0.f };
					if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Barbara_Burst_Circle", L"Layer_Effect", &descc)))
						return E_FAIL;
				}
				else if (m_iElementalBurstCount == 1) // 손가락
				{
					++m_iElementalBurstCount;
				}
				else if (m_iElementalBurstCount == 2) // 점프
				{
					CPlayer::Set_AllRatioHeal(0.3f, (_float)m_pGameInstance->Get_Float_Random(2682, 3727));
					m_iElementalBurstCount = 0;
				}
			}
			else if (eIndex == PLAYER_FOUR) /* 라이덴 */
			{
				m_MonsterHitCollider[eIndex].clear();
				m_bHit[eIndex] = TRUE;

				CRaiden_ElementAttack::RAIDEN_SKILL_DESC tDesc = {};
				tDesc.eRaidenSkillType = RAIDEN_ELEMENTAL_BURST;

				_vector vPos = PlayerInfo[eIndex].pTransform->Get_State(CTransform::STATE_POSITION);
				tDesc.tSkillDesc.vSpawnPos = vPos;
				tDesc.tSkillDesc.vScale = XMVectorSet(5.f, 0.f, 0.f, 0.f);

				if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Skill_RaidenElementAttack", L"Layer_Skill", &tDesc)))
					return E_FAIL;
			}
		}

	}
	else if (eState >= FSM_FALL_ATTACK_START1 && eState <= FSM_FALL_ATTACK_END2) /* 공중 찍기 */
	{
		if (CPlayer::isFunctionTrue(eIndex) == TRUE) // Function Event
		{
			m_MonsterHitCollider[eIndex].clear();
			m_bHit[eIndex] = TRUE;
		}

		if (m_bHit[eIndex] == TRUE)
		{
			iWeaponID = m_Parts[PART_WEAPON].pPartySlot[eIndex]->Get_CollisionID();
			vecHitMonsters = m_pGameInstance->Find_Collision_Objects(COLL_PLAYER_WEAPON, iWeaponID, COLL_MONSTER_BODY);

			for (auto& HitMonsters : vecHitMonsters)
			{
				_int iHitID = static_cast<CPartObject*>(HitMonsters)->Get_CollisionID();
				if (CPlayer::isHitCollider(m_MonsterHitCollider[eIndex], iHitID) == TRUE)
					continue;
				m_MonsterHitCollider[eIndex].push_back(iHitID); // 맞은 몬스터

				_float fDamage;
				fDamage = static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[eIndex])->Get_Attack(CPart_Status::ATK_TYPE::ATK_CHARGED);

				static_cast<CPartObject*>(HitMonsters)->Set_Damage(fDamage, (ELEMENT)m_iElement); // fDamage
				m_bHit[eIndex] = FALSE;

				CCamera_Main* pCamera = static_cast<CCamera_Main*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"));
				if (pCamera != nullptr && m_ePlayerNum != m_ePlayerNum)
				{
					pCamera->Set_ShakingMode(0.1f, 5.f, 1);
				}

				_vector vSelfPos = PlayerInfo[eIndex].pTransform->Get_State(CTransform::STATE_POSITION);
				_vector vSelfLook = PlayerInfo[eIndex].pTransform->Get_State(CTransform::STATE_LOOK);
				CEffect::EFFECT_DESC descc = {};
				descc.eEffectID = EFFECT_HITTING_STAR;
				descc.fLifeTime = 5.f;
				vSelfPos.m128_f32[1] += 1.f;
				vSelfPos += vSelfLook * 2.f;
				descc.vSpawnPos = vSelfPos;
				descc.vScale = { 5.f, 5.f, 1.f, 0.f };

				if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Hitting", L"Layer_Effect", &descc)))
					return E_FAIL;
			}
		}
	}
	else
		m_bHit[eIndex] = FALSE;

	return S_OK;
}

HRESULT CPlayer::MoveSpeed_Control(_float fTimeDelta, PLAYERINDEX eIndex)
{
	if (eIndex > PLAYERINDEX::PLAYER_END)
		return E_FAIL;

	if (m_fMoveSpeedDuration[eIndex] == -1.f)
	{
		PlayerInfo[m_ePlayerNum].pController->Set_MovementSpeed(m_fMoveSpeed[eIndex]);
	}
	else if (m_fMoveSpeedDuration[eIndex] > 0.f) /* 이동 속도 강제 제어 */
	{
		m_fMoveSpeedDuration[eIndex] -= fTimeDelta;
		PlayerInfo[m_ePlayerNum].pController->Set_MovementSpeed(m_fMoveSpeed[eIndex]);
	}
	else
		PlayerInfo[m_ePlayerNum].pController->Set_MovementSpeed(1.f);

	if (m_fSpeedCheat != 0.f)
		PlayerInfo[m_ePlayerNum].pController->Set_MovementSpeed(m_fSpeedCheat + 1.f);


	return S_OK;
}

void CPlayer::ElementalReaction(_float fDamage, ELEMENTAL_REACTION eReact, PLAYERINDEX eIndex)
{
	if (eIndex == PLAYER_END)
		eIndex = m_ePlayerNum;

	CSkillObj::SKILLOBJ_DESC tDesc = {};
	if (eReact != REACT_END)
	{
		tDesc.eCollider = COLL_MONSTER_SKILL;
		tDesc.vSpawnPos = PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.5f, 0.f, 0.f);
	}

	_float		fAdditionalDamage = 0.f;
	ELEMENT		eReactionElement = ELEMENT_END;

	switch (eReact)
	{
	case Client::REACT_VAPORIZE:			// 증발
		fAdditionalDamage = fDamage * 2.f;
		eReactionElement = ELEMENT_WATER;
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_FIRE, CElement_OHD::OVERHEAD_WATER);
		break;
	case Client::REACT_VAPORIZE_REVERSE:
		fAdditionalDamage = fDamage * 1.5f;
		eReactionElement = ELEMENT_FIRE;
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_FIRE, CElement_OHD::OVERHEAD_WATER);
		break;
	case Client::REACT_MELT:				// 융해
		fAdditionalDamage = fDamage * 2.f;
		eReactionElement = ELEMENT_FIRE;
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_FIRE, CElement_OHD::OVERHEAD_ICE);
		break;
	case Client::REACT_MELT_REVERSE:
		fAdditionalDamage = fDamage * 1.5f;
		eReactionElement = ELEMENT_FROST;
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_FIRE, CElement_OHD::OVERHEAD_ICE);
		break;
	case Client::REACT_OVERLOADED:			// 과부하
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_FIRE, CElement_OHD::OVERHEAD_LIGHTNING);
		break;
	case Client::REACT_SWIRL_FIRE:			// 확산
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_WIND, CElement_OHD::OVERHEAD_FIRE);
		break;
	case Client::REACT_SWIRL_WATER:
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_WIND, CElement_OHD::OVERHEAD_WATER);
		break;
	case Client::REACT_SWIRL_ELECTRIC:
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_WIND, CElement_OHD::OVERHEAD_LIGHTNING);
		break;
	case Client::REACT_SWIRL_FROST:
		tDesc.eReaction = REACT_SWIRL_FROST;
		tDesc.eElement = ELEMENT_FROST;
		tDesc.fDamage = fDamage;
		tDesc.vColor = XMVectorSet(0.f, 0.f, 1.f, 0.f);
		tDesc.vScale = XMVectorSet(5.f, 0.f, 0.f, 0.f);
		if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Skill_ElementalReaction", L"Layer_Effect", &tDesc)))
			return;

		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_WIND, CElement_OHD::OVERHEAD_ICE);
		break;
	case Client::REACT_SUPERCONDUCT:		// 초전도
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_ICE, CElement_OHD::OVERHEAD_LIGHTNING);
		break;
	case Client::REACT_ELECTROCHARGED:		// 감전
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_WATER, CElement_OHD::OVERHEAD_LIGHTNING);
		break;
	case Client::REACT_FROZEN:				// 빙결
		m_pElement_OHD->React_Elements(CElement_OHD::OVERHEAD_WATER, CElement_OHD::OVERHEAD_ICE);
		m_Parts[PART_BODY].pPartySlot[eIndex]->Set_Freezing(TRUE);
		break;
	case Client::REACT_CRYSTALLIZE_FIRE:	// 결정화
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
	{
		if (eIndex == PLAYER_END)
		{
			static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[m_ePlayerNum])->Set_Damage(fAdditionalDamage);
			_float fCurrentHp = static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[m_ePlayerNum])->Get_CurrentHp();
			__super::Create_DamageFont(fAdditionalDamage, eReactionElement, CPlayer::Get_Position());
			__super::Create_ReactionFont(eReact, CPlayer::Get_Position());
		}
		else
		{
			static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[eIndex])->Set_Damage(fAdditionalDamage);
			_float fCurrentHp = static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[eIndex])->Get_CurrentHp();
			__super::Create_DamageFont(fAdditionalDamage, eReactionElement, CPlayer::Get_Position(eIndex));
			__super::Create_ReactionFont(eReact, CPlayer::Get_Position(eIndex));
		}
	}
}

void CPlayer::Set_ElementUI(ELEMENT eElement)
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

_bool CPlayer::isHitCollider(list<_int> CheckList, _int ColliderID)
{
	if (CheckList.empty())
		return FALSE;

	for (auto iter : CheckList)
	{
		if (iter == ColliderID)
			return TRUE;
	}

	return FALSE;
}

HRESULT CPlayer::Create_Trail(FSM_STATE eState)
{
	if (m_ePlayerNum == PLAYER_ONE)
	{
		if (eState >= FSM_ELEMENTAL_SKILL1 && eState <= FSM_ELEMENTAL_SKILL3)
		{
			CFlameSlash_Trail::FLAMESLASH_DESC tDesc = {};
			tDesc.pParentsTransform = PlayerInfo[m_ePlayerNum].pTransform;

			if (eState == FSM_ELEMENTAL_SKILL1)
			{
				tDesc.fHeight = 0.9f;
				tDesc.fAngle = 72.f;
				tDesc.fTrailSpeed = 1.2f;
				tDesc.bTrailReverse = TRUE;
			}
			else if (eState == FSM_ELEMENTAL_SKILL2)
			{
				tDesc.fHeight = 0.9f;
				tDesc.fAngle = 145.f;
				tDesc.fTrailSpeed = 1.2f;
				tDesc.bTrailReverse = FALSE;
			}
			else if (eState == FSM_ELEMENTAL_SKILL3)
			{
				tDesc.fHeight = 0.9f;
				tDesc.fAngle = 90.f;
				tDesc.fTrailSpeed = 1.2f;
				tDesc.bTrailReverse = TRUE;
			}

			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_FlameSlash_Trail", L"Layer_Effect", &tDesc)))
				return E_FAIL;
		}
		else
		{
			CSlash_Trail::SLASH_DESC tDesc = {};
			tDesc.pParentsTransform = PlayerInfo[m_ePlayerNum].pTransform;

			if (eState == FSM_ATTACK1)
			{
				tDesc.fHeight = 0.9f;
				tDesc.fAngle = 20.f;
				tDesc.fTrailSpeed = 1.f;
				tDesc.bTrailReverse = TRUE;
			}
			else if (eState == FSM_ATTACK2)
			{
				tDesc.fHeight = 0.9f;
				tDesc.fAngle = 60.f;
				tDesc.fTrailSpeed = 1.2f;
				tDesc.bTrailReverse = FALSE;
			}
			else if (eState == FSM_ATTACK3)
			{
				tDesc.fHeight = 0.9f;
				tDesc.fAngle = 0.f;
				tDesc.fTrailSpeed = 1.f;
				tDesc.bTrailReverse = FALSE;
			}
			else if (eState == FSM_ATTACK4)
			{
				tDesc.fHeight = 0.9f;
				tDesc.fAngle = 90.f;
				tDesc.fTrailSpeed = 1.f;
				tDesc.bTrailReverse = TRUE;
			}
			else if (eState == FSM_ATTACK_CHARGED_READY || eState == FSM_ATTACK_CHARGED1 || eState == FSM_ATTACK_CHARGED2)
			{
				switch (m_iExtraAttackCount)
				{
				case 1:
					tDesc.fHeight = 0.9f;
					tDesc.fAngle = -15.f;
					tDesc.fTrailSpeed = 1.2f;
					tDesc.bTrailReverse = FALSE;
					break;

				case 2:
					tDesc.fHeight = 0.9f;
					tDesc.fAngle = 15.f;
					tDesc.fTrailSpeed = 1.2f;
					tDesc.bTrailReverse = TRUE;
					break;

				case 3:
					tDesc.fHeight = 0.9f;
					tDesc.fAngle = -15.f;
					tDesc.fTrailSpeed = 1.2f;
					tDesc.bTrailReverse = FALSE;
					break;

				case 4:
					tDesc.fHeight = 0.9f;
					tDesc.fAngle = 0.f;
					tDesc.fTrailSpeed = 1.2f;
					tDesc.bTrailReverse = FALSE;
					break;

				default:
					break;
				}
			}

			//cout << m_iExtraAttackCount << endl;

			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Slash_Trail", L"Layer_Effect", &tDesc)))
				return E_FAIL;
		}
	}
	else if (m_ePlayerNum == PLAYER_TWO)
	{
		if (eState == FSM_ATTACK_READY || eState == FSM_ATTACK1 || eState == FSM_ATTACK3 || eState == FSM_ATTACK4 || eState == FSM_ATTACK6) // 찌르기
		{
			CPiercing_Trail::PIERCING_DESC tDesc = {};
			tDesc.fTrailSpeed = 4.f;
			tDesc.pCombineWorldMatrix = static_cast<CPart_Weapon*>(m_Parts[PART_WEAPON].pPartySlot[m_ePlayerNum])->Get_CombinedWorldMatrix();
			tDesc.pModelCom = static_cast<CModel*>(m_Parts[PART_WEAPON].pPartySlot[m_ePlayerNum]->Find_Component(TEXT("Com_Model")));
			if (eState == FSM_ATTACK6)
				tDesc.vScale = _float3(3.f, 9.f, 1.f);
			else
				tDesc.vScale = _float3(2.f, 6.f, 1.f);

			tDesc.bRotation = FALSE;
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Eff_Piercing_Trail", L"Layer_Effect", &tDesc)))
				return E_FAIL;

			tDesc.bRotation = TRUE;
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Eff_Piercing_Trail", L"Layer_Effect", &tDesc)))
				return E_FAIL;

		}
		else if (eState == FSM_ATTACK2) // 베기
		{
			CSlash_Trail::SLASH_DESC tDesc = {};
			tDesc.pParentsTransform = PlayerInfo[m_ePlayerNum].pTransform;
			tDesc.fHeight = 0.9f;
			tDesc.fAngle = 0.f;
			tDesc.fTrailSpeed = 1.5f;
			tDesc.bTrailReverse = FALSE;

			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Slash_Trail", L"Layer_Effect", &tDesc)))
				return E_FAIL;
		}
		else if (eState == FSM_ATTACK5) // 빙글
		{
			CSlash_Trail::SLASH_DESC tDesc = {};
			tDesc.pParentsTransform = PlayerInfo[m_ePlayerNum].pTransform;
			tDesc.fHeight = 0.9f;
			tDesc.fAngle = 90.f;
			tDesc.fTrailSpeed = 6.f;
			tDesc.bTrailReverse = TRUE;
			tDesc.vMoveLook = 1.6f;
			tDesc.eTrail_Type = CSlash_Trail::TRAIL_RING;

			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Slash_Trail", L"Layer_Effect", &tDesc)))
				return E_FAIL;
		}
	}
	else if (m_ePlayerNum == PLAYER_FOUR)
	{
		CSlash_Trail::SLASH_DESC tDesc = {};
		tDesc.pParentsTransform = PlayerInfo[m_ePlayerNum].pTransform;

		if (eState == FSM_ATTACK1)
		{
			tDesc.fHeight = 0.9f;
			tDesc.fAngle = 105.f;
			tDesc.fTrailSpeed = 1.f;
			tDesc.bTrailReverse = FALSE;
			m_pGameInstance->PlaySoundW(L"Raiden_B_Attack1.mp3");
		}
		else if (eState == FSM_ATTACK2)
		{
			tDesc.fHeight = 0.9f;
			tDesc.fAngle = 0.f;
			tDesc.fTrailSpeed = 1.f;
			tDesc.bTrailReverse = FALSE;
			m_pGameInstance->PlaySoundW(L"Raiden_B_Attack2.mp3");
		}
		else if (eState == FSM_ATTACK3)
		{
			tDesc.fHeight = 0.9f;
			tDesc.fAngle = 100.f;
			tDesc.fTrailSpeed = 1.f;
			tDesc.bTrailReverse = TRUE;
			m_pGameInstance->PlaySoundW(L"Raiden_B_Attack3.mp3");
		}
		else if (eState == FSM_ATTACK4)
		{
			if (m_iBasicAttackCount == 0)
			{
				m_iBasicAttackCount++;
				tDesc.fHeight = 0.9f;
				tDesc.fAngle = 0.f;
				tDesc.fTrailSpeed = 1.f;
				tDesc.bTrailReverse = TRUE;
				m_pGameInstance->PlaySoundW(L"Raiden_B_Attack4-1.mp3");
			}
			else
			{
				m_iBasicAttackCount = 0;
				tDesc.fHeight = 0.9f;
				tDesc.fAngle = 20.f;
				tDesc.fTrailSpeed = 1.f;
				tDesc.bTrailReverse = FALSE;
				m_pGameInstance->PlaySoundW(L"Raiden_B_Attack4-2.mp3");
			}
		}
		else
		{
			tDesc.fHeight = 0.9f;
			tDesc.fAngle = 0.f;
			tDesc.fTrailSpeed = 1.f;
			tDesc.bTrailReverse = FALSE;
			m_pGameInstance->PlaySoundW(L"Raiden_B_Attack5.mp3");
		}
		if (m_bRaidenUltOn == true)
		{
			tDesc.eElementColor = ELEMENT_ELECTRIC;
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Raiden_Trail_Add", L"Layer_Effect", &tDesc)))
				return E_FAIL;
		}

		if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Slash_Trail", L"Layer_Effect", &tDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer::Create_Trail(FSM_STATE eState, PLAYERINDEX eIndex)
{
	if (eIndex == PLAYER_ONE)
	{
		if (eState >= FSM_ELEMENTAL_SKILL1 && eState <= FSM_ELEMENTAL_SKILL3)
		{
			CFlameSlash_Trail::FLAMESLASH_DESC tDesc = {};
			tDesc.pParentsTransform = PlayerInfo[eIndex].pTransform;

			if (eState == FSM_ELEMENTAL_SKILL1)
			{
				tDesc.fHeight = 0.9f;
				tDesc.fAngle = 72.f;
				tDesc.fTrailSpeed = 1.2f;
				tDesc.bTrailReverse = TRUE;
			}
			else if (eState == FSM_ELEMENTAL_SKILL2)
			{
				tDesc.fHeight = 0.9f;
				tDesc.fAngle = 145.f;
				tDesc.fTrailSpeed = 1.2f;
				tDesc.bTrailReverse = FALSE;
			}
			else if (eState == FSM_ELEMENTAL_SKILL3)
			{
				tDesc.fHeight = 0.9f;
				tDesc.fAngle = 90.f;
				tDesc.fTrailSpeed = 1.2f;
				tDesc.bTrailReverse = TRUE;
			}

			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_FlameSlash_Trail", L"Layer_Effect", &tDesc)))
				return E_FAIL;
		}
		else
		{
			CSlash_Trail::SLASH_DESC tDesc = {};
			tDesc.pParentsTransform = PlayerInfo[eIndex].pTransform;

			if (eState == FSM_ATTACK1)
			{
				tDesc.fHeight = 0.9f;
				tDesc.fAngle = 20.f;
				tDesc.fTrailSpeed = 1.f;
				tDesc.bTrailReverse = TRUE;
			}
			else if (eState == FSM_ATTACK2)
			{
				tDesc.fHeight = 0.9f;
				tDesc.fAngle = 60.f;
				tDesc.fTrailSpeed = 1.2f;
				tDesc.bTrailReverse = FALSE;
			}
			else if (eState == FSM_ATTACK3)
			{
				tDesc.fHeight = 0.9f;
				tDesc.fAngle = 0.f;
				tDesc.fTrailSpeed = 1.f;
				tDesc.bTrailReverse = FALSE;
			}
			else if (eState == FSM_ATTACK4)
			{
				tDesc.fHeight = 0.9f;
				tDesc.fAngle = 90.f;
				tDesc.fTrailSpeed = 1.f;
				tDesc.bTrailReverse = TRUE;
			}
			else if (eState == FSM_ATTACK_CHARGED_READY || eState == FSM_ATTACK_CHARGED1 || eState == FSM_ATTACK_CHARGED2)
			{
				switch (m_iExtraAttackCount)
				{
				case 1:
					tDesc.fHeight = 0.9f;
					tDesc.fAngle = -15.f;
					tDesc.fTrailSpeed = 1.2f;
					tDesc.bTrailReverse = FALSE;
					break;

				case 2:
					tDesc.fHeight = 0.9f;
					tDesc.fAngle = 15.f;
					tDesc.fTrailSpeed = 1.2f;
					tDesc.bTrailReverse = TRUE;
					break;

				case 3:
					tDesc.fHeight = 0.9f;
					tDesc.fAngle = -15.f;
					tDesc.fTrailSpeed = 1.2f;
					tDesc.bTrailReverse = FALSE;
					break;

				case 4:
					tDesc.fHeight = 0.9f;
					tDesc.fAngle = 0.f;
					tDesc.fTrailSpeed = 1.2f;
					tDesc.bTrailReverse = FALSE;
					break;

				default:
					break;
				}
			}

			//cout << m_iExtraAttackCount << endl;

			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Slash_Trail", L"Layer_Effect", &tDesc)))
				return E_FAIL;
		}
	}
	else if (eIndex == PLAYER_TWO)
	{
		if (eState == FSM_ATTACK_READY || eState == FSM_ATTACK1 || eState == FSM_ATTACK3 || eState == FSM_ATTACK4 || eState == FSM_ATTACK6) // 찌르기
		{
			CPiercing_Trail::PIERCING_DESC tDesc = {};
			tDesc.fTrailSpeed = 4.f;
			tDesc.pCombineWorldMatrix = static_cast<CPart_Weapon*>(m_Parts[PART_WEAPON].pPartySlot[eIndex])->Get_CombinedWorldMatrix();
			tDesc.pModelCom = static_cast<CModel*>(m_Parts[PART_WEAPON].pPartySlot[eIndex]->Find_Component(TEXT("Com_Model")));
			if (eState == FSM_ATTACK6)
				tDesc.vScale = _float3(3.f, 9.f, 1.f);
			else
				tDesc.vScale = _float3(2.f, 6.f, 1.f);

			tDesc.bRotation = FALSE;
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Eff_Piercing_Trail", L"Layer_Effect", &tDesc)))
				return E_FAIL;

			tDesc.bRotation = TRUE;
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Eff_Piercing_Trail", L"Layer_Effect", &tDesc)))
				return E_FAIL;

		}
		else if (eState == FSM_ATTACK2) // 베기
		{
			CSlash_Trail::SLASH_DESC tDesc = {};
			tDesc.pParentsTransform = PlayerInfo[eIndex].pTransform;
			tDesc.fHeight = 0.9f;
			tDesc.fAngle = 0.f;
			tDesc.fTrailSpeed = 1.5f;
			tDesc.bTrailReverse = FALSE;

			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Slash_Trail", L"Layer_Effect", &tDesc)))
				return E_FAIL;
		}
		else if (eState == FSM_ATTACK5) // 빙글
		{
			CSlash_Trail::SLASH_DESC tDesc = {};
			tDesc.pParentsTransform = PlayerInfo[eIndex].pTransform;
			tDesc.fHeight = 0.9f;
			tDesc.fAngle = 90.f;
			tDesc.fTrailSpeed = 6.f;
			tDesc.bTrailReverse = TRUE;
			tDesc.vMoveLook = 1.6f;
			tDesc.eTrail_Type = CSlash_Trail::TRAIL_RING;

			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Slash_Trail", L"Layer_Effect", &tDesc)))
				return E_FAIL;
		}
	}
	else if (eIndex == PLAYER_FOUR)
	{
		CSlash_Trail::SLASH_DESC tDesc = {};
		tDesc.pParentsTransform = PlayerInfo[eIndex].pTransform;


		if (eState == FSM_ATTACK1)
		{
			tDesc.fHeight = 0.9f;
			tDesc.fAngle = 105.f;
			tDesc.fTrailSpeed = 1.f;
			tDesc.bTrailReverse = FALSE;
		}
		else if (eState == FSM_ATTACK2)
		{
			tDesc.fHeight = 0.9f;
			tDesc.fAngle = 0.f;
			tDesc.fTrailSpeed = 1.f;
			tDesc.bTrailReverse = FALSE;
		}
		else if (eState == FSM_ATTACK3)
		{
			tDesc.fHeight = 0.9f;
			tDesc.fAngle = 100.f;
			tDesc.fTrailSpeed = 1.f;
			tDesc.bTrailReverse = TRUE;
		}
		else if (eState == FSM_ATTACK4)
		{
			if (m_iBasicAttackCount == 0)
			{
				m_iBasicAttackCount++;
				tDesc.fHeight = 0.9f;
				tDesc.fAngle = 0.f;
				tDesc.fTrailSpeed = 1.f;
				tDesc.bTrailReverse = TRUE;
			}
			else
			{
				m_iBasicAttackCount = 0;
				tDesc.fHeight = 0.9f;
				tDesc.fAngle = 20.f;
				tDesc.fTrailSpeed = 1.f;
				tDesc.bTrailReverse = FALSE;
			}
		}
		else
		{
			tDesc.fHeight = 0.9f;
			tDesc.fAngle = 0.f;
			tDesc.fTrailSpeed = 1.f;
			tDesc.bTrailReverse = FALSE;
		}

		if (m_bRaidenUltOn == true)
		{
			tDesc.eElementColor = ELEMENT_ELECTRIC;
			if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Raiden_Trail_Add", L"Layer_Effect", &tDesc)))
				return E_FAIL;
		}

		if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Slash_Trail", L"Layer_Effect", &tDesc)))
			return E_FAIL;

	}

	return S_OK;
}

void CPlayer::Look_MonsterTarget()
{ 
	if (m_MonsterTarget.size() == 0)
	{
		if (m_ePlayerNum == PLAYER_THREE)
			XMStoreFloat4(&m_vTargetMonsterPos, PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_LOOK)) * 3.f);
		return;
	}

	FSM_STATE eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Get_CurrentState());
	if (m_ePlayerNum == PLAYER_TWO && eState >= FSM_ATTACK_CHARGED1 && eState <= FSM_ATTACK_CHARGED_END)
		return;

	CTransform* pMonsterTransform = nullptr;
	_vector vCurrentPos = {};
	_vector vSetDir = {};
	_float  fCurrentDistance = 0.f, fMinDistance = 0.f;
	_vector vPlayerPos = PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_POSITION);
	_int iIndex = 0, iMinIndex = 0;

	for (auto& pMonsterTarget : m_MonsterTarget)
	{
		if (pMonsterTarget->Get_Destroy() == TRUE)
			continue;

		pMonsterTransform = static_cast<CTransform*>(pMonsterTarget->Find_Component(TEXT("Com_Transform")));
		vCurrentPos = pMonsterTransform->Get_State(CTransform::STATE_POSITION);
		
		if (static_cast<CCreature*>(pMonsterTarget)->Get_CreatureID() == MONSTER_ANDRIUS)
		{
			_vector vMonsterLook = pMonsterTransform->Get_State(CTransform::STATE_LOOK);
			vMonsterLook.m128_f32[1] = 0.f; vMonsterLook = XMVector4Normalize(vMonsterLook);
			vCurrentPos += vMonsterLook * 4.f;
		}
		
		vSetDir = vCurrentPos - vPlayerPos;
		fCurrentDistance = XMVector3Length(vSetDir).m128_f32[0];

		if (iIndex == 0)
		{
			fMinDistance = fCurrentDistance;
			iMinIndex = iIndex;
		}

		if (fMinDistance >= fCurrentDistance)
		{
			iMinIndex = iIndex;
			fMinDistance = fCurrentDistance;
		}

		iIndex++;
	}

	if (fMinDistance > 8.f || fMinDistance < 1.f)
		return;

	pMonsterTransform = static_cast<CTransform*>(m_MonsterTarget[iMinIndex]->Find_Component(TEXT("Com_Transform")));
	vCurrentPos = pMonsterTransform->Get_State(CTransform::STATE_POSITION);

	if (static_cast<CCreature*>(m_MonsterTarget[iMinIndex])->Get_CreatureID() == MONSTER_ANDRIUS)
	{
		_vector vMonsterLook = pMonsterTransform->Get_State(CTransform::STATE_LOOK);
		vMonsterLook.m128_f32[1] = 0.f; vMonsterLook = XMVector4Normalize(vMonsterLook);
		vCurrentPos += vMonsterLook * 5.f;
	}

	XMStoreFloat4(&m_vTargetMonsterPos, vCurrentPos);
	vSetDir = vCurrentPos - vPlayerPos;
	vSetDir.m128_f32[1] = 0.f; vSetDir = XMVector3Normalize(vSetDir);

	PlayerInfo[m_ePlayerNum].pTransform->Reset_Look_At_Direction();
	PlayerInfo[m_ePlayerNum].pTransform->Set_Look_At_Direction(vSetDir);
}

_bool CPlayer::isFunctionTrue()
{
	return static_cast<CPart_Body*>(m_Parts[PART_BODY].pPartySlot[m_ePlayerNum])->isFunctionTrue();
}

_bool CPlayer::isFunctionTrue(PLAYERINDEX eIndex)
{
	return static_cast<CPart_Body*>(m_Parts[PART_BODY].pPartySlot[eIndex])->isFunctionTrue(); return _bool();
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize_Clone(void* pArg)
{
	PlayerInfo.resize(4);

	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (FAILED(Add_ExtraComponent()))
		return E_FAIL;

	PlayerInfo[PLAYER_ONE].pTransform = m_pTransformCom;
	PlayerInfo[PLAYER_TWO].pTransform = m_pTransformCom2P;
	PlayerInfo[PLAYER_THREE].pTransform = m_pTransformCom3P;
	PlayerInfo[PLAYER_FOUR].pTransform = m_pTransformCom4P;

	PlayerInfo[PLAYER_ONE].pFiniteStateMachineCom = m_pFiniteStateMachineCom;
	PlayerInfo[PLAYER_TWO].pFiniteStateMachineCom = m_pFiniteStateMachineCom2P;
	PlayerInfo[PLAYER_THREE].pFiniteStateMachineCom = m_pFiniteStateMachineCom3P;
	PlayerInfo[PLAYER_FOUR].pFiniteStateMachineCom = m_pFiniteStateMachineCom4P;

	for (size_t i = 0; i < PLAYER_END; i++)
	{
		PlayerInfo[i].pTransform->Set_Scale({ 1.f, 1.f, 1.f, 0.f });
	}

	/* 플레이어 위치 결정 */
	_float4 vPosition = { };
	if (m_pGameInstance->Get_ReservePosition().w != 0)
	{
		vPosition = m_pGameInstance->Get_ReservePosition();
		m_pGameInstance->Reset_ReservePosition();
	}
	else
	{
		switch (static_cast<CLevel_Loading*>(m_pGameInstance->Get_CurrentLevelObject())->Get_NextLevel())
		{
		case LEVELID::LEVEL_WORLD:					// 월드 시작 위치
			vPosition = _float4(536.212f, 14.0785f, 255.44f, 1.f);
			break; 
		case LEVELID::LEVEL_TEMPLE_LION:			// 사원 시작 위치
			vPosition = _float4(-550.0f, -43.0f, -38.0, 1.0f);
			break;
		case LEVELID::LEVEL_STRONGHOLD_ABYSSMAGE:	// 요새 시작 위치
			vPosition = _float4(-63.0f, 10.0f, -39.0, 1.0f);
			break;
		case LEVELID::LEVEL_STORMTERROR:			// 드발린 시작 위치
			vPosition = _float4(-101.f, 0.f, 1.f, 1.0f);
			break;
		default:
			vPosition = _float4(0.f, 0.f, 0.f, 1.0f);
			break;
		}
	}

	//_float4 vPos =  _float4(171.3f, 60.2f, 257.2f, 1.f); /* 성당 앞 */
	//_float4 vPos = _float4(375.0f, 61.0f, 672.0f, 1.f); /* 드발린 입구 */
	//_float4 vPos = _float4(953.0f, 3.0f, 187.0f, 1.0f);	
	_float4 vPos = _float4(953.0f, 3.0f, 187.0f, 1.0f);	
	//_float4 vPos = _float4(1.0f, 1.f, 1.0f, 1.0f);
	//_float4 vPos = _float4(256.0f, 52.f, 256.0f, 1.0f);
	//_float4 vPos = _float4(434.5f, 11.f, 263.2f, 1.0f);
	//_float4 vPos = _float4(416.5f, 12.11684f, 254.2f, 1.0f);

	//_float4 vPos = _float4{-400.4520f, -37.1130f, -8.3300f, 1.f}; // 요새 기믹 테스트
	//_float4 vPos = _float4(4.0f, -0.4f, -46.0, 1.0f); // 퀘스트 시작 위치
	//_float4 vPos = _float4(807.0, 25.f, 185.0f, 1.0f);

	m_ePlayerNum = PLAYER_ONE;
	PlayerInfo[m_ePlayerNum].pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPosition));
	m_pTransformCom->RotationFromCurrentState(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(-90.f));

	/*PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Append_GeneralState(vector<_int>{ FSM_JUMP, FSM_FLY_START, FSM_FLY_NORMAL, FSM_FLY_LEFT, FSM_FLY_RIGHT, FSM_DROP_DOWN,
																FSM_FALL_ATTACK_START1, FSM_FALL_ATTACK_START2, FSM_FALL_ATTACK, FSM_FALL_ATTACK_END1, FSM_FALL_ATTACK_END2,
																FSM_ATTACK1, FSM_ATTACK2, FSM_ATTACK3, FSM_ATTACK4, FSM_ATTACK5, FSM_ATTACK6,
																FSM_ATTACK_CHARGED1, FSM_ATTACK_CHARGED2, FSM_ELEMENTAL_BURST, FSM_ELEMENTAL_BURST_IDLE,
															 	FSM_ELEMENTAL_SKILL1, FSM_ELEMENTAL_SKILL2, FSM_ELEMENTAL_SKILL3,
																FSM_ELEMENTAL_SKILL1_HOLD,
																FSM_ELEMENTAL_SKILL1_IDLE, FSM_ELEMENTAL_SKILL2_IDLE, FSM_ELEMENTAL_SKILL3_IDLE,
																FSM_HIT_L, FSM_HIT_H, FSM_HIT_THROW, FSM_HIT_THROW_FLY, FSM_HIT_ROLLING,
																FSM_DEAD1, FSM_DEAD2});*/

	for (size_t i = 0; i < PLAYER_END; i++)
	{
		PlayerInfo[i].pFiniteStateMachineCom->Append_GeneralState(vector<_int>{ FSM_JUMP, FSM_FLY_START, FSM_FLY_NORMAL, FSM_FLY_LEFT, FSM_FLY_RIGHT, FSM_DROP_DOWN,
			FSM_FALL_ATTACK_START1, FSM_FALL_ATTACK_START2, FSM_FALL_ATTACK, FSM_FALL_ATTACK_END1, FSM_FALL_ATTACK_END2,
			FSM_ATTACK1, FSM_ATTACK2, FSM_ATTACK3, FSM_ATTACK4, FSM_ATTACK5, FSM_ATTACK6,
			FSM_ATTACK_CHARGED1, FSM_ATTACK_CHARGED2, FSM_ELEMENTAL_BURST, FSM_ELEMENTAL_BURST_IDLE,
			FSM_ELEMENTAL_SKILL1, FSM_ELEMENTAL_SKILL2, FSM_ELEMENTAL_SKILL3,
			FSM_ELEMENTAL_SKILL1_HOLD,
			FSM_ELEMENTAL_SKILL1_IDLE, FSM_ELEMENTAL_SKILL2_IDLE, FSM_ELEMENTAL_SKILL3_IDLE,
			FSM_HIT_L, FSM_HIT_H, FSM_HIT_THROW, FSM_HIT_THROW_FLY, FSM_HIT_ROLLING,
			FSM_DEAD1, FSM_DEAD2, FSM_SHOW});
	}

	PlayerInfo[PLAYER_ONE].pController = CCharacterController::Create(0.75f, 0.25f, 0.5f, { 0.f,0.f,0.f,1.f }, PlayerInfo[PLAYER_ONE].pTransform);
	PlayerInfo[PLAYER_TWO].pController = CCharacterController::Create(0.75f, 0.25f, 0.5f, { 0.f,0.f,0.f,1.f }, PlayerInfo[PLAYER_TWO].pTransform);
	PlayerInfo[PLAYER_THREE].pController = CCharacterController::Create(0.75f, 0.25f, 0.5f, { 0.f,0.f,0.f,1.f }, PlayerInfo[PLAYER_THREE].pTransform);
	PlayerInfo[PLAYER_FOUR].pController = CCharacterController::Create(0.75f, 0.25f, 0.5f, { 0.f,0.f,0.f,1.f }, PlayerInfo[PLAYER_FOUR].pTransform);

	PlayerInfo[PLAYER_ONE].pController->Set_Position(vPosition);
	PlayerInfo[PLAYER_TWO].pController->Set_Position({ 0.f,-100.f,0.f,1.f });
	PlayerInfo[PLAYER_THREE].pController->Set_Position({ 0.f,-100.f,0.f,1.f });
	PlayerInfo[PLAYER_FOUR].pController->Set_Position({ 0.f,-100.f,0.f,1.f });

	if (FAILED(Add_Parts(pArg)))
		return E_FAIL;

	//COrtho::ORTHO_DESC tOrthoDesc;
	//tOrthoDesc.eTextureLevel = LEVEL_WORLD;
	//tOrthoDesc.strTextureTag = TEXT("Prototype_Component_Texture_UI_Interaction_Box");
	//tOrthoDesc.fPosition = { static_cast<_float>(1080.f - g_iWinSizeX / 2), static_cast<_float>(-(450.f - g_iWinSizeY / 2)) };
	//tOrthoDesc.fSize = { 240.f,45.f };
	//tOrthoDesc.ePage = PAGE_MAIN;
	//m_pInteractionBox = (CAlphaRect*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_AlphaRect"))->Clone(&tOrthoDesc);
	//m_pInteractionBox->Set_Text(TEXT("대화한다"), CAlphaRect::TEXT_ON);


	COrtho::ORTHO_DESC tOrthoDesc = { };
	tOrthoDesc.fPosition = { 0.f,0.f };
	tOrthoDesc.fSize = { 65.f, 68.f };
	tOrthoDesc.ePage = PAGE_MAIN;
	tOrthoDesc.iDepth = 0;

	m_pElement_OHD = (CElement_OHD*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Element_OHD"))->Clone(&tOrthoDesc);

	return S_OK;
}

HRESULT CPlayer::PreviousTick(_float fTimeDelta)
{
	/*if (m_bStartBGM == FALSE)
	{
		m_bStartBGM = TRUE;
		if (m_pGameInstance->Get_CurrentLevel() == LEVEL_WORLD)
		{
			m_pGameInstance->Register_Interaction(L"BattleBGM");
			_vector vCurrentPos = CPlayer::Get_Position();
			vCurrentPos.m128_f32[1] = 0.f;
			_vector vMondCenter = XMVectorSet(234, 0.f, 256, 1.f);
			_float	fLength = XMVectorGetX(XMVector3Length(vMondCenter - vCurrentPos));
			
			if (fLength <= 215.f)
				m_pGameInstance->PlayBGM(L"MondstadtBGM.mp3", 0.5f);
			else
				m_pGameInstance->PlayBGM(L"Mond_Field.mp3", 0.5f);
		}
	}*/

	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (m_bRegion == FALSE)
		{
			m_bRegion = TRUE;
			if (m_pGameInstance->Get_CurrentLevel() == LEVEL_WORLD)
			{
				//m_pPageMain->Display_Region_Name(TEXT("몬드"));
				//m_pGameInstance->PlayBGM(L"MondstadtBGM.mp3", 0.4f);

				m_pGameInstance->Register_Interaction(L"BattleBGM");
				_vector vCurrentPos = CPlayer::Get_Position();
				vCurrentPos.m128_f32[1] = 0.f;
				_vector vMondCenter = XMVectorSet(234, 0.f, 256, 1.f);
				_float	fLength = XMVectorGetX(XMVector3Length(vMondCenter - vCurrentPos));

				if (fLength <= 215.f)
				{
					Set_LocationID(LOCATION_MOND_CASTLE);
				}
				else
				{
					Set_LocationID(LOCATION_MOND_FIELD);
				}
			}
			else if (m_pGameInstance->Get_CurrentLevel() == LEVEL_STRONGHOLD_ABYSSMAGE)
			{
				m_pPageMain->Display_Region_Name(TEXT("시련의 비경"));
				m_pGameInstance->PlayBGM(L"Abyss_BGM.mp3", 0.4f);
			}
			else if (m_pGameInstance->Get_CurrentLevel() == LEVEL_STORMTERROR)
			{
				m_pPageMain->Display_Region_Name(TEXT("바람 드래곤의 폐허"));
			}
		}

		if (PlayerInfo[m_ePlayerNum].pController->Get_InAir() == FALSE) // 땅에 닿아있는 상태일때
		{
			XMStoreFloat4(&m_vLastPosition, PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_POSITION));
		}
		else
		{
			_float fHeight = 0.f;
			LEVELID eCurrentLevel = (LEVELID)m_pGameInstance->Get_CurrentLevel();
			_vector vCurrentPos = PlayerInfo[m_ePlayerNum].pTransform->Get_State(CTransform::STATE_POSITION);
			switch (eCurrentLevel)
			{
			case Client::LEVEL_WORLD:
				fHeight = -1.f;
				break;
			case Client::LEVEL_TEMPLE_LION:
				fHeight = -60.f;
				break;
			case Client::LEVEL_STRONGHOLD_ABYSSMAGE:
				fHeight = -60.f;
				break;
			case Client::LEVEL_STORMTERROR:
				fHeight = -25.f;
				break;
			default:
				fHeight = -100.f;
				break;
			}

			if (vCurrentPos.m128_f32[1] <= fHeight)
			{
				PlayerInfo[m_ePlayerNum].pController->Set_Position(m_vLastPosition);
				Set_Damage(m_pGameInstance->Get_Float_Random(1000.f, 2000.f), ELEMENT_END, FSM_HIT_THROW);
			}
		}


		if (FAILED(__super::PreviousTick(fTimeDelta)))
			return E_FAIL;

		if (m_bCombatMode == TRUE)
		{
			for (_uint i = 0; i < PLAYER_END; ++i)
			{
				if (FAILED(CPlayer::MoveSpeed_Control(fTimeDelta, (PLAYERINDEX)i)))
					return E_FAIL;

				if (m_fInvincibility[i] > 0.f)
					m_fInvincibility[i] -= fTimeDelta;
				else if (m_bInvincibility[i] == TRUE)
				{
					m_bInvincibility[i] = FALSE;
					m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, m_Parts[PART_BODY].pPartySlot[i]->Get_CollisionID(), TRUE);
				}
			}
		}
		else
		{
			if (FAILED(CPlayer::MoveSpeed_Control(fTimeDelta, m_ePlayerNum)))
				return E_FAIL;

			if (m_fInvincibility[m_ePlayerNum] > 0.f)
				m_fInvincibility[m_ePlayerNum] -= fTimeDelta;
			else if (m_bInvincibility[m_ePlayerNum] == TRUE)
			{
				m_bInvincibility[m_ePlayerNum] = FALSE;
				m_pGameInstance->Set_IsCollision(COLL_PLAYER_BODY, m_Parts[PART_BODY].pPartySlot[m_ePlayerNum]->Get_CollisionID(), TRUE);
			}
		}

		if (FAILED(CPlayer::Character_Dead(fTimeDelta)))
			return E_FAIL;

		if (m_Parts[PART_BODY].pPartySlot[m_ePlayerNum]->Get_Freezing() == FALSE)
		{
			if (FAILED(CPlayer::Input_State(fTimeDelta)))
				return E_FAIL;
		}

		m_pElement_OHD->Zero_Elements();
		for (_uint i = 0; i < ELEMENT_END; ++i)
		{
			if (m_Parts[PART_BODY].pPartySlot[m_ePlayerNum]->isHostElement((ELEMENT)i) == TRUE)
				CPlayer::Set_ElementUI((ELEMENT)i);
		}

		if (m_bCombatMode == FALSE) /* Player 1명만 온필드 */
		{
			for (auto& pPart : m_Parts)
			{
				pPart.pPartySlot[m_ePlayerNum]->PreviousTick(fTimeDelta);
			}
		}
		else /* Player 자동전투모드 */
		{
			for (auto& pPart : m_Parts)
			{
				for (_int iSlot = 0; iSlot < MAX_PARTYSLOT; ++iSlot)
				{
					if (pPart.pPartySlot[iSlot] != nullptr)
						pPart.pPartySlot[iSlot]->PreviousTick(fTimeDelta);
				}
			}
		}


		m_pElement_OHD->PreviousTick(fTimeDelta);
		// m_pOverHeadUI->PreviousTick(fTimeDelta);
	}

	return S_OK;
}

HRESULT CPlayer::Tick(_float fTimeDelta)
{
#pragma region
	if (m_pGameInstance->Key_Pressing(VK_LSHIFT) && m_pGameInstance->Key_Down('0') && m_pGameInstance->Get_CurrentLevel() == LEVEL_STORMTERROR)
	{
		OBJECT_DESC		ObjDesc = {};
		ObjDesc.vInitPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		ObjDesc.iObjectType = MONSTER_DVALIN;
		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_STORMTERROR, TEXT("Prototype_GameObject_Dvalin"), L"Layer_Boss", &ObjDesc)))
			return E_FAIL;
	}
#pragma endregion

	if (m_bBgmChange == TRUE) // 전투 시 배경음 변경
	{
		if (m_pGameInstance->Get_CurrentLevel() == LEVEL_WORLD)
		{
			m_fSoundChange += fTimeDelta;
			_float fVolume = 0.5f - m_fSoundChange;
			if (fVolume < 0.f)
				fVolume = 0.f;
			m_pGameInstance->SetChannelVolume(SOUND_BGM, fVolume);

			if (m_fSoundChange > 0.5f)
			{
				m_bBgmChange = FALSE;
				m_fSoundChange = 0.f;
				if (m_eLocation == LOCATION_MOND_FIELD)
				{
					if (m_bBattleSound == TRUE)
						m_pGameInstance->PlayBGM(L"BattleStart.mp3", 0.5f);
					else
						m_pGameInstance->PlayBGM(L"Mond_Field.mp3", 0.5f);

					if (m_bLocationFont == TRUE)
					{
						m_pPageMain->Display_Region_Name(TEXT("바람이 부는 곳"));
					}
					else
						m_bLocationFont = TRUE;
				}
				else if (m_eLocation == LOCATION_MOND_CASTLE)
				{
					m_pGameInstance->PlayBGM(L"MondstadtBGM.mp3", 0.5f);
					if (m_bLocationFont == TRUE)
					{
						m_pPageMain->Display_Region_Name(TEXT("몬드"));
					}
					else
						m_bLocationFont = TRUE;
				}
			}
		}
		
	}
	else if(m_pGameInstance->Get_CurrentLevel() == LEVEL_WORLD)
	{
		if (m_pGameInstance->Get_InteractionCount(L"BattleBGM") != 0 && m_bBattleSound == FALSE)
		{
			m_bBgmChange = TRUE;
			m_bBattleSound = TRUE;
			m_bLocationFont = FALSE;
		}
		else if (m_pGameInstance->Get_InteractionCount(L"BattleBGM") == 0 && m_bBattleSound == TRUE)
		{
			m_bBgmChange = TRUE;
			m_bBattleSound = FALSE;
			m_bLocationFont = FALSE;
		}
	}

	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (m_bRaidenUltOn == true)
		{
			m_fRaidenUltTime -= fTimeDelta;
			if (m_fRaidenUltTime < 0.f)
				m_bRaidenUltOn = false;
		}

		if (m_bCombatMode == FALSE)/* Player 1명만 온필드 */
		{
			if (FAILED(CPlayer::Hit_Collision_State(fTimeDelta)))
				return E_FAIL;
		}
		else /* Auto 모드 */
		{
			for (_uint i = 0; i < PLAYER_END; ++i)
			{
				if (FAILED(CPlayer::Hit_Collision_State(fTimeDelta, (PLAYERINDEX)i)))
					return E_FAIL;
			}
		}

		if (FAILED(CPlayer::Combat_Mode(fTimeDelta)))
			return E_FAIL;

		FSM_STATE eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Get_CurrentState());
		
		//cout << eState << endl;
		
		if ((eState >= FSM_ATTACK_READY && eState <= FSM_ELEMENTAL_BURST_IDLE && eState != FSM_ELEMENTAL_BURST && eState != FSM_ELEMENTAL_BURST_IDLE && eState != FSM_SHOW)
			|| eState >= FSM_HIT_THROW)
		{
			if (m_pGameInstance->Get_CurrentLevel() != LEVEL_STORMTERROR)
			{
				if (m_ePlayerNum == PLAYER_TWO && (eState != FSM_ATTACK_CHARGED1 && eState != FSM_ATTACK_CHARGED2))
				{
					CPlayer::Look_MonsterTarget();
				}
				else
					CPlayer::Look_MonsterTarget();
			}
			else
			{
				CGameObject* pDvalin = static_cast<CGameObject*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Boss")));
				if (pDvalin != nullptr)
				{
					FSM_DVALIN_STATE pDvalinFSM = static_cast<FSM_DVALIN_STATE>(static_cast<CFiniteStateMachine*>(pDvalin->Find_Component(TEXT("Com_FiniteStateMachine")))->Get_CurrentState());
					if (pDvalinFSM != FSM_DVALIN_GROGGY_LOOP)
					{
						if (m_ePlayerNum == PLAYER_TWO && (eState != FSM_ATTACK_CHARGED1 && eState != FSM_ATTACK_CHARGED2))
						{
							CPlayer::Look_MonsterTarget();
						}
						else
							CPlayer::Look_MonsterTarget();
					}
				}
			}
		}

		if (m_bCombatMode == FALSE) /* Player 1명만 온필드 */
		{
			PlayerInfo[m_ePlayerNum].pTransform->Verify_Look_At_Direction();
			PlayerInfo[m_ePlayerNum].pTransform->Invalidate_Look_At_Lerp(fTimeDelta);

			for (auto& pPart : m_Parts)
			{
				pPart.pPartySlot[m_ePlayerNum]->Tick(fTimeDelta);
			}

			if (eState == FSM_FLY_LEFT || eState == FSM_FLY_RIGHT
				|| eState == FSM_FLY_NORMAL || eState == FSM_FLY_START)
			{
				if (m_eFlySoundChannel == MAXCHANNEL)
				{
					m_eFlySoundChannel = m_pGameInstance->Get_NextChannel();
					m_pGameInstance->PlayBGM(L"FlySound.mp3", 0.4f, m_eFlySoundChannel);
				}
				if (PlayerInfo[m_ePlayerNum].bGravity == TRUE)
					PlayerInfo[m_ePlayerNum].pController->Apply_Gravity(fTimeDelta * 0.3f);
			}
			else if (eState == FSM_DROP_DOWN || eState == FSM_FALL_ATTACK)
				PlayerInfo[m_ePlayerNum].pController->Apply_Gravity(fTimeDelta * 3.5f);
			else
			{
				if (m_eFlySoundChannel != MAXCHANNEL)
				{
					m_pGameInstance->StopSound(m_eFlySoundChannel);
					m_eFlySoundChannel = MAXCHANNEL;
				}
				PlayerInfo[m_ePlayerNum].pController->Apply_Gravity(fTimeDelta);
			}
		}
		else /* Player 자동전투모드 */
		{
			for (auto& Info : PlayerInfo)
			{
				Info.pTransform->Verify_Look_At_Direction();
				Info.pTransform->Invalidate_Look_At_Lerp(fTimeDelta);
			}

			for (auto& pPart : m_Parts)
			{
				for (_int iSlot = 0; iSlot < MAX_PARTYSLOT; ++iSlot)
				{
					if (pPart.pPartySlot[iSlot] != nullptr)
						pPart.pPartySlot[iSlot]->Tick(fTimeDelta);
				}
			}

			for (auto& Info : PlayerInfo)
			{
				FSM_STATE eState = static_cast<FSM_STATE>(Info.pFiniteStateMachineCom->Get_CurrentState());

				if (eState == FSM_FLY_LEFT || eState == FSM_FLY_RIGHT
					|| eState == FSM_FLY_NORMAL || eState == FSM_FLY_START)
				{
					if(Info.bGravity == TRUE)
						Info.pController->Apply_Gravity(fTimeDelta * 0.3f);
				}
				else if (eState == FSM_DROP_DOWN || eState == FSM_FALL_ATTACK)
					Info.pController->Apply_Gravity(fTimeDelta * 3.5f);
				else
					Info.pController->Apply_Gravity(fTimeDelta);
			}

		}

		_float fBurstTime = static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[m_ePlayerNum])->Get_ElementalBurstTime();
		_float fCurrentBurstTime = static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[m_ePlayerNum])->Get_CurBurstTime();

		_float fSkillTime = static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[m_ePlayerNum])->Get_ElementalSkillTime();
		_float fCurrentSkillTime = static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[m_ePlayerNum])->Get_CurSkillTime();

		_float fCurrentEnergy = static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[m_ePlayerNum])->Get_CurEnergy();
		_float fElementEnergy = static_cast<CPart_Status*>(m_Parts[PART_STATUS].pPartySlot[m_ePlayerNum])->Get_ElementalEnergy();

		m_pPageMain->Set_Element_Rate(fCurrentEnergy/fElementEnergy);
		m_pPageMain->Set_Skill_CoolDown_Rate(fCurrentSkillTime / fSkillTime);
		m_pPageMain->Set_Burst_CoolDown_Rate(fCurrentBurstTime / fBurstTime);

		m_pElement_OHD->Update_Matrix((_float)(661.f - (_float)g_iWinSizeX / 2.f), (_float)(-(800.f - (_float)g_iWinSizeY / 2.f)), 30.f, 30.f);
		m_pElement_OHD->Tick(fTimeDelta);

		eState = static_cast<FSM_STATE>(PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Get_CurrentState());
		if (eState != FSM_SPLINT)
			m_bSplintSound = FALSE;

		if (m_bSplintSound == FALSE && eState == FSM_SPLINT)
		{
			if (m_ePlayerNum == PLAYER_ONE)
			{
				_int iRandomVoice = m_pGameInstance->Get_Int_Random(1, 3);
				wstring soundFileName = L"VO_KO_Diluc_Sprint_Start_0" + to_wstring(iRandomVoice) + L".ogg";
				m_pGameInstance->PlaySoundW(soundFileName.c_str(), 1.f);
			}
			else if (m_ePlayerNum == PLAYER_TWO)
			{
				_int iRandomVoice = m_pGameInstance->Get_Int_Random(1, 3);
				wstring soundFileName = L"VO_KO_Zhongli_Sprint_Start_0" + to_wstring(iRandomVoice) + L".ogg";
				m_pGameInstance->PlaySoundW(soundFileName.c_str(), 1.f);
			}
			else if (m_ePlayerNum == PLAYER_THREE)
			{
				_int iRandomVoice = m_pGameInstance->Get_Int_Random(1, 3);
				wstring soundFileName = L"VO_KO_Barbara_Sprint_Start_0" + to_wstring(iRandomVoice) + L".ogg";
				m_pGameInstance->PlaySoundW(soundFileName.c_str(), 1.f);
			}
			else if (m_ePlayerNum == PLAYER_FOUR)
			{
				_int iRandomVoice = m_pGameInstance->Get_Int_Random(1, 3);
				wstring soundFileName = L"VO_KO_Raiden_Shogun_Sprint_Start_0" + to_wstring(iRandomVoice) + L".ogg";
				m_pGameInstance->PlaySoundW(soundFileName.c_str(), 1.f);
			}
			m_bSplintSound = TRUE;
		}
	}

	return S_OK;
}

HRESULT CPlayer::LateTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_RenderValidity() == S_OK)
	{
		if (FAILED(__super::LateTick(fTimeDelta)))
			return E_FAIL;

		if (m_bCombatMode == FALSE) /* Player 1명만 온필드 */
		{
			for (auto& pPart : m_Parts)
			{
				pPart.pPartySlot[m_ePlayerNum]->LateTick(fTimeDelta);
			}

			if (PlayerInfo[m_ePlayerNum].pController->Get_InAir() == FALSE)
				PlayerInfo[m_ePlayerNum].fKnockback = 0.f;

			if (PlayerInfo[m_ePlayerNum].fKnockback > 0.f && (PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Get_CurrentState() != FSM_DEAD1 || PlayerInfo[m_ePlayerNum].pFiniteStateMachineCom->Get_CurrentState() != FSM_DEAD2))
			{
				PlayerInfo[m_ePlayerNum].pController->Movement(fTimeDelta, PlayerInfo[m_ePlayerNum].vKnockbackDir, PlayerInfo[m_ePlayerNum].fKnockback);
				PlayerInfo[m_ePlayerNum].fKnockback -= fTimeDelta * 2.f;

				/*PlayerInfo[m_ePlayerNum].pTransform->Reset_Look_At_Direction();
				PlayerInfo[m_ePlayerNum].pTransform->Look_At_Direction(XMLoadFloat4(&m_vKnockbackDir));*/
			}
		}
		else if (m_bCombatMode == TRUE)
		{
			for (auto& pPart : m_Parts)
			{
				for (_int iSlot = 0; iSlot < MAX_PARTYSLOT; ++iSlot)
				{
					if (pPart.pPartySlot[iSlot] != nullptr)
						pPart.pPartySlot[iSlot]->LateTick(fTimeDelta);
				}
			}

			for (auto& Info : PlayerInfo)
			{
				if (Info.pController->Get_InAir() == FALSE)
					Info.fKnockback = 0.f;

				if (Info.fKnockback > 0.f && (Info.pFiniteStateMachineCom->Get_CurrentState() != FSM_DEAD1 || Info.pFiniteStateMachineCom->Get_CurrentState() != FSM_DEAD2))
				{
					Info.pController->Movement(fTimeDelta, Info.vKnockbackDir, Info.fKnockback);
					Info.fKnockback -= fTimeDelta * 2.f;
				}
			}
		}

		m_pElement_OHD->LateTick(fTimeDelta);
		//for (auto pHp : m_pHp_Bar)
		//{
		//	pHp->LateTick(fTimeDelta);
		//}
		//m_pStamina->LateTick(fTimeDelta);


		////TEST
		// m_pOverHeadUI->LateTick(fTimeDelta);



		//m_pInteractionBox->LateTick(fTimeDelta);
	}

	return S_OK;
}

HRESULT CPlayer::Render()
{
	return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom2P);
	Safe_Release(m_pTransformCom3P);
	Safe_Release(m_pTransformCom4P);
	Safe_Release(m_pFiniteStateMachineCom2P);
	Safe_Release(m_pFiniteStateMachineCom3P);
	Safe_Release(m_pFiniteStateMachineCom4P);

	for (auto& pPart : m_Parts)
	{
		for (_int iSlot = 0; iSlot < PLAYER_END; ++iSlot)
		{
			pPart.pPartySlot[iSlot]->Release_ColliderID();
			Safe_Release(pPart.pPartySlot[iSlot]);
		}
	}
	m_Parts.clear();

	if (m_bCloned == true)
	{
		Safe_Release(m_pPageMain);
		Safe_Release(m_pElement_OHD);
	}

	for (auto& pController : PlayerInfo)
	{
		Safe_Release(pController.pController);
	}

	PlayerInfo.clear();
	m_pGameInstance->Unregister_Interaction(L"BattleBGM");
}
