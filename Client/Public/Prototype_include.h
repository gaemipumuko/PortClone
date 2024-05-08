#pragma once

#pragma region Prototype GameObject

/* Player */
#include "Player.h"

/* Camera */
#include "Camera_Main.h"
#include "Camera_Char.h"

/* Monster */
#include "Slime_Small.h"
#include "Slime_Big.h"
#include "Hili_Normal.h"
#include "Brute.h"
#include "Andrius.h"
#include "Effigy_Fire.h"
#include "Dvalin.h"
#include "Abyss_Mage.h"

/* Effect */
#include "Effect_Flame.h"
#include "Effect_Hitting.h"
#include "Effect_Rush_Circle.h"
#include "Effect_Ice_Floor.h"
#include "Effect_Fog.h"
#include "Flame0.h"
#include "Eff_Flame1.h"
#include "FlameSlash_Trail.h"
#include "ItemGlow.h"
#include "Eff_Impact_Glow.h"
#include "Eff_Squence.h"
#include "Eff_Flame_Enchant_Cross.h"
#include "Eff_WindField.h"
#include "Eff_Barbara_Normal_Star.h"
#include "Eff_Water_Fog.h"
#include "Eff_Barbara_Burst_Magic_Circle.h"
#include "EFF_TEST.h"
#include "Pillar_Wave.h"
#include "Piercing_Trail.h"
#include "FallingSpearTrail.h"
#include "Eff_Transition.h"
#include "Eff_GroundStomp.h"
#include "Eff_Cross.h"
#include "Eff_Barbara_Normal_Atk.h"
#include "Eff_Liquid_Splash.h"
#include "Eff_Smoke.h"
#include "Eff_Effigy_Recharge_Trail.h"
#include "LineTrap.h"
#include "ChainLightning.h"
#include "DoorPortal.h"
#include "Shougun_Trail_TEST.h"
#include "Eff_Activate.h"
#include "Raiden_Trail_Add.h"

/* Skill */
#include "Skill_Pillar.h"
#include "Skill_Zhongli_Shield.h"
#include "Skill_FallingStone.h"
#include "Skill_FallingStoneTrail.h"
#include "Skill_FallingSpear.h"
#include "Skill_BurstBackGround.h"
#include "Skill_Phoenix.h"
#include "Skill_IceCone.h"
#include "Skill_IceShockWave.h"
#include "Skill_IcePillar.h"
#include "Skill_WindBlade.h"
#include "Skill_WindRange.h"
#include "Skill_Rush.h"
#include "Skill_Rush2.h"
#include "Skill_Claw.h"
#include "Skill_FireCube.h"
#include "Skill_FirePillar.h"
#include "Skill_FireCore.h"
#include "Zhongli_SpExplode.h"
#include "Element_Shield.h"
#include "ElementalReaction.h"
#include "BabaraBasicAttack.h"
#include "Raiden_ElementAttack.h"
#include "Slash_Trail.h"
#include "Skill_BigExplosion.h"
#include "Skill_Abyss_Mage_Shield.h"
#include "Skill_Chakra.h"
#include "Raiden_Symbol.h"
#include "Lightning.h"

/* NPC */
#include "Paimon.h"
#include "AndriusSword.h"
#include "Katheryne.h"
#include "Ambor.h"
#include "CommonNPC.h"
#include "Swan.h"
#include "TelePort.h"

/* UI */
#include "Rect.h"
#include "UI_Decor.h"
#include "AlphaRect.h"
#include "LoadingRect.h"
#include "Mannequin.h"
#include "UI_Manager.h"
#include "Quest_Manager.h"
#include "Button.h"
#include "ButtonAlpha.h"
#include "UI_DualTex.h"
#include "SelRect.h"
#include "Page_Main.h"
#include "Main_Party_Buttons.h"
#include "Hp_Bar.h"
#include "Stamina_Bar.h"
#include "Paimon_Page.h"
#include "Gacha_Page.h"
#include "Gacha_Result_Page.h"
#include "Dialogue_Page.h"
#include "Quest_Page.h"
#include "Element_OHD.h"
#include "Inventory.h"
#include "Item_Weapon.h"
#include "Item_Artifact.h"
#include "Item_Material.h"
#include "Item_Consumable.h"
#include "Ground_Item.h"
#include "Shop_Page.h"
#include "Alchemy_Page.h"
#include "Cooking_Page.h"

/* Interaction */
#include "SpawnBox.h"
#include "Updraft.h"
#include "Gliding_Ring.h"
#include "Accelerating_Ring.h"
#include "Gate.h"
#include "Location.h"

/* Map */
#include "World.h"
#include "SkyBox.h"
#include "Dungeon.h"

/* etc. */
#include "BossChest.h"

#pragma endregion

#pragma region Prototype PartObject

/* PartObject : Body Character */
#include "Body_Traveler.h"
#include "Body_Barbara.h"
#include "Body_Diluc.h"
#include "Body_Raiden.h"
#include "Body_Zhongli.h"

/* PartObject : Body Monster */
#include "Body_Andrius.h"
#include "Body_Slime_Water.h"
#include "Body_Slime_Ice.h"
#include "Body_Hili_Normal.h"
#include "Body_Brute.h"
#include "Body_Effigy_Fire.h"
#include "Body_Dvalin.h"
#include "Body_Abyss_Mage.h"

/* PartObject : Body NPC */
#include "Body_Paimon.h"
#include "Body_Katheryne.h"
#include "Body_Ambor.h"
#include "Body_NPCMale.h"
#include "Body_NPCFeMale.h"
#include "Body_Swan.h"
#include "Body_TelePort.h"

/* PartObject : Weapon */
#include "Claymore_Wolfmound.h"
#include "Pole_Kunwu.h"
#include "Catalyst_Zephyrus.h"
#include "Pole_Narukami.h"
#include "Slime_IceCone.h"
#include "Hili_Club.h"
#include "Brute_Shield.h"
#include "Brute_Axe.h"
#include "Staff_Elec.h"
#include "Staff_Ice.h"
#include "Staff_Water.h"

/* PartObject : Glider */
#include "Glider_Origin.h"
#include "Glider_Liyue.h"
#include "Glider_Psalmus.h"
#include "Glider_Thunderclap.h"

/* PartObject : Status */
#include "Status_Diluc.h"
#include "Status_Zhongli.h"
#include "Status_Barbara.h"
#include "Status_Raiden.h"
#include "Status_Andrius.h"
#include "Status_Dvalin.h"

#pragma endregion