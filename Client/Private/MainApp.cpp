#include "stdafx.h"
#include "MainApp.h"
#include "Level_Loading.h"

#pragma region AssetFX

#include "FX_Tornado.h"
#include "FX_Meteorite.h"
#include "FX_ShockWave.h"
#include "FX_CircularIndicator.h"
#include "FX_BarbaraES.h"
#include "FX_RaidenES.h"
#include "FX_EffigyFireAura.h"
#include "FX_EffigyFireExplode.h"
#include "FX_EffigyFireImpact.h"
#include "FX_EffigyFirePillar.h"
#include "FX_EffigyFireProjectile.h"
#include "FX_EffigyFireTrail.h"
#include "FX_EffigyFire_MagicCircle.h"
#include "FX_EffigyFireTrsCow.h"
#include "FX_EffigyFireTrsFish.h"
#include "FX_EffigyFireTrsBomb.h"
#include "FX_EffigyFireClap.h"
#include "FX_AndriusSavageTornado.h"
#include "FX_AndriusMeteorite.h"

#pragma endregion

#include "Prototype_include.h"

#include "Elemental_Manager.h"

#include "Mouse.h"
#include "Collider.h"
#include "GameInstance.h"

#include "DebugCheat.h"
#ifdef _DEBUG
#include "Camera_Main.h"
#endif

#ifndef _DEBUGNUI
#include "DebugTool.h"
#endif

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Open_Level(LEVELID eLevel)
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;

	if (eLevel >= LEVEL_LOADING)
		return E_FAIL;

	CLevel* pLevel = CLevel_Loading::Create(m_pDevice, m_pContext, eLevel);
	if (pLevel == nullptr)
	{
		MSG_BOX(TEXT("Failed to Create : CLevel_Loading"));
		return E_FAIL;
	}

	return m_pGameInstance->Open_Level(LEVEL_LOADING, pLevel);
}

