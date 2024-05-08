#include "stdafx.h"
#include "Creature.h"

#include "Ground_Item.h"
#include "PartObject.h"
#include "FiniteStateMachine.h"

#include "Part_Body.h"
#include "CharacterController.h"
#include "SkillObj.h"

#include "GameInstance.h"


CCreature::CCreature(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CDeciderObject(pDevice, pContext)
{
}

CCreature::CCreature(const CCreature& rhs)
    : CDeciderObject(rhs)
{
}

HRESULT CCreature::Add_Component(void* pArg)
{
	CREATURE_DESC* pDesc = static_cast<CREATURE_DESC*>(pArg);

	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_FiniteStateMachine */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_FiniteStateMachine"),
		TEXT("Com_FiniteStateMachine"), (CComponent**)&m_pFiniteStateMachineCom)))
		return E_FAIL;

	/* Bind Init States */
	m_pFiniteStateMachineCom->Bind_IdleState(FSM_IDLE);

	return S_OK;
}

HRESULT CCreature::Initialize_Controller(_float fHeight, _float fRadius, _float fStepOffset, CTransform* pTransformCom)
{
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	_float4 vIdentityPos = _float4(0.0f, 0.0f, 0.0f, 1.0f);

	m_pController = CCharacterController::Create(fHeight, fRadius, fStepOffset, vIdentityPos, m_pTransformCom);

	if (nullptr == m_pController)
		return E_FAIL;

	_float4 vPosition;
	XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pController->Set_Position(vPosition);

	return S_OK;
}

HRESULT CCreature::Drop_Item(_int iItemIndex, _int ItemQuantity)
{
	CGround_Item::DROP_ITEM_DESC	titemdesc = { };
	titemdesc.iItemIndex = iItemIndex;
	titemdesc.iItemQuantity = ItemQuantity;
	titemdesc.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Ground_Item"), L"Layer_Item", &titemdesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCreature::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCreature::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	m_eRenderType = RENDER_TYPE::RTYPE_GAMEPLAY;

    return S_OK;
}

HRESULT CCreature::PreviousTick(_float fTimeDelta)
{
    return S_OK;
}

HRESULT CCreature::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		/* Compare Look */
		m_pTransformCom->Verify_Look_At_Direction();
		m_pTransformCom->Invalidate_Look_At_Lerp(fTimeDelta);

		if (m_pGameInstance->IsProgress_StatusEffect(SE_ELECTRO_CHARGE, m_strStatusTag) == TRUE) /* 감전 */
		{
			m_fElectroChargedTime += fTimeDelta;
			if (m_fElectroChargedTime >= 1.f)
			{
				m_fSelfDamage = m_pGameInstance->Get_Float_Random(412.f, 734.f);
				m_fElectroChargedTime = 0.f;
				CSkillObj::SKILLOBJ_DESC tDesc = {};
				tDesc.eElement = ELEMENT_ELECTRIC;
				tDesc.eReaction = REACT_ELECTROCHARGED;
				tDesc.fDamage = m_pGameInstance->Get_Float_Random(412.f, 734.f);
				tDesc.vSpawnPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

				if (m_eCreatureID >= CHARACTER_BARBARA && m_eCreatureID <= CHARACTER_ZHONGLI)
					tDesc.eCollider = COLL_MONSTER_SKILL;
				else
					tDesc.eCollider = COLL_PLAYER_SKILL;

				if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Skill_ElementalReaction", L"Layer_Effect", &tDesc)))
					return E_FAIL;
			}
		}
		else
			m_fElectroChargedTime = 0.f;
	}

    return S_OK;
}

HRESULT CCreature::LateTick(_float fTimeDelta)
{
    return S_OK;
}

HRESULT CCreature::Render()
{
    return S_OK;
}

