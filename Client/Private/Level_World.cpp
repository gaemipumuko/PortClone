#include "stdafx.h"
#include "Level_World.h"
#include "DeciderObject.h"

#include "Ortho.h"
#include "Camera_Main.h"
#include "Camera_Char.h"

#include "GameInstance.h"

#include "Effect.h"

#include "Ground_Item.h"

#include "SpawnBox.h"
#include "Updraft.h"

#include "SkyBox.h"
#include "CommonNPC.h"
#include "TelePort.h"
#include "Gate.h"
#include "Location.h"

#include "Level_Loading.h"

#define CURRENT_LEVEL LEVEL_WORLD

CLevel_World::CLevel_World(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_World::Ready_Level()
{
	if (FAILED(m_pGameInstance->Init_Scene()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_ConstantBuffer()))
		return E_FAIL;

	if (FAILED(Ready_ParticleManager()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	 if (FAILED(Ready_Layer_World(TEXT("Layer_World"))))
	 	return E_FAIL;

	 if (FAILED(Ready_Layer_Icon(TEXT("Layer_Icon"))))
		 return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_EventCollider(TEXT("Layer_EventCollider"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_NPC(TEXT("Layer_NPC"))))
		return E_FAIL;

	 if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
	 	return E_FAIL;

	 if (FAILED(Ready_Layer_TelePort(TEXT("Layer_TelePort"))))
		 return E_FAIL;

	 if (FAILED(Ready_Layer_LocationCollider(TEXT("Layer_Location"))))
		 return E_FAIL;

	/* Init RenderMode */
	CDeciderObject::Set_RenderMode(CDeciderObject::RMODE_GAMEPLAY);

	return S_OK;
}

HRESULT CLevel_World::Ready_Lights()
{
	LIGHT_DESC			LightDesc = {};

	/* Create Directional Light */
	LightDesc.eLightType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(-0.45f, -0.89f, 0.23f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 0.6f);

	if (FAILED(m_pGameInstance->Add_Light(CURRENT_LEVEL, LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_World::Ready_ConstantBuffer()
{
	m_pGameInstance->Update_LUTIndex(3);
	// m_pGameInstance->Update_LUTIndex(24);

	_float		fBloomScale		= { 0.7f };
	_float		fBloomThreshold = { 10.f };
	m_pGameInstance->Update_BloomCB(fBloomScale, fBloomThreshold);

	_float		fSaturation		= { 1.f };
	_float		fExposure		= { 1.f };
	_float		fContrast		= { 1.f };
	_float3		vColorFilter	= { 1.f, 1.f, 1.f };
	m_pGameInstance->Update_ColorGradeCB(vColorFilter, fSaturation, fExposure, fContrast);

	return S_OK;
}

HRESULT CLevel_World::Ready_ParticleManager()
{
	if (FAILED(m_pGameInstance->Prepare_ParticleManager(
		static_cast<CTexture*>(m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Particle"))),
		static_cast<CRenderer*>(m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"))))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Load_Particles(L"../../Reference/Resources/Data/Effect/Particle.dat")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_World::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CCamera_Main::CAMERAMAIN_DESC tCameraMainDesc = {  };

	tCameraMainDesc.tCameraInfo.fMouseSensor = 0.025f;

	tCameraMainDesc.tCameraDesc.fFovy = XMConvertToRadians(60.f);
	tCameraMainDesc.tCameraDesc.fAspect = static_cast<_float>(g_iWinSizeX) / static_cast<_float>(g_iWinSizeY);
	tCameraMainDesc.tCameraDesc.fNear = 0.2f;
	tCameraMainDesc.tCameraDesc.fFar = 400.f;

	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Camera"), strLayerTag, &tCameraMainDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_World::Ready_Layer_World(const wstring& strLayerTag)
{
	CSkyBox::SKYBOX_DESC tSkyBoxDesc = {};
	{
		tSkyBoxDesc.eLevel = CURRENT_LEVEL;
	}

	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_SkyBox"), strLayerTag, &tSkyBoxDesc)))
		return E_FAIL;

	 if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_World"), strLayerTag)))
		 return E_FAIL;

	 _float4x4 vVisionGateMatrix;
	 {
		 XMStoreFloat4x4(&vVisionGateMatrix, XMMatrixIdentity());
		 vVisionGateMatrix.m[3][0] = 724.0f;
		 vVisionGateMatrix.m[3][1] = 10.0f;
		 vVisionGateMatrix.m[3][2] = 190.0f;
		 vVisionGateMatrix.m[3][3] = 1.0f;
	 }

	 CGate::GATE_DESC tVisionGateDesc = {};
	 {
		 tVisionGateDesc.eGateType = CGate::GATE_VISION;
		 tVisionGateDesc.vWorldMatrix = vVisionGateMatrix;
		 tVisionGateDesc.szModelTag = TEXT("Prototype_Component_Model_VisionGate");
		 tVisionGateDesc.eLevel = CURRENT_LEVEL;
		 tVisionGateDesc.eReserveLevel = LEVEL_STRONGHOLD_ABYSSMAGE;
	 }

	 if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Gate"), strLayerTag, &tVisionGateDesc)))
		 return E_FAIL;

	 _float4x4 vDvalinEntryMatrix;
	 {
		 XMStoreFloat4x4(&vDvalinEntryMatrix, XMMatrixIdentity());
		 vDvalinEntryMatrix.m[3][0] = 366.0f;
		 vDvalinEntryMatrix.m[3][1] = 59.0f;
		 vDvalinEntryMatrix.m[3][2] = 663.0f;
		 vDvalinEntryMatrix.m[3][3] = 1.0f;
	 }

	 CGate::GATE_DESC tDvalinEntryDesc = {};
	 {
		 tDvalinEntryDesc.eGateType = CGate::GATE_DVALIN;
		 tDvalinEntryDesc.vWorldMatrix = vDvalinEntryMatrix;
		 tDvalinEntryDesc.szModelTag = TEXT("Prototype_Component_Model_DvalinEntry");
		 tDvalinEntryDesc.eLevel = CURRENT_LEVEL;
		 tDvalinEntryDesc.eReserveLevel = LEVEL_STORMTERROR;
	 }

	 if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Gate"), strLayerTag, &tDvalinEntryDesc)))
		 return E_FAIL;

	return S_OK;
}

HRESULT CLevel_World::Ready_Layer_Player(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Player"), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Temp_NPC"), strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_World::Ready_Layer_Monster(const wstring& strLayerTag)
{
	OBJECT_DESC		ObjDesc = {};
	ObjDesc.vInitPos = XMVectorSet(820.047f, 10.9661f, 456.029f, 1.0f);
	ObjDesc.iObjectType = MONSTER_EFFIGY_FIRE;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Effigy_Fire"), strLayerTag, &ObjDesc)))
		return E_FAIL;

#pragma region 주석
	//if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Andrius"), strLayerTag)))
	//	return E_FAIL;

	//OBJECT_DESC		ObjDesc = {};
	//ObjDesc.vInitPos = XMVectorSet(953.0f, 0.f, 207.0f, 1.0f);
	//ObjDesc.iObjectType = MONSTER_SLIME_ICE;
	//if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Slime_Big"), strLayerTag, &ObjDesc)))
	//	return E_FAIL;

	//ObjDesc.vInitPos = XMVectorSet(953.0f, 0.f, 200.0f, 1.0f);
	//ObjDesc.iObjectType = static_cast<HILI_TYPE>(HILI_NORMAL);
	//if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Hili_Normal"), strLayerTag, &ObjDesc)))
		//return E_FAIL;

	//OBJECT_DESC		ObjDesc = {};
	//ObjDesc.vInitPos = XMVectorSet(953.0f, 0.f, 200.0f, 1.0f);
	//ObjDesc.iObjectType = MONSTER_EFFIGY_FIRE;
	//if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Effigy_Fire"), strLayerTag, &ObjDesc)))
	//	return E_FAIL;

	//OBJECT_DESC		ObjDesc;
	//ObjDesc = {};
	//ObjDesc.vInitPos = XMVectorSet(978.0f, 0.f, 192.0f, 1.0f);
	//ObjDesc.iObjectType = MONSTER_BRUTE_AXE;
	//if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Brute"), strLayerTag, &ObjDesc)))
	//	return E_FAIL;

	//OBJECT_DESC		ObjDesc;
	//ObjDesc = {};
	//ObjDesc.vInitPos = XMVectorSet(807.0f, 25.f, 178.0f, 1.0f);
	//if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Slime_Small"), strLayerTag, &ObjDesc)))
	//	return E_FAIL;

	//ObjDesc = {};
	//ObjDesc.vInitPos = XMVectorSet(812.0f, 25.f, 178.0f, 1.0f);
	//if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Slime_Small"), strLayerTag, &ObjDesc)))
	//	return E_FAIL;

	//ObjDesc = {};
	//ObjDesc.vInitPos = XMVectorSet(809.0f, 25.f, 174.0f, 1.0f);
	//if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Slime_Small"), strLayerTag, &ObjDesc)))
	//	return E_FAIL;

	/*ObjDesc = {};
	ObjDesc.vInitPos = XMVectorSet(807.0f, 25.f, 185.0f, 1.0f);
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Slime_Big"), strLayerTag, &ObjDesc)))
		return E_FAIL;

	ObjDesc = {};
	ObjDesc.vInitPos = XMVectorSet(812.0f, 25.f, 185.0f, 1.0f);
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Slime_Big"), strLayerTag, &ObjDesc)))
		return E_FAIL;*/

	///* 츄츄 */

	//ObjDesc = {};
	//ObjDesc.vInitPos = XMVectorSet(943.0f, 0.f, 180.0f, 1.0f);
	//ObjDesc.iObjectType = static_cast<HILI_TYPE>(HILI_DANCE);
	//if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Hili_Normal"), strLayerTag, &ObjDesc)))
	//	return E_FAIL;

	//ObjDesc = {};
	//ObjDesc.vInitPos = XMVectorSet(963.0f, 0.f, 180.0f, 1.0f);
	//ObjDesc.iObjectType = static_cast<HILI_TYPE>(HILI_DANCE);
	//if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Hili_Normal"), strLayerTag, &ObjDesc)))
	//	return E_FAIL;

	//ObjDesc = {};
	//ObjDesc.vInitPos = XMVectorSet(943.0f, 0.f, 190.0f, 1.0f);
	//ObjDesc.iObjectType = static_cast<HILI_TYPE>(HILI_DANCE);
	//if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Hili_Normal"), strLayerTag, &ObjDesc)))
	//	return E_FAIL;

	//ObjDesc = {};
	//ObjDesc.vInitPos = XMVectorSet(963.0f, 0.f, 190.0f, 1.0f);
	//ObjDesc.iObjectType = static_cast<HILI_TYPE>(HILI_DANCE);
	//if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Hili_Normal"), strLayerTag, &ObjDesc)))
	//	return E_FAIL;

	/*ObjDesc.vInitPos = XMVectorSet(40.f, 0.f, 20.f, 1.f);
	ObjDesc.iObjectType = static_cast<HILI_TYPE>(HILI_DANCE);
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Hili_Normal"), strLayerTag, &ObjDesc)))
		return E_FAIL;

	ObjDesc.vInitPos = XMVectorSet(40.f, 0.f, 30.f, 1.f);
	ObjDesc.iObjectType = static_cast<HILI_TYPE>(HILI_SIT);
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Hili_Normal"), strLayerTag, &ObjDesc)))
		return E_FAIL;

	ObjDesc.vInitPos = XMVectorSet(40.f, 0.f, 40.f, 1.f);
	ObjDesc.iObjectType = static_cast<HILI_TYPE>(HILI_SLEEP);
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Hili_Normal"), strLayerTag, &ObjDesc)))
		return E_FAIL;*/
#pragma endregion

	return S_OK;
}

HRESULT CLevel_World::Ready_Layer_EventCollider(const wstring& strLayerTag)
{
	//CSpawnBox::SPAWNBOX_DESC tDesc = {};
	//tDesc.eLevel = LEVEL_WORLD;
	//tDesc.eCreature = MONSTER_HILI_NORMAL;
	//tDesc.iSpawnNum = 7;
	//tDesc.fSpawnLengthToCenter = 4.f;
	//tDesc.vPosition = _float4(950.f, 0.f, 180.f, 1.f);
	//tDesc.fRadius = 4.f;
	//if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_SpawnBox"), strLayerTag, &tDesc)))
	//	return E_FAIL;

	//tDesc = {};
	//tDesc.eLevel = LEVEL_WORLD;
	//tDesc.eCreature = MONSTER_HILI_NORMAL;
	//tDesc.iSpawnNum = 3;
	//tDesc.fSpawnLengthToCenter = 4.f;
	//tDesc.vPosition = _float4(940.f, 0.f, 180.f, 1.f);
	//tDesc.fRadius = 4.f;
	//if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_SpawnBox"), strLayerTag, &tDesc)))
	//	return E_FAIL;

	//tDesc = {};
	//tDesc.eLevel = LEVEL_WORLD;
	//tDesc.eCreature = MONSTER_HILI_NORMAL;
	//tDesc.iSpawnNum = 1;
	//tDesc.fSpawnLengthToCenter = 4.f;
	//tDesc.vPosition = _float4(960.f, 0.f, 180.f, 1.f);
	//tDesc.fRadius = 4.f;
	//if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_SpawnBox"), strLayerTag, &tDesc)))
	//	return E_FAIL;

	/*CUpdraft::UPDRAFT_DESC tUpdraftDesc = {};
	tUpdraftDesc.vPosition = _float4(953.f, 0.f, 187.f, 1.f);
	tUpdraftDesc.vColliderSize = _float3(7.f, 20.f, 7.f);
	tUpdraftDesc.vColliderRadian = _float3(XMConvertToRadians(45.f), 0.f, 0.f);
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Updraft"), strLayerTag, &tUpdraftDesc)))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_World::Ready_Layer_NPC(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Paimon"), L"Layer_Paimon")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_AndriusSword"), strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Ambor"), L"Layer_Ambor")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Swan"), strLayerTag)))
		return E_FAIL;

#pragma region Common NPC
	CCommon_NPC::NPC_DESC NPCDesc;

	/* Knights */
	/*NPCDesc.vInitPos = { 451.2f, 10.f, 252.5f, 1.f };
	NPCDesc.fAngle = 90.f;
	NPCDesc.eObjectType = NPC_SWAN;
	NPCDesc.eNPCType = NPC_KNIGHTS;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag,  &NPCDesc)))
		return E_FAIL;*/

	NPCDesc.vInitPos = { 451.2f, 10.f, 259.6f, 1.f };
	NPCDesc.fAngle = 90.f;
	NPCDesc.eObjectType = NPC_LAWRENCE;
	NPCDesc.eNPCType = NPC_SOLIDER;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag, &NPCDesc)))
		return E_FAIL;

	NPCDesc.vInitPos = { 426.5f, 10.1f, 245.9f, 1.f };
	NPCDesc.fAngle = 90.f;
	NPCDesc.eObjectType = NPC_MILES;
	NPCDesc.eNPCType = NPC_WALK_STRAIGHT;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag, &NPCDesc)))
		return E_FAIL;

	NPCDesc.vInitPos = { 357.5f, 10.1f, 336.4f, 1.f };
	NPCDesc.fAngle = 180.f;
	NPCDesc.eObjectType = NPC_GUY;
	NPCDesc.eNPCType = NPC_SOLIDER;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag, &NPCDesc)))
		return E_FAIL;

	NPCDesc.vInitPos = { 159.f, 60.1f, 252.4f, 1.f };
	NPCDesc.fAngle = 90.f;
	NPCDesc.eObjectType = NPC_GUY;
	NPCDesc.eNPCType = NPC_SOLIDER;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag, &NPCDesc)))
		return E_FAIL;

	NPCDesc.vInitPos = { 350.4f, 20.1f, 218.8f, 1.f };
	NPCDesc.fAngle = 90.f;
	NPCDesc.eObjectType = NPC_RAYMOND;
	NPCDesc.eNPCType = NPC_WALK_STRAIGHT;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag, &NPCDesc)))
		return E_FAIL;

	/* Church */
	NPCDesc.vInitPos = { 238.2f, 50.1f, 257.2f, 1.f };
	NPCDesc.fAngle = 140.f;
	NPCDesc.eObjectType = NPC_JILLIANA;
	NPCDesc.eNPCType = NPC_TALK3;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag, &NPCDesc)))
		return E_FAIL;

	/* Adventure */
	NPCDesc.vInitPos = { 365.3f, 24.2f, 172.5f, 1.f };
	NPCDesc.fAngle = 40.f;
	NPCDesc.eObjectType = NPC_STANLEY;
	NPCDesc.eNPCType = NPC_HOLDARMHAND;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag, &NPCDesc)))
		return E_FAIL;

	NPCDesc.vInitPos = { 366.5f, 24.2f, 172.2f, 1.f };
	NPCDesc.fAngle = 330.f;
	NPCDesc.eObjectType = NPC_JACK;
	NPCDesc.eNPCType = NPC_REQUEST;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag, &NPCDesc)))
		return E_FAIL;

	NPCDesc.vInitPos = { 583.f, 10.f, 264.f, 1.f };
	NPCDesc.fAngle = 270.f;
	NPCDesc.eObjectType = NPC_LYNN;
	NPCDesc.eNPCType = NPC_STANDBY;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag, &NPCDesc)))
		return E_FAIL;

	/* Castle */
	NPCDesc.vInitPos = { 399.6f, 14.1f, 260.5f, 1.f };
	NPCDesc.fAngle = 180.f;
	NPCDesc.eObjectType = NPC_MARJORIE;
	NPCDesc.eNPCType = NPC_TALK1;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag, &NPCDesc)))
		return E_FAIL;

	NPCDesc.vInitPos = { 368.7f, 16.1f, 238.8f, 1.f };
	NPCDesc.fAngle = 0.f;
	NPCDesc.eObjectType = NPC_MARGARET;
	NPCDesc.eNPCType = NPC_WALK_STRAIGHT;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag, &NPCDesc)))
		return E_FAIL;

	NPCDesc.vInitPos = { 368.f, 16.1f, 238.8f, 1.f };
	NPCDesc.fAngle = 0.f;
	NPCDesc.eObjectType = NPC_NORMA;
	NPCDesc.eNPCType = NPC_WALK_STRAIGHT;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag, &NPCDesc)))
		return E_FAIL;

	NPCDesc.vInitPos = { 388.2f, 16.3f, 244.5f, 1.f };
	NPCDesc.fAngle = 0.f;
	NPCDesc.eObjectType = NPC_SARA;
	NPCDesc.eNPCType = NPC_AKIMBO;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag, &NPCDesc)))
		return E_FAIL;

	NPCDesc.vInitPos = { 434.9f, 10.1f, 236.5f, 1.f };
	NPCDesc.fAngle = 270.f;
	NPCDesc.eObjectType = NPC_WAGNER;
	NPCDesc.eNPCType = NPC_TALK1;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag, &NPCDesc)))
		return E_FAIL;

	NPCDesc.vInitPos = { 432.3f, 10.2f, 268.5f, 1.f };
	NPCDesc.fAngle = 180.f;
	NPCDesc.eObjectType = NPC_QUINN;
	NPCDesc.eNPCType = NPC_TALK3;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag, &NPCDesc)))
		return E_FAIL;

	NPCDesc.vInitPos = { 382.8f, 16.4f, 272.8f, 1.f };
	NPCDesc.fAngle = 180.f;
	NPCDesc.eObjectType = NPC_BLANCHE;
	NPCDesc.eNPCType = NPC_TALK3;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag, &NPCDesc)))
		return E_FAIL;

	NPCDesc.vInitPos = { 431.9f, 10.1f, 266.9f, 1.f };
	NPCDesc.fAngle = 0.f;
	NPCDesc.eObjectType = NPC_BEATRICE;
	NPCDesc.eNPCType = NPC_AKIMBO;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag, &NPCDesc)))
		return E_FAIL;

	NPCDesc.vInitPos = { 399.2f, 14.2f, 247.7f, 1.f };
	NPCDesc.fAngle = 220.f;
	NPCDesc.eObjectType = NPC_EURY;
	NPCDesc.eNPCType = NPC_HOLDARM;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag, &NPCDesc)))
		return E_FAIL;

	NPCDesc.vInitPos = { 413.5f, 12.2f, 272.7f, 1.f };
	NPCDesc.fAngle = 90.f;
	NPCDesc.eObjectType = NPC_DONNA;
	NPCDesc.eNPCType = NPC_STANDBY;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag, &NPCDesc)))
		return E_FAIL;

	NPCDesc.vInitPos = { 378.1f, 16.1f, 250.2f, 1.f };
	NPCDesc.fAngle = 310.f;
	NPCDesc.eObjectType = NPC_ANTHONY;
	NPCDesc.eNPCType = NPC_HOLDARM;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag, &NPCDesc)))
		return E_FAIL;

	NPCDesc.vInitPos = { 392.8f, 14.2f, 314.8f, 1.f };
	NPCDesc.fAngle = 180.f;
	NPCDesc.eObjectType = NPC_CHARLES;
	NPCDesc.eNPCType = NPC_AKIMBO;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag, &NPCDesc)))
		return E_FAIL;

	NPCDesc.vInitPos = { 357.5f, 20.3f, 265.9f, 1.f };
	NPCDesc.fAngle = 30.f;
	NPCDesc.eObjectType = NPC_ANNA;
	NPCDesc.eNPCType = NPC_STANDBY;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag, &NPCDesc)))
		return E_FAIL;

	//NPCDesc.vInitPos = { 356.f, 20.3f, 203.1f, 1.f };
	//NPCDesc.fAngle = 270.f;
	//NPCDesc.eObjectType = NPC_GLORY;
	//NPCDesc.eNPCType = NPC_SIT;
	//if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag, &NPCDesc)))
	//	return E_FAIL;

	NPCDesc.vInitPos = { 241.2f, 50.1f, 254.f, 1.f };
	NPCDesc.fAngle = 310.f;
	NPCDesc.eObjectType = NPC_SUSIE;
	NPCDesc.eNPCType = NPC_CLAP;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag, &NPCDesc)))
		return E_FAIL;

	NPCDesc.vInitPos = { 298.1f, 30.2f, 165.6f, 1.f };
	NPCDesc.fAngle = 310.f;
	NPCDesc.eObjectType = NPC_MICHELLE;
	NPCDesc.eNPCType = NPC_STANDBY;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag, &NPCDesc)))
		return E_FAIL;

	NPCDesc.vInitPos = { 207.8f, 50.1f, 239.4f, 1.f };
	NPCDesc.fAngle = 0.f;
	NPCDesc.eObjectType = NPC_ARAMIS;
	NPCDesc.eNPCType = NPC_WALK_STRAIGHT;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag, &NPCDesc)))
		return E_FAIL;

	NPCDesc.vInitPos = { 157.9f, 60.1f, 272.7f, 1.f };
	NPCDesc.fAngle = 120.f;
	NPCDesc.eObjectType = NPC_ALBERT;
	NPCDesc.eNPCType = NPC_STANDBY;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag, &NPCDesc)))
		return E_FAIL;

	NPCDesc.vInitPos = { 243.f, 50.2f, 256.f, 1.f };
	NPCDesc.fAngle = 270.f;
	NPCDesc.eObjectType = NPC_QUINN;
	NPCDesc.eNPCType = NPC_PRAY;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag, &NPCDesc)))
		return E_FAIL;

	NPCDesc.vInitPos = { 375.7f, 16.1f, 271.6f, 1.f };
	NPCDesc.fAngle = 180.f;
	NPCDesc.eObjectType = NPC_TIMAEUS;
	NPCDesc.eNPCType = NPC_TALK2;
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_CommonNPC"), strLayerTag, &NPCDesc)))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CLevel_World::Ready_Layer_Icon(const wstring& strLayerTag)
{
	COrtho::ORTHO_DESC tOrthoDesc = {};
	{
		tOrthoDesc.eTextureLevel = LEVEL_STATIC;
		tOrthoDesc.strTextureTag = L"";

		tOrthoDesc.fPosition = { 0.f, 0.f };
		tOrthoDesc.fSize = { static_cast<_float>(g_iWinSizeX), static_cast<_float>(g_iWinSizeY) };
	}

	if (m_pGameInstance->Find_Object(LEVEL_STATIC, L"Layer_UIManager") == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_UI_Manager"), TEXT("Layer_UIManager"))))
			return E_FAIL;
	}
	if (m_pGameInstance->Find_Object(LEVEL_STATIC, L"Layer_QuestManager") == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Quest_Manager"), TEXT("Layer_QuestManager"))))
			return E_FAIL;
	}
	if (m_pGameInstance->Find_Object(LEVEL_STATIC, L"Layer_Inventory") == nullptr)
	{
		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_Inventory"), L"Layer_Inventory", &tOrthoDesc)))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CLevel_World::Ready_Layer_UI(const wstring& strLayerTag)
{


	return S_OK;
}

HRESULT CLevel_World::Ready_Layer_TelePort(const wstring& strLayerTag)
{
	CTelePort::TELEPORT_DESC TelePortDesc{};
	TelePortDesc.eTelePortId = CTelePort::TELEPORT_CASTLE;
	TelePortDesc.vInitPos = { 359.f, 20.4f, 273.5f, 1.f }; /* 몬드성 광장 */
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_TelePort"), strLayerTag, &TelePortDesc)))
		return E_FAIL;

	TelePortDesc.eTelePortId = CTelePort::TELEPORT_OPENWORLD;
	TelePortDesc.vInitPos = { 566.2f, 10.f, 272.2f, 1.f }; /* 몬드성 다리 */
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_TelePort"), strLayerTag, &TelePortDesc)))
		return E_FAIL;

	TelePortDesc.eTelePortId = CTelePort::TELEPORT_DVALIN;
	TelePortDesc.vInitPos = { 416.3f, 30.f, 670.3f, 1.f }; /* 몬드성 
	*/
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_TelePort"), strLayerTag, &TelePortDesc)))
		return E_FAIL;

	TelePortDesc.eTelePortId = CTelePort::TELEPORT_GATE;
	TelePortDesc.vInitPos = { 720.5f, 10.f, 181.6f, 1.f }; /* 몬드 비경 */
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_TelePort"), strLayerTag, &TelePortDesc)))
		return E_FAIL;

	TelePortDesc.eTelePortId = CTelePort::TELEPORT_EFFIGYFIRE;
	TelePortDesc.vInitPos = { 790.2f, 10.f, 425.7f, 1.f }; /* 몬드 무상의불 */
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_TelePort"), strLayerTag, &TelePortDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_World::Ready_Layer_LocationCollider(const wstring& strLayerTag)
{
	CLocation::LOCATION_DESC tDesc = {};
	tDesc.eLocation = LOCATION_MOND_CASTLE;
	tDesc.fRadius = 5.f;
	tDesc.vSpawnPosition = _float4(437.635f, 10.1f, 255.708f, 1.f);
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Location"), strLayerTag, &tDesc)))
		return E_FAIL;

	tDesc.eLocation = LOCATION_MOND_FIELD;
	tDesc.fRadius = 5.f;
	tDesc.vSpawnPosition = _float4(454.669f, 10.f, 256.137f, 1.f);
	if (FAILED(m_pGameInstance->Add_Object_Layer(CURRENT_LEVEL, TEXT("Prototype_GameObject_Location"), strLayerTag, &tDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_World::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Level()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_World::PreviousTick(_float fTimeDelta)
{
	if (FAILED(__super::PreviousTick(fTimeDelta)))
		return E_FAIL;

	/* Cursor */
	if (m_pGameInstance->Key_Down(VK_MENU))
	{
		m_pGameInstance->Clip_Mouse();
		m_pGameInstance->FreeFix_Mouse();
	}
	if (m_pGameInstance->Key_Up(VK_MENU))
	{
		m_pGameInstance->Fix_Mouse();
		m_pGameInstance->FreeClip_Mouse();
	}

	return S_OK;
}

HRESULT CLevel_World::Tick(_float fTimeDelta)
{
	if (FAILED(__super::Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_World::LateTick(_float fTimeDelta)
{
	if (FAILED(__super::LateTick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_World::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CLevel_World* CLevel_World::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_World* pLevel = new CLevel_World(pDevice, pContext);

	if (FAILED(pLevel->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CLevel_World"));
		Safe_Release(pLevel);
	}

	return pLevel;
}

void CLevel_World::Free()
{
	__super::Free();

	m_pGameInstance->StopAll();
}