HRESULT CMainApp::Ready_Prototype()
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;

	if (FAILED(CMainApp::Ready_Prototype_GameObject()))
		return E_FAIL;

	if (FAILED(CMainApp::Ready_Prototype_Component_For_Static()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Json()
{
	/* Item Info */
	if (FAILED(m_pGameInstance->Load_JsonFile(L"Item_Info", "../../Reference/Resources/Data/Json/Item_Info.json")))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(Ready_Prototype_Player()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Camera()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Monster()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Effect()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Skill()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_NPC()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_UI()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Interaction()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Map()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_PartObject_Body()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_PartObject_Weapon()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_PartObject_Glider()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_PartObject_Status()))
		return E_FAIL;

	/* Component : Model_Boss_Chest */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Boss_Chest"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Dungeon/Boss_Chest/Boss_Chest.dat"))))
		return E_FAIL;

	/* GameObject : Boss_Chest */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Boss_Chest"),
		CBossChest::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : DebugCheat */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_DebugCheat"),
		CDebugCheat::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_DebugCheat"), TEXT("Layer_Debug"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Player()
{
	/* GameObject : Player */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Camera()
{
	/* GameObject : Camera */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Camera"),
		CCamera_Main::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : CameraChar */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_CameraChar"),
		CCamera_Char::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Monster()
{
	/* GameObject : Slime_Small */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Slime_Small"),
		CSlime_Small::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Slime_Ice */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Slime_Big"),
		CSlime_Big::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Hili_Normal */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Hili_Normal"),
		CHili_Normal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Brute */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Brute"),
		CBrute::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Andrius */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Andrius"),
		CAndrius::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Effigy_Fire */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Effigy_Fire"),
		CEffigy_Fire::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Dvalin */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Dvalin"),
		CDvalin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Abyss_Mage */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Abyss_Mage"),
		CAbyss_Mage::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Effect()
{
	/* GameObject : Effect_Flame */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Effect_Flame"),
		CEffect_Flame::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Effect_Hitting */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Effect_Hitting"),
		CEffect_Hitting::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Effect_Rush_Circle */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Effect_Rush_Circle"),
		CEffect_Rush_Circle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Effect_Ice_Floor */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Effect_Ice_Floor"),
		CEffect_Ice_Floor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Effect_Fog */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Effect_Fog"),
		CEffect_Fog::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Effect_Flame */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Effect_Trail_Flame"),
		CFlame0::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Effect_Flame */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Effect_Flame1"),
		CEff_Flame1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Effect_Mesh */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Effect_FlameSlash_Trail"),
		CFlameSlash_Trail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Item_Glow */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Effect_Item_Glow"),
		CItem_Glow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Impact_Glow */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Effect_Impact_Glow"),
		CImpact_Glow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Flame_Enchant */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Effect_Squence"),
		CEff_Squence::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Flame_Enchant_Cross*/
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Effect_Flame_Enchant_Cross"),
		CEff_Flame_Enchant_Cross::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Eff_Wind_Field*/
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Effect_Wind_Field"),
		CEff_WindField::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Eff_Barbara_Normal_Star*/
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Effect_Barbara_Normal_Star"),
		CEff_Barbara_Normal_Star::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Eff_Water_Fog */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Effect_Water_Fog"),
		CWater_Fog::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Eff_Barbara_Burst_Circle*/
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Effect_Barbara_Burst_Circle"),
		CBarbara_Burst_Circle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Eff_TEST_CLASS */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Effect_TEST_CLASS"),
		CEff_TEST_CLASS::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Pillar_Wave */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Eff_Pillar_Wave"),
		CPillar_Wave::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Piercing_Trail */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Eff_Piercing_Trail"),
		CPiercing_Trail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : FallingSpearTrail */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Eff_FallingSpearTrail"),
		CFallingSpearTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Eff_Trasition */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Eff_Transition"),
		CEff_Transition::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Eff_Ground_Stomp */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Eff_Ground_Stomp"),
		CEff_GroundStomp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Effect_Barbara_Normal_Atk */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Effect_Barbara_Normal_Atk"),
		CEff_Barbara_Normal_Atk::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Effect_Liquid_Splash */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Effect_Liquid_Splash"),
		CEff_Liquid_Splash::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Eff_Smoke */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Eff_Smoke"),
		CEff_Smoke::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Eff_Cross */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Eff_Cross"),
		CEff_Cross::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Eff_Effigy_Recharge_Trail */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Eff_Effigy_Recharge_Trail"),
		CEff_Effigy_Recharge_Trail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : LineTrap */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Eff_LineTrap"),
		CLineTrap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : ChainLightning */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Eff_ChainLightning"),
		CChainLightning::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Eff_Door_Portal */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Eff_Door_Portal"),
		CDoor_Portal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Eff_Door_Portal */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Eff_Activate"),
		CEff_Activate::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Skill()
{
	/* GameObject : Skill_Pillar */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Skill_Pillar"),
		CSkill_Pillar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Skill_Zhongli_Shield */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Skill_Zhongli_Shield"),
		CSkill_Zhongli_Shield::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Skill_Zhongli_Shield */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Skill_Abyss_Mage_Shield"),
		CSkill_Abyss_Mage_Shield::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Skill_FallingStone */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Skill_FallingStone"),
		CSkill_FallingStone::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Skill_FallingStoneTrail */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Skill_FallingStoneTrail"),
		CSkill_FallingStoneTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Skill_FallingSpear */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Skill_FallingSpear"),
		CSkill_FallingSpear::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Skill_BurstBackGround */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Skill_BurstBackGround"),
		CSkill_BurstBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Skill_Phoenix */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Skill_Phoenix"),
		CSkill_Phoenix::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Skill_IceCone */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Skill_IceCone"),
		CSkill_IceCone::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Skill_IceShockWave */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Skill_IceShockWave"),
		CSkill_IceShockWave::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Skill_IcePillar */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Skill_IcePillar"),
		CSkill_IcePillar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Skill_WindBlade */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Skill_WindBlade"),
		CSkill_WindBlade::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Skill_WindRange */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Skill_WindRange"),
		CSkill_WindRange::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Skill_Rush */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Skill_Rush"),
		CSkill_Rush::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Skill_Rush2 */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Skill_Rush2"),
		CSkill_Rush2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Skill_Claw */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Skill_Claw"),
		CSkill_Claw::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Skill_FireCube */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Skill_FireCube"),
		CSkill_FireCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Skill_FirePillar */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Skill_FirePillar"),
		CSkill_FirePillar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Skill_FireCore */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Skill_FireCore"),
		CSkill_FireCore::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Prototype_GameObject_Zhongli_Sp_Explosion */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Zhongli_Sp_Explosion"),
		CZhongli_SpExplod::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Prototype_GameObject_Skill_Element_Shield */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Skill_Element_Shield"),
		CElement_Shield::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Prototype_GameObject_Skill_ElementalReaction */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Skill_ElementalReaction"),
		CElementalReaction::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Prototype_GameObject_Skill_BarbaraBasicAttack */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Skill_BabaraBasicAttack"),
		CBabaraBasicAttack::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Prototype_GameObject_Skill_BarbaraBasicAttack */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Skill_RaidenElementAttack"),
		CRaiden_ElementAttack::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Effect_Mesh */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Effect_Slash_Trail"),
		CSlash_Trail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Effect_Raiden_Trail_Add */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Effect_Raiden_Trail_Add"),
		CRaiden_Trail_Add::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Prototype_GameObject_Big_Explosion */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Skill_BigExplosion"),
		CSKill_BigExplosion::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Prototype_GameObject_Skill_Chakra */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Skill_Chakra"),
		CSkill_Chakra::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Prototype_GameObject_Skill_Symbol */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Skill_Symbol"),
		CRaiden_Symbol::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Prototype_GameObject_Skill_Light */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Effect_Lightning"),
		CLightning::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Prototype_GameObject_Effect_Shougun_Trail */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Effect_Shougun_Trail"),
		CShougun_Trail_TEST::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_NPC()
{
	/* GameObject : Paimon */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Paimon"),
		CPaimon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : AndriusSword */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_AndriusSword"),
		CAndriusSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Katheryne */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Temp_NPC"),
		CKatheryne::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Ambor */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Ambor"),
		CAmbor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : CommonNPC */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_CommonNPC"),
		CCommon_NPC::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Swan */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Swan"),
		CSwan::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : TelePort */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_TelePort"),
		CTelePort::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_UI()
{
	/* GameObject : Rect */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Rect"),
		CRect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : AlphaRect */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_AlphaRect"),
		CAlphaRect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : LoadingRect */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_LoadingRect"),
		CLoadingRect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : UI_Decor */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Decor"),
		CDecor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Mannequin */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Mannequin"),
		CMannequin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : UI_Manager */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_UI_Manager"),
		CUI_Manager::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Quest_Manager */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Quest_Manager"),
		CQuest_Manager::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Button */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Button"),
		CButton::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Button_Alpha */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Button_Alpha"),
		CButtonAlpha::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : UI_Dual_Tex */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_UI_Dual_Tex"),
		CUI_DualTex::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Interaction_Box */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Interaction_Box"),
		CInteraction_Box::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Select_Box */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Select_Box"),
		CSelRect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Page_Main */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Page_Main"),
		CPage_Main::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : CParty_Buttons */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Party_Buttons"),
		CParty_Buttons::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Hp_Bar */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Ui_Hp_Bar"),
		CHp_Bar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Stamina_Bar */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Stamina_Bar"),
		CStamina_Bar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Paimon_Page */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Paimon_Page"),
		CPaimon_Page::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Gacha_Page */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Gacha_Page"),
		CGacha_Page::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Shop_Page */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Shop_Page"),
		CShop_Page::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Alchemy_Page */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Alchemy_Page"),
		CAlchemy_Page::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Cooking_Page */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Cooking_Page"),
		CCooking_Page::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* GameObject : Gacha_Result_Page */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Gacha_Result_Page"),
		CGacha_Result_Page::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Prototype_GameObject_Dialogue_Page */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Dialogue_Page"),
		CDialogue_Page::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Prototype_GameObject_Quest_Page */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Quest_Page"),
		CQuest_Page::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Element_OHD */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Element_OHD"),
		CElement_OHD::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Inventory */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Inventory"),
		CInventory::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Item_Weapon */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Item_Weapon"),
		CItem_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Item_Artifact */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Item_Artifact"),
		CItem_Artifact::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Item_Material */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Item_Material"),
		CItem_Material::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Item_Consumable */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Item_Consumable"),
		CItem_Consumable::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Ground_Item */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Ground_Item"),
		CGround_Item::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Interaction()
{
	/* GameObject : SpawnBox */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_SpawnBox"),
		CSpawnBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Updraft */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Updraft"),
		CUpdraft::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Gliding_Ring */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Gliding_Ring"),
		CGliding_Ring::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Accelerating_Ring */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Accelerating_Ring"),
		CAccelerating_Ring::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Gate */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Gate"),
		CGate::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Location */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Location"),
		CLocation::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Map()
{
	/* GameObject : World */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_World"),
		CWorld::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : SkyBox */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_SkyBox"),
		CSkyBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Dungeon */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Dungeon"),
		CDungeon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_PartObject_Body()
{
#pragma region PartObject : Body Character

	/* GameObject : Body_Traveler */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Body_Traveler"),
		CBody_Traveler::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Body_Barbara */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Body_Barbara"),
		CBody_Barbara::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Body_Diluc */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Body_Diluc"),
		CBody_Diluc::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Body_Ganyu */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Body_Raiden"),
		CBody_Raiden::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Body_Zhongli */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Body_Zhongli"),
		CBody_Zhongli::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

#pragma region PartObject : Body NPC

	/* GameObject : Body_Paimon */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Body_Paimon"),
		CBody_Paimon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Body_Paimon */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Body_Katheryne"),
		CBody_Katheryne::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Body_Ambor */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Body_Ambor"),
		CBody_Ambor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Body_NPCMale */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCMale"),
		CBody_NPCMale::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Body_NPCFeMale */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Body_NPCFeMale"),
		CBody_NPCFeMale::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Body_Swan */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Body_Swan"),
		CBody_Swan::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Body_TelePort */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Body_TelePort"),
		CBody_TelePort::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

#pragma region PartObject : Body Monster

	/* GameObject : Body_Andrius */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Body_Andrius"),
		CBody_Andrius::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Body_Slime_Water */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Body_Slime_Water"),
		CBody_Slime_Water::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Body_Slime_Ice */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Body_Slime_Ice"),
		CBody_Slime_Ice::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Body_Hili_Normal */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Body_Hili_Normal"),
		CBody_Hili_Normal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Body_Brute */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Body_Brute"),
		CBody_Brute::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Body_Effigy_Fire */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Body_Effigy_Fire"),
		CBody_Effigy_Fire::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Body_Dvalin */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Body_Dvalin"),
		CBody_Dvalin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Body_Abyss_Mage */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Body_Abyss_Mage"),
		CBody_Abyss_Mage::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_PartObject_Weapon()
{
	/* GameObject : Claymore_Wolfmound */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Claymore_Wolfmound"),
		CClaymore_Wolfmound::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Pole_Kunwu */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Pole_Kunwu"),
		CPole_Kunwu::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Catalyst_Zephyrus */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Catalyst_Zephyrus"),
		CCatalyst_Zephyrus::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Pole_Narukami */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Pole_Narukami"),
		CPole_Narukami::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Slime_IceCone */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Slime_IceCone"),
		CSlime_IceCone::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Hili_Club */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Hili_Club"),
		CHili_Club::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Brute_Shield */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Brute_Shield"),
		CBrute_Shield::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Brute_Axe */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Brute_Axe"),
		CBrute_Axe::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Staff_Elec */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Staff_Elec"),
		CStaff_Elec::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Staff_Ice */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Staff_Ice"),
		CStaff_Ice::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Staff_Water */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Staff_Water"),
		CStaff_Water::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_PartObject_Glider()
{
	/* GameObject : Glider_Origin */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Glider_Origin"),
		CGlider_Origin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Glider_Liyue */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Glider_Liyue"),
		CGlider_Liyue::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Glider_Psalmus */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Glider_Psalmus"),
		CGlider_Psalmus::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Glider_Thunderclap */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Glider_Thunderclap"),
		CGlider_Thunderclap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_PartObject_Status()
{
	/* GameObject : Status_Diluc */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Status_Diluc"),
		CStatus_Diluc::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Status_Zhongli */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Status_Zhongli"),
		CStatus_Zhongli::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Status_Barbara */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Status_Barbara"),
		CStatus_Barbara::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Status_Raiden */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Status_Raiden"),
		CStatus_Raiden::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Status_Andrius */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Status_Andrius"),
		CStatus_Andrius::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* GameObject : Status_Dvalin */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Status_Dvalin"),
		CStatus_Dvalin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component_For_Static()
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;

	if (FAILED(Ready_ProtoType_Component_Basic()))
		return E_FAIL;

	if (FAILED(Ready_ProtoType_Component_Shader()))
		return E_FAIL;

	if (FAILED(Ready_ProtoType_Component_VIBuffer()))
		return E_FAIL;

	if (FAILED(Ready_ProtoType_Component_Texture_UI()))
		return E_FAIL;

	if (FAILED(Ready_ProtoType_Component_Texture_Effect()))
		return E_FAIL;

	if (FAILED(Ready_ProtoType_Component_Texture_Map()))
		return E_FAIL;

	if (FAILED(Ready_ProtoType_Component_Texture_Asset()))
		return E_FAIL;

	if (FAILED(Ready_ProtoType_Component_Model_Character()))
		return E_FAIL;

	if (FAILED(Ready_ProtoType_Component_Model_Weapon()))
		return E_FAIL;

	if (FAILED(Ready_ProtoType_Component_Model_Glider()))
		return E_FAIL;

	if (FAILED(Ready_ProtoType_Component_Model_Monster()))
		return E_FAIL;

	if (FAILED(Ready_ProtoType_Component_Model_NPC()))
		return E_FAIL;

	if (FAILED(Ready_ProtoType_Component_Model_Skill()))
		return E_FAIL;

	if (FAILED(Ready_ProtoType_Component_Model_Effect()))
		return E_FAIL;

	if (FAILED(Ready_ProtoType_Component_Model_Interaction()))
		return E_FAIL;

	if (FAILED(Loading_WorldModel(LEVEL_STATIC, TEXT("../../Reference/Resources/Model/Map/*.*"))))
		return E_FAIL;

#pragma region Face Shadow

	/* Component : Texture_FaceShadow */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FaceShadow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/FaceShadow/FaceShadow_%d.png"), 5))))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CMainApp::Ready_ProtoType_Component_Basic()
{
	/* Component : Renderer */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	Safe_AddRef(m_pRenderer);

	/* Component : Transform */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::CTYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::CTYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_SPHERE */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::CTYPE_SPHERE))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_FRUSTUM */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_FRUSTUM"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::CTYPE_FRUSTUM))))
		return E_FAIL;

	/* Component : FiniteStateMachine */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_FiniteStateMachine"),
		CFiniteStateMachine::Create(m_pDevice, m_pContext, FSM_END))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_ProtoType_Component_Shader()
{
	/* Component : Shader_Vtx */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Vtx"),
		CShader::Create(m_pDevice, m_pContext, VTXPOS::VertexElements, VTXPOS::iNumElements,
			TEXT("../../Reference/ShaderFiles/Shader_Vtx.hlsl")))))
		return E_FAIL;

	/* Component : Shader_VtxTex */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, VTXPOSTEX::VertexElements, VTXPOSTEX::iNumElements,
			TEXT("../../Reference/ShaderFiles/Shader_VtxTex.hlsl")))))
		return E_FAIL;

	/* Component : Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, VTXNORTEX::VertexElements, VTXNORTEX::iNumElements,
			TEXT("../../Reference/ShaderFiles/Shader_VtxNorTex.hlsl")))))
		return E_FAIL;

	/* Component : Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, VTXMESH::VertexElements, VTXMESH::iNumElements,
			TEXT("../../Reference/ShaderFiles/Shader_VtxMesh.hlsl")))))
		return E_FAIL;

	/* Component : Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, VTXANIMMESH::VertexElements, VTXANIMMESH::iNumElements,
			TEXT("../../Reference/ShaderFiles/Shader_VtxAnimMesh.hlsl")))))
		return E_FAIL;

	/* Component : Shader_Trail */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Trail"),
		CShader::Create(m_pDevice, m_pContext, VTXPOSTEX::VertexElements, VTXPOSTEX::iNumElements,
			TEXT("../../Reference/ShaderFiles/Shader_Trail.hlsl")))))
		return E_FAIL;

	/* Component : Shader_UI */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		CShader::Create(m_pDevice, m_pContext, VTXPOSTEX::VertexElements, VTXPOSTEX::iNumElements,
			TEXT("../../Reference/ShaderFiles/Shader_UI.hlsl")))))
		return E_FAIL;

	/* Component : Shader_Effect */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffect"),
		CShader::Create(m_pDevice, m_pContext, VTXEFFECT::VertexElements, VTXEFFECT::iNumElements,
			TEXT("../../Reference/ShaderFiles/Shader_VtxEffect.hlsl")))))
		return E_FAIL;

	/* Component : Shader_SkyBox */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_SkyBox"),
		CShader::Create(m_pDevice, m_pContext, VTXCUBETEX::VertexElements, VTXCUBETEX::iNumElements,
			TEXT("../../Reference/ShaderFiles/Shader_SkyBox.hlsl")))))
		return E_FAIL;

	/* Component : Shader_Instancing */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Instancing"),
		CShader::Create(m_pDevice, m_pContext, VTXMESH_INSTANCE::VertexElements, VTXMESH_INSTANCE::iNumElements,
			TEXT("../../Reference/ShaderFiles/Shader_Instancing.hlsl")))))
		return E_FAIL;

	/* Component : Shader_TexFX (Asset) */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_TexFX"),
		CShader::Create(m_pDevice, m_pContext, VTXPOSTEX::VertexElements, VTXPOSTEX::iNumElements,
			TEXT("../../Reference/ShaderFiles/Shader_TexFX.hlsl")))))
		return E_FAIL;

	/* Component : Shader_MeshFX (Asset) */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MeshFX"),
		CShader::Create(m_pDevice, m_pContext, VTXMESH::VertexElements, VTXMESH::iNumElements,
			TEXT("../../Reference/ShaderFiles/Shader_MeshFX.hlsl")))))
		return E_FAIL;

	/* Component : Shader_AnimFX (Asset) */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimFX"),
		CShader::Create(m_pDevice, m_pContext, VTXANIMMESH::VertexElements, VTXANIMMESH::iNumElements,
			TEXT("../../Reference/ShaderFiles/Shader_AnimFX.hlsl")))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_ProtoType_Component_VIBuffer()
{
	/* Component : VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Component : VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Component : VIBuffer_Terrain World*/
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain_World"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Data/Map/WorldHeightMap.dat")))))
		return E_FAIL;

	/* Component : VIBuffer_Trail */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail"),
		CVIBuffer_Trail::Create(m_pDevice, m_pContext, 20))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_ProtoType_Component_Texture_UI()
{
	/* For. Prototype_Component_Texture_Main_Icons */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Main_Icons"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/World_UI/Icons/MainIcon%d.dds"), 10))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_Main_No_Responce */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Main_No_Responce"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/World_UI/NoResponse/Main_NoResponce%d.dds"), 7))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_HPBar */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_HPBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/World_UI/UI_HPBar_%d.dds"), 2))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_Stamina_Bar */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Stamina_Bar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/World_UI/Stamina/UI_Stamina_Bar%d.dds"), 4))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_HPBar */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Mini_Map_Boundary"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/World_UI/Mini_Map/Mini_Map.dds")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_Select_Box */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Select_Box"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/World_UI/Select_Box%d.dds"),5))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Gacha_Bg */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Gacha_Bg"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/Gacha_UI/Gacha_Bg0.dds")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Gacha_Banner */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Gacha_Banner"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/Gacha_UI/Banner_Temp%d.dds"), 2))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Gacha_Result_Bg */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Gacha_Result_Bg"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/Gacha_UI/Gacha_Result_Bg.dds")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Gacha_Result_Base */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Gacha_Result_Base"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/Gacha_UI/Result/Result_Icon_Base%d.dds"), 2))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Gacha_Avatar_Bg */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Gacha_Avatar_Bg"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/Gacha_UI/Result/AvatarIcon/UI_Gacha_AvatarIcon%d.png"), 77))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_Dialogue_Objects */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Dialogue_Objects"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/Dialogue_UI/DialogueObject%d.dds"), 3))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_Interaction_Box */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Interaction_Box"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/Dialogue_UI/Interaction0.dds")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_Interaction_Icons */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Interaction_Icons"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/Dialogue_UI/Interaction_Icons.png")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_Element_OHD */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Element_OHD"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/Elem_OHD/Eff_Common_Elem_%d.png"), 3))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Inventory */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Inventory"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/Inventory_UI/Inventory_%d.dds"), 11))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Item_Rarity */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_Rarity"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/Item_UI/Item_Rarity/Item_Rarity_%d.png"), 6))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Item_Texture */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Item_Texture"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/Item_UI/Item_Texture/Item_Texture_%d.png"), 173))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_Paimon_Page */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Paimon_Page"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/Paimon_UI/Base_Textures/UI_Paimon%d.dds"), 5))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_Paimon_Portraits */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Paimon_Portraits"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/Paimon_UI/Portraits/Portrait%d.png"), 8))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_Paimon_Name_Card */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Paimon_Name_Card"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/Paimon_UI/NameCardPic_Used/UI_NameCardPic%d.png"), 9))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_Paimon_Icon */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Paimon_Icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/Paimon_UI/Paimon_Icons/Paimon_Icon%d.dds"), 19))))
		return E_FAIL;

	/* Component : Texture_Loading */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/Loading_UI/Loading_%d.png"), 4))))
		return E_FAIL;

	/* Component : Texture_UI_QuestIcon */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QuestIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/Quest_UI/Quest_Icon_1.png")))))
		return E_FAIL;

	/* Component : Texture_UI_QuickMenu */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_QuickMenu"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/QuickMenu_UI/QuickMenu_Icon.png")))))
		return E_FAIL;

	/* Component : Texture_UI_CharMenu_BackGround */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_CharMenu_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/PopUp_UI/PopUp_UI_BackGround1.png")))))
		return E_FAIL;

	/* Component : Texture_UI_CharMenu_Cloud */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_CharMenu_Cloud"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/PopUp_UI/Eff_Noise.png")))))
		return E_FAIL;

	/* Component : Texture_UI_CharMenu_At */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_CharMenu_At"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/PopUp_UI/UI_charMenu_3.png")))))
		return E_FAIL;

	/* Component : Texture_UI_Button */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Button"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/PopUp_UI/Icon_UI_RoundBox.png")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_Main_Party_Buttons */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Main_Party_Buttons"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/World_UI/PartyButtons/Party_Button%d.dds"), 10))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_Skill_Elemental */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Skill_Elemental"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/World_UI/PartyButtons/Skill_Icons/Elemental/UI_Elemental%d.png"), 7))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_Skill_Burst */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Skill_Burst"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/World_UI/PartyButtons/Skill_Icons/Burst/UI_Burst%d.png"), 7))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_AvatarIcon_Side */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_AvatarIcon_Side"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/World_UI/PartyButtons/Characters/UI_AvatarIcon_Side%d.dds"), 7))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_Char_Portraits */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Char_Portraits"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/Portraits/UI_AvatarIcon%d.png"), 4))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_Shop_Elements */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Shop_Elements"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/Shop_UI/Shop_%d.dds"), 10))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_Alchemy_Elements */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Alchemy_Elements"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/Alchemy_UI/Alchemy_%d.dds"),8))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_UI_Cooking_Elements */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Cooking_Elements"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/Cooking_UI/Cooking_%d.png"), 6))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_ProtoType_Component_Texture_Effect()
{
	/* Component : Texture_GrayFireTexture */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_GrayFireTexture"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/GrayFireTexture.png")))))
		return E_FAIL;

	/* Component : Texture_Trail */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Trail"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/TrailEffect_%d.png"), 5))))
		return E_FAIL;

	/* Component : Texture_Trail */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Trail_Andrius"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Eff_Trail_Scratch_02.png")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Dissolve */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dissolve"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Dissolve.png")))))
		return E_FAIL;

	/* For. Prototype_Component_Texture_Petrification */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Petrification"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Petrification.png")))))
		return E_FAIL;

	/* Component : Texture_Particle */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Particle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Particle_%d.png"), 28))))
		return E_FAIL;

	/* Component : Texture_Flame */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Flame"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Flame/%d.dds"), 4))))
		return E_FAIL;

	/* Component : Texture_Hitting */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Hitting"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Hitting/Effcet_Hitting%d.png"), 3))))
		return E_FAIL;

	/* Component : Texture_Rush_Circle */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Rush_Circle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Rush/Effcet_RushRing_0.png")))))
		return E_FAIL;

	/* Component : Texture_Ice_Floor */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Ice_Floor"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Ice_Floor/Ice_Floor_%d.png"), 2))))
		return E_FAIL;

	/* Component : Texture_Freezing */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Freezing"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Freezing/Freezing%d.png"), 2))))
		return E_FAIL;

	/* Component : Prototype_Component_Texture_Effect_WaterNoise */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_WaterNoise"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Water/Eff_Water_Ripple_01.png")))))
		return E_FAIL;

	/* Component : Prototype_Component_Texture_Effect_Crack */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Crack"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Cracks/Eff_Crack_%d.png"), 3))))
		return E_FAIL;

	/* Component : Texture_Ice_Floor */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Fog"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Fog/Fog.png")))))
		return E_FAIL;

	/* Component : Texture PillarWave */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Pillar_Wave"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Pillar/Pillar_Wave.png")))))
		return E_FAIL;

	/* Component : Texture PiercingTrail */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Piercing_Trail"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Piercing/Eff_Piercing%d.png"), 3))))
		return E_FAIL;

	/* Component : Texture Trasition */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_Transition"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Transition/Eff_Transition%d.png"), 2))))
		return E_FAIL;

	/* Component : Texture Burst_Background_Mask */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Burst_Background_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/SkillObj/Zhongli/Burst_BackGround/Eff_Mask_22_Clamp.png")))))
		return E_FAIL;

	/* Component : Texture Ground_Stomp */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_Ground_Stomp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/GroundShock/GroundStomp%d.png"), 2))))
		return E_FAIL;

	/* Component : Texture Flame_Trail */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_Flame_Trail"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Flame_Slash/Eff_FlameTrail%d.png"), 2))))
		return E_FAIL;

	/* Component : Texture Cross */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_Cross"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Eff_Sc_Blur_02.png")))))
		return E_FAIL;

	/* Component : Texture Wind_Field */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_WindField"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Effect/Enviroment/Eff_WindField_Mask%d.png"), 2))))
		return E_FAIL;

	/* Component : Texture_Fire_Ramp */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Fire_Ramp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/FireTest/Eff_Burn_Ramp.png")))))
		return E_FAIL;
	/* Component : Texture_Eff_Fire */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_Fire_Diff"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/FireTest/Eff_Fire_%d.png"), 6))))
		return E_FAIL;

	/* Component : Texture_Item_Glow */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_Item_Glow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Item_Glow/Item_Glow%d.png"),2))))
		return E_FAIL;

	/* Component : Texture_Impact_Glow */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_Impact_Glow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Glow/Eff_Glow_0.png")))))
		return E_FAIL;

	/* Component : Texture_Eff_Flame_Enchant */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_Flame_Enchant"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Flame_Enchant/Eff_Flame_Enchant%d.png"), 3))))
		return E_FAIL;

	/* Component : Texture_Meteor_Trail*/
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Meteor_Trail"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Meteor_Trail/Eff_Meteor_Trail%d.png"), 3))))
		return E_FAIL;

	/* Component : Texture_Squence*/
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Squence"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Squence/Eff_Squence%d.png"), 2))))
		return E_FAIL;

	/* Component : Texture_Eff_Water_Fog*/
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_Water_Fog"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Eff_Water_Fog/Eff_RainDrop_01.png")))))
		return E_FAIL;

	/* Component : Texture_Barbara_Color_Ramp*/
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Barbara_Color_Ramp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Barbara/Eff_Barbara_IdolHeal_Ramp.png")))))
		return E_FAIL;

	///* Component : Texture_Water_Bubble*/
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Water_Bubble"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Bubbles/Eff_Matcap_Liquid_Water_08.png")))))
	//	return E_FAIL;

	/* Component : Texture_Burst_Circle_Diffuse*/
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Burst_Circle_Diffuse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Barbara/Burst_Circle/Eff_Burst_Circle_Diffuse%d.png"), 2))))
		return E_FAIL;

	/* Component : Texture_Water_Turbulence*/
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Water_Turbulence"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Barbara/Burst_Circle/Eff_Turbulence%d.png"), 3))))
		return E_FAIL;

	/* Component : Texture Barbara_Glow_Star */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_Barbara_Glow_Star"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Barbara/Barbara_Glow_Star.png")))))
		return E_FAIL;

	/* Component : Texture Eff_Liquid_Splash */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_Liquid_Splash"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Splash/Eff_Liquid_Splash%d.png"),2))))
		return E_FAIL;
	
	/* Component : Texture Eff_Dimentional_Slash */
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_Dimentional_Slash"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Slash/Slash.png")))))
	//	return E_FAIL;
		
	/* Component : Texture Eff_Smoke */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Smoke"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Smoke/Eff_Smoke_%d.png"),3))))
		return E_FAIL;

	/* Component : Texture Eff_Dvalin_EvillTex */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_Dvalin_EvillTex"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Body/Monster/Dvalin/Monster_Dvalin_01_Evil_Tex_Eff.png")))))
		return E_FAIL;

	/* Component : Texture_Recharge_Trail */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Recharge_Trail"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Flame_Trail/Eff_Recharge_Trail%d.png"),2))))
		return E_FAIL;

	/* Component : LupiBoreas_LightMap */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LupiBoreas_LightMap"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Body/Monster/Andrius/Monster_LupiBoreas_01_Ice_Tex_Lightmap.png")))))
		return E_FAIL;
	
	/* Component : LupiBoreas_Fluid */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LupiBoreas_Fluid"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Body/Monster/Andrius/Eff_Normal_Fluid_01_Normal.png")))))
		return E_FAIL;

	/* Component : Ground_Blur */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Ground_Blur"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Dvalin_Entry/Eff_Sc_GroundBlur_01.png")))))
		return E_FAIL;

	/* Component : Ramp_Star */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Ramp_Star"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Dvalin_Entry/Eff_Sc_RampStar_01.png")))))
		return E_FAIL;

	/* Component : Eff_Universe */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_Universe"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Universe/Universe%d.png"),3))))
		return E_FAIL;

	/* Component : MultiChannel */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MultiChannel"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Dvalin_Entry/Eff_MultiChannel_03.png")))))
		return E_FAIL;

	/* Component : NoiseMulti */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoiseMulti"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Dvalin_Entry/Eff_Noise_Multi_01.png")))))
		return E_FAIL;

	/* Component : Turbulence */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Turbulence"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Dvalin_Entry/Eff_Turbulence_07.png")))))
		return E_FAIL;

	/* Component : Raiden_Symbol */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Raiden_Symbol"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Eff_Shougun_Symbol_01.png")))))
		return E_FAIL;
	
	/* Component : Raiden_Light */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Raiden_Light"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Texture2D/Eff_Trail_06_Clamp.png")))))
		return E_FAIL;

	/* Component : ChainLightning */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ChainLightning"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Effect/Lightning/ChainLightning.png")))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_ProtoType_Component_Texture_Map()
{
	/* Component : Texture SkyBox World */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkyBox_World"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/Terrain/SkyBox/SkyBox_World.dds")))))
		return E_FAIL;

	/* Component : Texture_SkyCylinder_World */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkyCylinder_World"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/SkyBox/MD_SkyCylinder/MD_SkyCylinder.dat"))))
		return E_FAIL;

	/* Component : Texture SkyBox Dungeon */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkyBox_Dungeon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/Terrain/SkyBox/SkyBox_Dungeon.dds")))))
		return E_FAIL;

	/* Component : Texture Grass Diffuse */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Grass_Diffuse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/Terrain/Diffuse/Terrain_Grass_Diffuse.png")))))
		return E_FAIL;

	/* Component : Texture Grass Normal */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Grass_Normal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/Terrain/Normal/Terrain_Grass_Normal.png")))))
		return E_FAIL;

	/* Component : Texture Road Diffuse */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Road_Diffuse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/Terrain/Diffuse/Terrain_Road_Diffuse.png")))))
		return E_FAIL;

	/* Component : Texture Road Normal */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Road_Normal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/Terrain/Normal/Terrain_Road_Normal.png")))))
		return E_FAIL;

	/* Component : Texture Stone Diffuse */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Stone_Diffuse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/Terrain/Diffuse/Terrain_Stone_Diffuse.png")))))
		return E_FAIL;

	/* Component : Texture Stone Normal */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Stone_Normal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/Terrain/Normal/Terrain_Stone_Normal.png")))))
		return E_FAIL;

	/* Component : Texture Mask */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/Terrain/MaskMap/WorldMaskMap.dds")))))
		return E_FAIL;

	/* Component : Texture Water */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Water"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/Terrain/Water/Water_0.dds")))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_ProtoType_Component_Texture_Asset()
{
#pragma region Tornado

	/* Component : Texture_Tornado */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Tornado"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Asset/Tornado/Tornado_%d.png"), 3))))
		return E_FAIL;

	/* Component : Model_Tornado */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Tornado"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Asset/Tornado/Tornado.dat"))))
		return E_FAIL;

	m_pGameInstance->Ready_FXPrototype(L"Tornado", CFX_Tornado::Create(m_pDevice, m_pContext));