void CCreature::Create_DamageFont(_float fDamage, ELEMENT eElement, _fvector vPos, ELEMENT eImmuneElement)
{
	if (fDamage == 0 && eImmuneElement == ELEMENT_END)
		return;

	_vector vColor = {};

	/*
	Fire
	{1.f,0.65882352941.f,0.43921568627.f,1.f}

	Frost
	{ 0.80392156862f,1.f,1.f,1.f}

	Wind
	{0.65098039215f,95686274509.f,0.8f,1.f}

	Lightning
	{0.87058823529f,0.7294117647 ,1.f,1.f}

	Earth
	{244,214,96}

	Water
	{8, 288, 255}

	Nature
	{177,235,41}

	*/

	switch (eElement)
	{
	case Client::ELEMENT_FIRE:
		vColor = { 1.f,.65882352941f,.43921568627f,1.f };
		break;
	case Client::ELEMENT_WIND:
		vColor = { .65098039215f,.95686274509f,.8f,1.f };
		break;
	case Client::ELEMENT_ELECTRIC:
		vColor = { .87058823529f,.7294117647f,1.f,1.f };
		break;
	case Client::ELEMENT_WATER:
		vColor = { 0.80392156862f,1.f,1.f,1.f };
		break;
	case Client::ELEMENT_EARTH:
		vColor = { 1.f, 0.7804f, 0.440f, 1.f };
		break;
	case Client::ELEMENT_FROST:
		vColor = { 0.80392156862f, 1.f, 1.f, 1.f };
		break;		
	default:
		vColor = { 1.f, 1.f, 1.f, 1.f };
		break;
	}


	CDamage_Font::DMGFONT_DESC tDamageDesc = {};
	if(eImmuneElement != ELEMENT_END)
		tDamageDesc.strAilments = TEXT("면역");
	else
		tDamageDesc.iDamage = (_int)fDamage;
	tDamageDesc.vColor = vColor;

	if (vPos.m128_f32[0] == 0.f && vPos.m128_f32[1] == 0.f && vPos.m128_f32[2] == 0.f)
		tDamageDesc.vWorldPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	else
		tDamageDesc.vWorldPos = vPos;

	CDamage_Font::Create(&tDamageDesc);

	if (eElement == ELEMENT_ELECTRIC)
	{
		CSkillObj::SKILLOBJ_DESC tLightningDesc = {};
		tLightningDesc.vScale = XMVectorSet(0.8f, 1.8f, 0.8f, 0.f);
		tLightningDesc.vSpawnPos = tDamageDesc.vWorldPos;
		tLightningDesc.vSpawnPos.m128_f32[1] += 0.2f;
		tLightningDesc.fLifeTime = 0.3f;

		tLightningDesc.vDir = XMVectorSet(1.f, 0.f, 0.f, 0.f);
		if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Lightning", L"Layer_Effect", &tLightningDesc)))
			return;

		tLightningDesc.vDir = XMVectorSet(-1.f, 0.f, 0.f, 0.f);
		if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Lightning", L"Layer_Effect", &tLightningDesc)))
			return;

		tLightningDesc.vDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);
		if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Lightning", L"Layer_Effect", &tLightningDesc)))
			return;

		tLightningDesc.vDir = XMVectorSet(0.f, 0.f, -1.f, 0.f);
		if (FAILED(m_pGameInstance->Add_Object_Layer(m_pGameInstance->Get_CurrentLevel(), L"Prototype_GameObject_Effect_Lightning", L"Layer_Effect", &tLightningDesc)))
			return;
	}
}

void CCreature::Create_ReactionFont(ELEMENTAL_REACTION eReaction, _fvector vPos, ELEMENTAL_REACTION eImmuneReaction)
{
	if (eReaction == eImmuneReaction)
		return;

	CDamage_Font::DMGFONT_DESC tDamageDesc = {};

	switch (eReaction)
	{
	case Client::REACT_VAPORIZE:
	case Client::REACT_VAPORIZE_REVERSE:
		tDamageDesc.strAilments = TEXT("증발");
		tDamageDesc.vColor = { 1.f,.65882352941f,.43921568627f,1.f };
		break;
	case Client::REACT_MELT:
	case Client::REACT_MELT_REVERSE:
		tDamageDesc.strAilments = TEXT("융해");
		tDamageDesc.vColor = { 1.f,.65882352941f,.43921568627f,1.f };
		break;
	case Client::REACT_OVERLOADED:
		tDamageDesc.strAilments = TEXT("과부하");
		tDamageDesc.vColor = { 0.87058823529f, 0.7294117647f, 1.f, 1.f };
		break;
	case Client::REACT_SWIRL_FIRE:
	case Client::REACT_SWIRL_WATER:
	case Client::REACT_SWIRL_ELECTRIC:
	case Client::REACT_SWIRL_FROST:
		tDamageDesc.strAilments = TEXT("확산");
		tDamageDesc.vColor = { .65098039215f,.95686274509f,.8f,1.f };
		break;
	case Client::REACT_SUPERCONDUCT:
		tDamageDesc.strAilments = TEXT("초전도");
		tDamageDesc.vColor = { .87058823529f,.7294117647f,1.f,1.f };
		break;
	case Client::REACT_ELECTROCHARGED:
		tDamageDesc.strAilments = TEXT("감전");
		tDamageDesc.vColor = { .87058823529f,.7294117647f,1.f,1.f };
		break;
	case Client::REACT_FROZEN:
		tDamageDesc.strAilments = TEXT("빙결");
		tDamageDesc.vColor = { 0.80392156862f, 1.f, 1.f, 1.f };
		break;
	case Client::REACT_CRYSTALLIZE_FIRE:
	case Client::REACT_CRYSTALLIZE_WATER:
	case Client::REACT_CRYSTALLIZE_ELECTIRC:
	case Client::REACT_CRYSTALLIZE_FROST:
		tDamageDesc.strAilments = TEXT("결정");
		tDamageDesc.vColor = { 1.f, 0.7804f, 0.440f, 1.f };
		break;
	default:
		return;
	}
	
	if (vPos.m128_f32[0] == 0.f && vPos.m128_f32[1] == 0.f && vPos.m128_f32[2] == 0.f)
		tDamageDesc.vWorldPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	else
		tDamageDesc.vWorldPos = vPos;

	CDamage_Font::Create(&tDamageDesc);
}

void CCreature::Create_HealFont(_float fHeal, _fvector vPos)
{
	CDamage_Font::DMGFONT_DESC tDamageDesc = {};
	tDamageDesc.iDamage = (_int)fHeal;
	tDamageDesc.vColor = { 0.2f, 0.9f, 0.2f,1.f };

	if (vPos.m128_f32[0] == 0.f && vPos.m128_f32[1] == 0.f && vPos.m128_f32[2] == 0.f)
		tDamageDesc.vWorldPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	else
		tDamageDesc.vWorldPos = vPos;

	CDamage_Font::Create(&tDamageDesc);
}

void CCreature::Free()
{
	__super::Free();
	
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pFiniteStateMachineCom);

	Safe_Release(m_pController);
}