#pragma endregion

#pragma region Meteorite

	/* Component : Texture_Meteorite */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Meteorite"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Asset/Meteorite/Meteorite_%d.png"), 1))))
		return E_FAIL;

	/* Component : Model_Meteorite */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Meteorite"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Asset/Meteorite/MeteoriteSphere.dat"))))
		return E_FAIL;

	m_pGameInstance->Ready_FXPrototype(L"Meteorite", CFX_Meteorite::Create(m_pDevice, m_pContext));

#pragma endregion

#pragma region ShockWave

	/* Component : Texture_ShockWave */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShockWave"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Asset/ShockWave/ShockWave_%d.png"), 3))))
		return E_FAIL;

	/* Component : Model_ShockWave_Cylinder_1 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_ShockWave_Cylinder_1"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Asset/ShockWave/ShockWave_Cylinder_1.dat"))))
		return E_FAIL;

	/* Component : Model_ShockWave_Cylinder_2 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_ShockWave_Cylinder_2"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Asset/ShockWave/ShockWave_Cylinder_2.dat"))))
		return E_FAIL;

	m_pGameInstance->Ready_FXPrototype(L"ShockWave", CFX_ShockWave::Create(m_pDevice, m_pContext));

#pragma endregion

#pragma region CircularIndicator

	/* Component : Model_CircularIndicator_Gage */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_CircularIndicator_Gage"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Asset/CircularIndicator/CircularIndicator_Gage.dat"))))
		return E_FAIL;

	/* Component : Model_CircularIndicator_Border */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_CircularIndicator_Border"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Asset/CircularIndicator/CircularIndicator_Border.dat"))))
		return E_FAIL;

	m_pGameInstance->Ready_FXPrototype(L"CircularIndicator", CFX_CircularIndicator::Create(m_pDevice, m_pContext));

#pragma endregion

#pragma region BarbaraES

	/* Component : Texture_Barbara_ES */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Barbara_ES"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Asset/Barbara_ES/Barbara_ES_%d.png"), 5))))
		return E_FAIL;

	/* Component : Model_Barbara_ES_1 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Barbara_ES_1"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Asset/Barbara_ES/Barbara_ES_1/Barbara_ES_1.dat"))))
		return E_FAIL;

	/* Component : Model_Barbara_ES_2 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Barbara_ES_2"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Asset/Barbara_ES/Barbara_ES_2/Barbara_ES_2.dat"))))
		return E_FAIL;

	/* Component : Model_Barbara_ES_3 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Barbara_ES_3"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Asset/Barbara_ES/Barbara_ES_3/Barbara_ES_3.dat"))))
		return E_FAIL;

	/* Component : Model_Barbara_ES_4 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Barbara_ES_4"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Asset/Barbara_ES/Barbara_ES_4/Barbara_ES_4.dat"))))
		return E_FAIL;

	m_pGameInstance->Ready_FXPrototype(L"BarbaraES", CFX_BarbaraES::Create(m_pDevice, m_pContext));

#pragma endregion

#pragma region RaidenES

	/* Component : Model_Raiden_ES */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Raiden_ES"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Asset/Raiden_ES/Raiden_ES.dat"))))
		return E_FAIL;

	m_pGameInstance->Ready_FXPrototype(L"RaidenES", CFX_RaidenES::Create(m_pDevice, m_pContext));

#pragma endregion

#pragma region EffigyFireAura

	/* Component : Texture_EffigyFire_Aura */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EffigyFire_Aura"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Asset/EffigyFire_Aura/EffigyFire_Aura_%d.png"), 2))))
		return E_FAIL;

	/* Component : Model_EffigyFire_Torus_0 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_EffigyFire_Torus_0"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Asset/EffigyFire_Aura/EffigyFire_Torus_0/EffigyFire_Torus_0.dat"))))
		return E_FAIL;

	/* Component : Model_EffigyFire_Torus_45 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_EffigyFire_Torus_45"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Asset/EffigyFire_Aura/EffigyFire_Torus_45/EffigyFire_Torus_45.dat"))))
		return E_FAIL;

	/* Component : Model_EffigyFire_Torus_90 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_EffigyFire_Torus_90"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Asset/EffigyFire_Aura/EffigyFire_Torus_90/EffigyFire_Torus_90.dat"))))
		return E_FAIL;

	/* Component : Model_EffigyFire_Torus_135 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_EffigyFire_Torus_135"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Asset/EffigyFire_Aura/EffigyFire_Torus_135/EffigyFire_Torus_135.dat"))))
		return E_FAIL;

	m_pGameInstance->Ready_FXPrototype(L"EffigyFireAura", CFX_EffigyFireAura::Create(m_pDevice, m_pContext));

#pragma endregion

#pragma region EffigyFireExplode

	/* Component : Texture_EffigyFire_Explode */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EffigyFire_Explode"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Asset/EffigyFire_Explode/EffigyFire_Explode_%d.png"), 4))))
		return E_FAIL;

	m_pGameInstance->Ready_FXPrototype(L"EffigyFireExplode", CFX_EffigyFireExplode::Create(m_pDevice, m_pContext));

#pragma endregion

#pragma region EffigyFireImpact

	/* Component : Texture_EffigyFire_Impact */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EffigyFire_Impact"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Asset/EffigyFire_Impact/EffigyFire_Impact_%d.png"), 2))))
		return E_FAIL;

	/* Component : Model_EffigyFire_HalfTorus */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_EffigyFire_HalfTorus"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Asset/EffigyFire_Impact/EffigyFire_HalfTorus/EffigyFire_HalfTorus.dat"))))
		return E_FAIL;

	/* Component : Model_EffigyFire_Thorn */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_EffigyFire_Thorn"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Asset/EffigyFire_Impact/EffigyFire_Thorn/EffigyFire_Thorn.dat"))))
		return E_FAIL;

	m_pGameInstance->Ready_FXPrototype(L"EffigyFireImpact", CFX_EffigyFireImpact::Create(m_pDevice, m_pContext));

#pragma endregion

#pragma region EffigyFirePillar

	/* Component : Texture_EffigyFire_Pillar */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EffigyFire_Pillar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Asset/EffigyFire_Pillar/EffigyFire_Pillar_%d.png"), 6))))
		return E_FAIL;

	/* Component : Model_EffigyFire_Cone */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_EffigyFire_Cone"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Asset/EffigyFire_Pillar/EffigyFire_Cone/EffigyFire_Cone.dat"))))
		return E_FAIL;

	/* Component : Model_EffigyFire_Clinder */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_EffigyFire_Clinder"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Asset/EffigyFire_Pillar/EffigyFire_Clinder/EffigyFire_Clinder.dat"))))
		return E_FAIL;

	/* Component : Model_EffigyFire_Circle */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_EffigyFire_Circle"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Asset/EffigyFire_Pillar/EffigyFire_Circle/EffigyFire_Circle.dat"))))
		return E_FAIL;

	m_pGameInstance->Ready_FXPrototype(L"EffigyFirePillar", CFX_EffigyFirePillar::Create(m_pDevice, m_pContext));

#pragma endregion

#pragma region EffigyFireProjectile

	/* Component : Texture_EffigyFire_Projectile */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EffigyFire_Projectile"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Asset/EffigyFire_Projectile/EffigyFire_Projectile_%d.png"), 2))))
		return E_FAIL;

	/* Component : Model_EffigyFire_Cylinder */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_EffigyFire_Cylinder"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Asset/EffigyFire_Projectile/EffigyFire_Cylinder/EffigyFire_Cylinder.dat"))))
		return E_FAIL;

	/* Component : Model_EffigyFire_Sphere */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_EffigyFire_Sphere"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Asset/EffigyFire_Projectile/EffigyFire_Sphere/EffigyFire_Sphere.dat"))))
		return E_FAIL;

	m_pGameInstance->Ready_FXPrototype(L"EffigyFireProjectile", CFX_EffigyFireProjectile::Create(m_pDevice, m_pContext));

#pragma endregion

#pragma region EffigyFireTrail

	/* Component : Texture_EffigyFire_Trail */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EffigyFire_Trail"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Asset/EffigyFire_Trail/EffigyFire_Trail_%d.png"), 2))))
		return E_FAIL;

	/* Component : VIBuffer_Trail_Fire2 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail_Fire"),
		CVIBuffer_Trail::Create(m_pDevice, m_pContext, 60))))
		return E_FAIL;

	m_pGameInstance->Ready_FXPrototype(L"EffigyFireTrail", CFX_EffigyFireTrail::Create(m_pDevice, m_pContext));

#pragma endregion

#pragma region EffigyFireMagicCircle

	/* Component : Texture_EffigyFire_Projectile */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EffigyFire_MagicCircle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Asset/EffigyFire_MagicCircle/Magic_Circle.png")))))
		return E_FAIL;

	m_pGameInstance->Ready_FXPrototype(L"EffigyFireMagicCircle", CFX_EffigyFire_MagicCircle::Create(m_pDevice, m_pContext));

#pragma endregion

#pragma region EffigyFireTrsCow

	/* Component : Texture_EffigyFire_TrsCow */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EffigyFire_TrsCow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Asset/EffigyFire_TrsCow/EffigyFire_TrsCow_%d.png"), 1))))
		return E_FAIL;

	/* Component : Model_EffigyFire_HalfSphere */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_EffigyFire_HalfSphere"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Asset/EffigyFire_TrsCow/EffigyFire_HalfSphere/EffigyFire_HalfSphere.dat"))))
		return E_FAIL;

	m_pGameInstance->Ready_FXPrototype(L"EffigyFireTrsCow", CFX_EffigyFireTrsCow::Create(m_pDevice, m_pContext));

#pragma endregion

#pragma region EffigyFireTrsFish

	/* Component : Texture_EffigyFire_TrsFish */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EffigyFire_TrsFish"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Asset/EffigyFire_TrsFish/EffigyFire_TrsFish_%d.png"), 2))))
		return E_FAIL;

	m_pGameInstance->Ready_FXPrototype(L"EffigyFireTrsFish", CFX_EffigyFireTrsFish::Create(m_pDevice, m_pContext));

#pragma endregion

#pragma region EffigyFireTrsBomb

	m_pGameInstance->Ready_FXPrototype(L"EffigyFireTrsBomb", CFX_EffigyFireTrsBomb::Create(m_pDevice, m_pContext));

#pragma endregion

#pragma region EffigyFireClap

	m_pGameInstance->Ready_FXPrototype(L"EffigyFireClap", CFX_EffigyFireClap::Create(m_pDevice, m_pContext));

#pragma endregion

#pragma region AndriusSavageTornado

	m_pGameInstance->Ready_FXPrototype(L"AndriusSavageTornado", CFX_AndriusSavageTornado::Create(m_pDevice, m_pContext));

#pragma endregion

#pragma region AndriusMeteorite

	m_pGameInstance->Ready_FXPrototype(L"AndriusMeteorite", CFX_AndriusMeteorite::Create(m_pDevice, m_pContext));

#pragma endregion

#pragma region Abyss

	/* Component : Texture_Abyss_Mage_Ice */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Abyss_Mage_Ice"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Body/Monster/Abyss_Mage/Monster_Abyss_01_Ice_Tex_Diffuse.png")))))
		return E_FAIL;

	/* Component : Texture_Abyss_Mage_Water */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Abyss_Mage_Water"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Body/Monster/Abyss_Mage/Monster_Abyss_01_Water_Tex_Diffuse.png")))))
		return E_FAIL;

	/* Component : Texture_Abyss_Mage_Fire */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Abyss_Mage_Fire"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/Body/Monster/Abyss_Mage/Monster_Abyss_01_Fire_Tex_Diffuse.png")))))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CMainApp::Ready_ProtoType_Component_Model_Character()
{
	/* Component : Model_Traveler */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Traveler"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/Character/Nahida/Body_Nahida.dat"))))
		return E_FAIL;

	/* Component : Model_Barbara */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Barbara"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/Character/Barbara/Body_Barbara.dat"))))
		return E_FAIL;

	/* Component : Model_Diluc */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Diluc"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/Character/Diluc/Body_Diluc.dat"))))
		return E_FAIL;

	///* Component : Model_Ganyu */
	//if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Ganyu"),
	//	CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/Character/Ganyu/Body_Ganyu.dat"))))
	//	return E_FAIL;

	/* Component : Model_Raiden */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Raiden"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/Character/Raiden/Body_Raiden.dat"))))
		return E_FAIL;

	/* Component : Model_Zhongli */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Zhongli"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/Character/Zhongli/Body_Zhongli.dat"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_ProtoType_Component_Model_Weapon()
{
	/* Component : Model_Wolfmound */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Wolfmound"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Weapon/Character/Claymore/Claymore_Wolfmound/Claymore_Wolfmound.dat"))))
		return E_FAIL;

	/* Component : Model_Kunwu */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Kunwu"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Weapon/Character/Pole/Pole_Kunwu/Pole_Kunwu.dat"))))
		return E_FAIL;

	/* Component : Model_Zephyrus */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Zephyrus"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Weapon/Character/Catalyst/Catalyst_Zephyrus/Catalyst_Zephyrus.dat"))))
		return E_FAIL;

	/* Component : Model_Narukami */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Narukami"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Weapon/Character/Pole/Pole_Narukami/Pole_Narukami.dat"))))
		return E_FAIL;

	/* Component : Model_Katana */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Katana"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Weapon/Character/Sword/Katana/Sword_Katana.dat"))))
		return E_FAIL;

	/* Component : Model_Slime_IceCone */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Slime_IceCone"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Weapon/Monster/Slime_IceCone/Slime_IceCone.dat"))))
		return E_FAIL;

	/* Component : Model_Hili_Club */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Hili_Club"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Weapon/Monster/Hili_Club/Hili_Club.dat"))))
		return E_FAIL;

	/* Component : Model_Brute_Shield */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Brute_Shield"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Weapon/Monster/Brute_Shield2/MonEquip_Shield_02_Model.dat"))))
		return E_FAIL;

	/* Component : Model_Brute_Axe */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Brute_Axe"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Weapon/Monster/Brute_Axe/MonEquip_Axe_Electric_01.dat"))))
		return E_FAIL;

	/* Component : Model_Staff_Elec */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Staff_Elec"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Weapon/Monster/Abyss_Mage_Staff_Elec/MonEquip_Staff_Electric_01_Model (merge).dat"))))
		return E_FAIL;

	/* Component : Model_Staff_Ice */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Staff_Ice"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Weapon/Monster/Abyss_Mage_Staff_Ice/MonEquip_Staff_Ice_01 (merge).dat"))))
		return E_FAIL;

	/* Component : Model_Staff_Ice */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Staff_Water"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Weapon/Monster/Abyss_Mage_Staff_Water/MonEquip_Staff_Water_01 (merge).dat"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_ProtoType_Component_Model_Glider()
{
	/* Component : Glider_Origin */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Glider_Origin"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Glider/Glider_Origin/Glider_Origin.dat"))))
		return E_FAIL;

	/* Component : Glider_Liyue */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Glider_Liyue"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Glider/Glider_Liyue/Glider_Liyue.dat"))))
		return E_FAIL;

	/* Component : Glider_Psalmus */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Glider_Psalmus"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Glider/Glider_Psalmus/Glider_Psalmus.dat"))))
		return E_FAIL;

	/* Component : Glider_Thunderclap */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Glider_Thunderclap"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Glider/Glider_Thunderclap/Glider_Thunderclap.dat"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_ProtoType_Component_Model_Monster()
{
	/* Component : Model_Andrius */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Andrius"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/Monster/Andrius/Monster_Andrius.dat"))))
		return E_FAIL;

	/* Component : Model_Slime_Water */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Slime_Water"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/Monster/Slime_Water/Slime_Water.dat"))))
		return E_FAIL;

	/* Component : Model_Slime_Ice */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Slime_Ice"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/Monster/Slime_Ice/Slime_Ice.dat"))))
		return E_FAIL;

	/* Component : Model_Hili_Normal */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Hili_Normal"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/Monster/Hili_Normal/Hili_Normal.dat"))))
		return E_FAIL;

	/* Component : Model_Brute */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Brute"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/Monster/Monster_Brute/Monster_Brute.dat"))))
		return E_FAIL;

	/* Component : Model_Effigy_Fire */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effigy_Fire"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/Monster/Effigy_Fire/Monster_Effigy_04_Model (merge).dat"))))
		return E_FAIL;

	/* Component : Model_Dvalin */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Dvalin"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/Monster/Dvalin/Monster_Dvalin_Model (merge).dat"))))
		return E_FAIL;

	/* Component : Model_Abyss_Mage */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Abyss_Mage"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/Monster/Abyss_Mage/Monster_Abyss_Electric_01.dat"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_ProtoType_Component_Model_NPC()
{
	/* Component : Model_Paimon */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Paimon"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Paimon/Paimon.dat"))))
		return E_FAIL;

	/* Component : Model_Katheryne */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Katheryne"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Katheryne/Body_Katheryne.dat"))))
		return E_FAIL;

	/* Component : Model_Ambor */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Ambor"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Ambor/Body_Ambor.dat"))))
		return E_FAIL;

	/* Component : Model_AndriusSword */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_AndriusSword"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Andrius_Sword/Andrius_Sword.dat"))))
		return E_FAIL;

	/* Component : Model_TelePort */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_TelePort"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/TelePort/TelePort.dat"))))
		return E_FAIL;

#pragma region Common NPC
	/* Knights */
	/* Component : Model_Swan */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Swan"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Swan/Body_Swan.dat"))))
		return E_FAIL;

	/* Component : Model_Lawrence */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Lawrence"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Lawrence/Body_Lawrence.dat"))))
		return E_FAIL;

	/* Component : Model_Guy */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Guy"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Guy/Body_Guy.dat"))))
		return E_FAIL;

	/* Component : Model_Miles */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Miles"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Miles/Body_Miles.dat"))))
		return E_FAIL;

	/* Component : Model_Raymond */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Raymond"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Raymond/Body_Raymond.dat"))))
		return E_FAIL;

	/* Church */
	/* Component : Model_Jilliana */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Jilliana"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Jilliana/Body_Jilliana.dat"))))
		return E_FAIL;

	/* Adventure */
	/* Component : Model_Jack */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Jack"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Jack/Body_Jack.dat"))))
		return E_FAIL;

	/* Component : Model_Lynn */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Lynn"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Lynn/Body_Lynn.dat"))))
		return E_FAIL;

	/* Component : Model_Pallad */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Pallad"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Pallad/Body_Pallad.dat"))))
		return E_FAIL;

	/* Component : Model_Stanley */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Stanley"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Stanley/Body_Stanley.dat"))))
		return E_FAIL;

	/* Castle */
	/* Component : Model_Albert */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Albert"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Albert/Body_Albert.dat"))))
		return E_FAIL;

	/* Component : Model_Anna */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Anna"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Anna/Body_Anna.dat"))))
		return E_FAIL;

	/* Component : Model_Anthony */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Anthony"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Anthony/Body_Anthony.dat"))))
		return E_FAIL;

	/* Component : Model_Beatrice */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Beatrice"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Beatrice/Body_Beatrice.dat"))))
		return E_FAIL;

	/* Component : Model_Blanche */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Blanche"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Blanche/Body_Blanche.dat"))))
		return E_FAIL;

	/* Component : Model_Charles */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Charles"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Charles/Body_Charles.dat"))))
		return E_FAIL;

	/* Component : Model_Donna */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Donna"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Donna/Body_Donna.dat"))))
		return E_FAIL;

	/* Component : Model_Eury */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eury"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Eury/Body_Eury.dat"))))
		return E_FAIL;

	/* Component : Model_Glory */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Glory"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Glory/Body_Glory.dat"))))
		return E_FAIL;

	/* Component : Model_Margaret */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Margaret"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Margaret/Body_Margaret.dat"))))
		return E_FAIL;

	/* Component : Model_Marjorie */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Marjorie"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Marjorie/Body_Marjorie.dat"))))
		return E_FAIL;

	/* Component : Model_Norma */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Norma"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Norma/Body_Norma.dat"))))
		return E_FAIL;

	/* Component : Model_Quinn */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Quinn"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Quinn/Body_Quinn.dat"))))
		return E_FAIL;

	/* Component : Model_Sansa */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Sansa"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Sansa/Body_Sansa.dat"))))
		return E_FAIL;

	/* Component : Model_Sara */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Sara"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Sara/Body_Sara.dat"))))
		return E_FAIL;

	/* Component : Model_Susie */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Susie"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Susie/Body_Susie.dat"))))
		return E_FAIL;

	/* Component : Model_Wagner */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Wagner"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Wagner/Body_Wagner.dat"))))
		return E_FAIL;

	/* Component : Model_Aramis */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Aramis"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Aramis/Body_Aramis.dat"))))
		return E_FAIL;

	/* Component : Model_Schulz */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Schulz"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Schulz/Body_Schulz.dat"))))
		return E_FAIL;

	/* Component : Model_Timaeus */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Timaeus"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Timaeus/Body_Timaeus.dat"))))
		return E_FAIL;

	/* Component : Model_Michelle */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Michelle"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Body/NPC/Michelle/Body_Michelle.dat"))))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CMainApp::Ready_ProtoType_Component_Model_Skill()
{
	/* Component : Model_Pillar */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Pillar"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/SkillObj/Zhongli/Pillar/Pillar.dat"))))
		return E_FAIL;

	/* Component : Model_Zhongli_Shield */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Zhongli_Shield"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/SkillObj/Zhongli/Shield/Zhongli_Shield.dat"))))
		return E_FAIL;

	/* Component : Model_FallingStone */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_FallingStone"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/SkillObj/Zhongli/FallingStone/Falling_Stone.dat"))))
		return E_FAIL;

	/* Component : Model_Burst_BackGround */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Burst_BackGround"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/SkillObj/Zhongli/Burst_BackGround/ZhongliBG.dat"))))
		return E_FAIL;

	/* Component : Model_Phoenix */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Phoenix"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/SkillObj/Diluc/Phoenix/Phoenix.dat"))))
		return E_FAIL;

	/* Component : Model_IceCone */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_IceCone"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/SkillObj/Andrius/Andrius_IceCone/IceCone.dat"))))
		return E_FAIL;

	/* Component : Model_IceShockWave */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_IceShockWave"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/SkillObj/Andrius/Andrius_IceCone/IceCone.dat"))))
		return E_FAIL;

	/* Component : Model_IcePillar */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_IcePillar"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/SkillObj/Andrius/Andrius_IceCone/IceCone.dat"))))
		return E_FAIL;

	/* Component : Model_WindBlade */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_WindBlade"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/SkillObj/Andrius/Andrius_WindBlade/Andrius_WindBlade.dat"))))
		return E_FAIL;

	/* Component : Model_WindRange */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_WindRange"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/SkillObj/Andrius/Andrius_WindRange/Andrius_WindRange.dat"))))
		return E_FAIL;

	/* Component : Model_Rush */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rush"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/SkillObj/Andrius/Andrius_Rush/Andrius_Rush.dat"))))
		return E_FAIL;

	/* Component : Model_Rush2 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Rush2"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/SkillObj/Andrius/Andrius_Rush2/Andrius_Rush2.dat"))))
		return E_FAIL;

	/* Component : Model_Andrius_Scratch */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Andrius_Claw"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/SkillObj/Andrius/Andrius_Claw/Andrius_Scratch.dat"))))
		return E_FAIL;

	/* Component : Model_Effigy_FireCube */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effigy_FireCube"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/SkillObj/Effigy/Fire_Cube/Eff_Model_Effigy_Fire_Shield.dat"))))
		return E_FAIL;

	/* Component : Model_Effigy_FirePillar */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effigy_FirePillar"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/SkillObj/Effigy/Fire_Pillar/SkillObj_Monster_Effigy_Fire_Pillar_Model.dat"))))
		return E_FAIL;

	/* Component : Model_Effigy_FireCore */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Effigy_FireCore"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/SkillObj/Effigy/Fire_Core/Fire_Core.dat"))))
		return E_FAIL;

	/* Component : Texture_Chakra */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Chakra"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Model/SkillObj/Raiden/Eff_Chakra/Eff_Chakra_%d.png"), 1))))
		return E_FAIL;

	/* Component : Model_Chakra */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Chakra"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/SkillObj/Raiden/Eff_Chakra/Eff_Chakra.dat"))))
		return E_FAIL;

	/* Component : Model_Lightning */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Lightning"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Effect/Lightning/Lightning.dat"))))
		return E_FAIL;

	/* Component : Model_Fog */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Fog"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/FOG/Fog.dat"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_ProtoType_Component_Model_Effect()
{
	/* Component : Model_Sphere */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Sphere"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Primitive_Models/Sphere2.dat"))))
		return E_FAIL;

	/* Component : Model_Slash */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Slash"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Primitive_Models/Slash2.dat"))))
		return E_FAIL;

	/* Component : Model_Slash */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Slash_19_12"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Primitive_Models/Slash3.dat"))))
		return E_FAIL;

	/* Component : Model_Slash */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Slash_20"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Primitive_Models/Slash4.dat"))))
		return E_FAIL;

	/* Component : Model_Cross */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Cross"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Primitive_Models/Cross.dat"))))
		return E_FAIL;

	/* Component : Model_Eff_WindField */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eff_WindField"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Effect/Enviroment/WindField.dat"))))
		return E_FAIL;

	/* Component : Model_Eff_Shougun_Trail_7_0 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eff_Shougun_Trail_7_0"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Effect/Trail/Trail7_0.dat"))))
		return E_FAIL;

	/* Component : Model_Eff_Shougun_Trail_7_1 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eff_Shougun_Trail_7_1"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Effect/Trail/Trail7_1.dat"))))
		return E_FAIL;
	/* Component : Model_Eff_Shougun_Trail_7_2 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eff_Shougun_Trail_7_2"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Effect/Trail/Trail7_2.dat"))))
		return E_FAIL;
	/* Component : Model_Eff_Shougun_Trail_7_3 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eff_Shougun_Trail_7_3"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Effect/Trail/Trail7_3.dat"))))
		return E_FAIL;
	/* Component : Model_Eff_Shougun_Trail_7_4 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eff_Shougun_Trail_7_4"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Effect/Trail/Trail7_4.dat"))))
		return E_FAIL;
	/* Component : Model_Eff_Shougun_Trail_7_5 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eff_Shougun_Trail_7_5"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Effect/Trail/Trail7_5.dat"))))
		return E_FAIL;
	/* Component : Model_Eff_Shougun_Trail_7_6 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eff_Shougun_Trail_7_6"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Effect/Trail/Trail7_6.dat"))))
		return E_FAIL;
	/* Component : Model_Eff_Shougun_Trail_7_7 */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eff_Shougun_Trail_7_7"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Effect/Trail/Trail7_7.dat"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eff_Shougun_Trail_7_8"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Effect/Trail/Trail7_8.dat"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eff_Shougun_Trail_7_9"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Effect/Trail/Trail7_9.dat"))))
		return E_FAIL;

	/* Component : Model_Eff_Barbara_Star */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eff_Barbara_Star"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Effect/Barbara/Normal_Attack/Normal_Attack_Star.dat"))))
		return E_FAIL;

	/* Component : Model_Eff_WorldSkyBox */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eff_WorldSkyBox"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Effect/Eff_WorldSkyBox/Eff_WorldSkyBox.dat"))))
		return E_FAIL;

	/* Component : Model_Eff_DungeonSkyBox */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eff_DungeonSkyBox"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Effect/Eff_DungeonSkyBox/Eff_DungeonSkyBox.dat"))))
		return E_FAIL;

	/* Component : Model_Eff_DvalinSkyBox */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Eff_DvalinSkyBox"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Effect/Eff_DvalinSkyBox/Eff_DvalinSkyBox.dat"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_ProtoType_Component_Model_Interaction()
{
	/* Component : Model_Relic_Item */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Relic_Item"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Item/Relic_Item/Relic_Item.dat"))))
		return E_FAIL;

	/* Component : Model_Gliding_Ring */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Gliding_Ring"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Gliding_Ring/Gliding_Ring.dat"))))
		return E_FAIL;

	/* Component : Model_VisionGate */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_VisionGate"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/VisionGate/VisionGate.dat"))))
		return E_FAIL;

	/* Component : Model_DvalinEntry */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_DvalinEntry"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/Dvalin_Entry/Dvalin_Entry.dat"))))
		return E_FAIL;

	/* Component : Model_DvalinPlatform */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_DvalinPlatform"),
		CModel::Create(m_pDevice, m_pContext, "../../Reference/Resources/Model/DvalinPlatform/DvalinPlatform.dat"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Fonts()
{
	// /* USE TEMP */
	 if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Default"), TEXT("../../Reference/Resources/Font/DefaultFont.spritefont"))))
	 	return E_FAIL;

	 if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Number"), TEXT("../../Reference/Resources/Font/NumberFont.spritefont"))))
		 return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Loading_WorldModel(LEVELID eLevel, wstring strDataPath)
{
	if (true == strDataPath.empty())
		return E_FAIL;

	if (true == strDataPath.empty())
		return E_FAIL;

	HANDLE hFinder;
	WIN32_FIND_DATA tagFindData;

	_bool bResult = true;

	_tchar szDir[MAX_PATH] = {};
	_tchar szEXT[MAX_PATH] = {};
	wstring strNewFilePath;
	wstring strFullPath;

	hFinder = FindFirstFile(strDataPath.c_str(), &tagFindData);

	if (INVALID_HANDLE_VALUE == hFinder)
		return E_FAIL;

	_wsplitpath_s(strDataPath.c_str(), NULL, NULL, szDir, MAX_PATH, NULL, NULL, NULL, NULL);

	while (bResult)
	{
		_wsplitpath_s(tagFindData.cFileName, NULL, NULL, NULL, NULL, NULL, NULL, szEXT, MAX_PATH);

		if (tagFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (lstrcmp(tagFindData.cFileName, L".") && lstrcmp(tagFindData.cFileName, L".."))
			{
				strNewFilePath = L"";
				strNewFilePath += szDir;
				strNewFilePath += tagFindData.cFileName;
				strNewFilePath += L"/*.*";

				wstring strFolderTag = tagFindData.cFileName;

				Loading_WorldModel(eLevel, strNewFilePath);
			}
		}
		else if (!lstrcmp(szEXT, TEXT(".dat")))
		{
			strFullPath = L"";
			strFullPath += szDir;
			strFullPath += tagFindData.cFileName;

			string strPath = ConvertWCToC(strFullPath);

			wstring strProtoTag = TEXT("Prototype_Component_Model_");

			_tchar szName[MAX_PATH];
			_wsplitpath_s(strFullPath.c_str(), NULL, NULL, NULL, NULL, szName, MAX_PATH, NULL, NULL);

			strProtoTag.append(szName);

			if (FAILED(m_pGameInstance->Add_Component_Prototype(eLevel, strProtoTag,
				CModel::Create(m_pDevice, m_pContext, strPath))))
				return E_FAIL;
		}

		bResult = FindNextFile(hFinder, &tagFindData);
	}

	FindClose(hFinder);

	return S_OK;
}

HRESULT CMainApp::Initialize()
{
	/* Ready Engine */
	GRAPHIC_DESC	eGraphicDesc;
	ZeroMemory(&eGraphicDesc, sizeof GRAPHIC_DESC);

	eGraphicDesc.iWinSizeX = g_iWinSizeX;
	eGraphicDesc.iWinSizeY = g_iWinSizeY;
	eGraphicDesc.isWindowed = GRAPHIC_DESC::MODE_WIN;
	eGraphicDesc.hWnd = g_hWnd;

	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, COLL_END, SE_END, g_hInstance, eGraphicDesc, &m_pDevice, &m_pContext)))
	{
		MSG_BOX(TEXT("Failed to Initialize : Engine"));
		return E_FAIL;
	}

	/* Ready Client */
	m_pElemental_Manager = CElemental_Manager::Get_Instance();
	if (FAILED(m_pElemental_Manager->Initialize(m_pDevice, m_pContext)))
	{
		MSG_BOX(TEXT("Failed to Initialize : CElemental_Manager"));
		return E_FAIL;
	}

	if (FAILED(Ready_Json())) 
	{
		MSG_BOX(TEXT("Failed to Ready Prototype : CMainApp"));
		return E_FAIL;
	}

	if (FAILED(Ready_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Ready Prototype : CMainApp"));
		return E_FAIL;
	}

	if (FAILED(Ready_Fonts()))
	{
		MSG_BOX(TEXT("Failed to Ready Fonts : CMainApp"));
		return E_FAIL;
	}

	// Mouse
	/* Component : Texture_Cursor */
	if (FAILED(m_pGameInstance->Add_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Cursor"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Reference/Resources/Texture/UI_Textures/GamePlay_UI/UI_Img_Cursor.png")))))
		return E_FAIL;

	/* GameObject : Mouse */
	if (FAILED(m_pGameInstance->Add_Object_Prototype(TEXT("Prototype_GameObject_Mouse"),
		CMouse::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_STATIC, L"Prototype_GameObject_Mouse", L"Layer_Mouse")))
		return E_FAIL;

#ifndef _DEBUGNUI
	m_pDebugTool = CDebugTool::Create(m_pDevice, m_pContext);
#endif

	//if (FAILED(Open_Level(LEVEL_STRONGHOLD_ABYSSMAGE)))
	//if (FAILED(Open_Level(LEVEL_TEMPLE_LION)))
	//if (FAILED(Open_Level(LEVEL_STORMTERROR)))
	if (FAILED(Open_Level(LEVEL_WORLD)))
	{
		MSG_BOX(TEXT("Failed to Open Level : CMainApp"));
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMainApp::PreviousTick(_float fTimeDelta)
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;

	if (FAILED(m_pElemental_Manager->PreviousTick(fTimeDelta)))
		return E_FAIL;

	m_fTimeAcc += fTimeDelta;
	++m_iFPS;

	if (m_fTimeAcc >= 1.f)
	{
		wstring strFPS = TEXT("FPS ") + to_wstring(m_iFPS);

		SetWindowText(g_hWnd, strFPS.c_str());

		m_iFPS = 0;
		m_fTimeAcc = 0.f;
	}

#ifdef _DEBUG
	m_fTimeAcc += fTimeDelta;
	++m_iFPS;

	if (m_fTimeAcc >= 1.f)
	{
		wstring strFPS = TEXT("FPS ") + to_wstring(m_iFPS);

		SetWindowText(g_hWnd, strFPS.c_str());

		m_iFPS = 0;
		m_fTimeAcc = 0.f;
	}

#endif

	return m_pGameInstance->PreviousTick_Engine(fTimeDelta);
}

HRESULT CMainApp::Tick(_float fTimeDelta)
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;

	if (FAILED(m_pElemental_Manager->Tick(fTimeDelta)))
		return E_FAIL;

	return m_pGameInstance->Tick_Engine(fTimeDelta);
}

HRESULT CMainApp::LateTick(_float fTimeDelta)
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;

	if (FAILED(m_pElemental_Manager->LateTick(fTimeDelta)))
		return E_FAIL;

	return m_pGameInstance->LateTick_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	if (m_pGameInstance == nullptr)
		return E_FAIL;

	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pRenderer->Draw_RenderGroup();
	m_pGameInstance->Render_Level();

#ifndef _DEBUGNUI
	m_pDebugTool->Render();
#endif

	m_pGameInstance->Present();

	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp* pMainApp = new CMainApp();

	if (FAILED(pMainApp->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CMainApp"));
		Safe_Release(pMainApp);
	}

	return pMainApp;
}

void CMainApp::Free()
{
	__super::Free();
  
	Safe_Release(m_pElemental_Manager);

#ifndef _DEBUGNUI
	Safe_Release(m_pDebugTool);
#endif

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pRenderer);
	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
}