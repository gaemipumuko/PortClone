#include "stdafx.h"
#include "GearOperator_Manager.h"

#include "SpawnBox.h"

#include "GameInstance.h"

CGearOperator_Manager::CGearOperator_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CGearOperator_Manager::CGearOperator_Manager(const CGearOperator_Manager& rhs)
    : CGameObject(rhs)
{
	m_pGearPrototype = rhs.m_pGearPrototype;
	Safe_AddRef(m_pGearPrototype);

	m_pDungeonPlatformPrototype = rhs.m_pDungeonPlatformPrototype;
	Safe_AddRef(m_pDungeonPlatformPrototype);
}

HRESULT CGearOperator_Manager::Ready_Dungeon_Temple()
{
    return S_OK;
}

HRESULT CGearOperator_Manager::Ready_Dungeon_StrongHold()
{
	CGameObject*						pSpawnBox = { nullptr };
	CGearOperator::GEAR_DESC			tGearDesc = { };
	CSpawnBox::MONSTER_INFO				tMonsterInfo = { };
	CSpawnBox::SPAWNER_DESC				tSpawnerDesc = { };
	CDungeonPlatform::DPLATFORM_DESC	tDungeonPlatformDesc = { };
	vector<CSpawnBox::MONSTER_INFO>		MonsterInfos;

	/* GearOperator - 1(Spawner) */
	MonsterInfos.clear();
	tMonsterInfo = { };
	tMonsterInfo.eCreatureId = MONSTER_SLIME_WATER;

	tMonsterInfo.vSpawnPosition = { -414.0000f, -36.2489f, -6.5000f, 1.f };
	MonsterInfos.push_back(tMonsterInfo);

	tMonsterInfo.vSpawnPosition = { -414.0000f, -36.2489f, -10.0000f, 1.f };
	MonsterInfos.push_back(tMonsterInfo);

	tMonsterInfo.vSpawnPosition = { -417.0000f, -36.2489f, -6.5000f, 1.f };
	MonsterInfos.push_back(tMonsterInfo);

	tMonsterInfo.vSpawnPosition = { -417.0000f, -36.2489f, -10.0000f, 1.f };
	MonsterInfos.push_back(tMonsterInfo);

	tSpawnerDesc				= { };
	tSpawnerDesc.iLevel			= m_eLevelID;
	tSpawnerDesc.fSpawnRange	= 5.f;
	tSpawnerDesc.vSpawnerPos	= _float4{ -410.4520f, -37.1130f, -8.4300f, 1.f };
	tSpawnerDesc.pMonsterInfo	= &MonsterInfos;
	
	if (FAILED(m_pGameInstance->Add_Object_Layer(m_eLevelID, TEXT("Prototype_GameObject_SpawnBox"), TEXT("Layer_SpawnBox"), 
		&tSpawnerDesc, &pSpawnBox)))
		return E_FAIL;

	/* GearOperator - 1 */
	GEAROPER_CONTAINER	tGearOperatorContainer1 = { };

	tGearDesc.eOperatorType		= CGearOperator::OPERATOR_TYPE::OPR_WINDFIELD;
	tGearDesc.eChallengeType	= CGearOperator::CHALLENGE_TYPE::CHL_SEALEDAUTO;
	tGearDesc.bSealed			= TRUE;
	tGearDesc.eGearLevel		= m_eLevelID;
	tGearDesc.strSpawnBoxTag	= static_cast<CSpawnBox*>(pSpawnBox)->CSpawnBox::Get_SpawnerTag();
	tGearDesc.strInteraction	= L"윈드필드 활성화";
	tGearDesc.vGearRotation		= _float4{ 0.0f, 90.f, 0.0f, 0.0f };
	tGearDesc.vGearPosition		= _float4{ -410.4520f, -37.1130f, -8.4300f, 1.f };
	CGearOperator* pGear_1		= static_cast<CGearOperator*>(m_pGearPrototype->Clone(&tGearDesc));

	tGearOperatorContainer1.pGearOperator = pGear_1;
	m_GearOperators.push_back(tGearOperatorContainer1);
	
	/* GearOperator - 2(Spawner) */
	MonsterInfos.clear();
	tMonsterInfo = { };
	tMonsterInfo.eCreatureId = MONSTER_ABYSS_MAGE_ELEC;

	tMonsterInfo.vSpawnPosition = { -451.3700f, -30.3000f, -16.0610f, 1.f };
	MonsterInfos.push_back(tMonsterInfo);

	/* GearOperator - 2(Platform) */
	tDungeonPlatformDesc.ePlatformLevel = m_eLevelID;
	// tDungeonPlatformDesc.eTransformDesc.vScale			= _float4{ 1.3f, 1.0f, 1.3f, 0.f };
	tDungeonPlatformDesc.eTransformDesc.vScale			= _float4{ 1.f, 1.f, 1.f, 0.f };
	tDungeonPlatformDesc.eTransformDesc.vRotation		= _float4{ 0.0f, 0.0f, 0.0f, 0.f };
	tDungeonPlatformDesc.eTransformDesc.vTranslation	= _float4{ -466.6690f, -33.2050f, -16.1180f, 1.f };
	tDungeonPlatformDesc.vLiftTranslation				= _float4{ -466.6690f, -45.2050f, -16.1180f, 1.f };
	CDungeonPlatform* pDungeonPlatform_2 = static_cast<CDungeonPlatform*>(m_pDungeonPlatformPrototype->Clone(&tDungeonPlatformDesc));

	/* GearOperator - 2 */
	GEAROPER_CONTAINER	tGearOperatorContainer2 = { };

	tGearDesc.eOperatorType		= CGearOperator::OPERATOR_TYPE::OPR_PLATFORM;
	tGearDesc.eChallengeType	= CGearOperator::CHALLENGE_TYPE::CHL_INTERACTION;
	tGearDesc.bSealed			= FALSE;
	tGearDesc.eGearLevel		= m_eLevelID;
	tGearDesc.strInteraction	= L"시련 시작";
	tGearDesc.SpawnBoxDesc		= MonsterInfos;
	tGearDesc.pPlatform			= pDungeonPlatform_2;
	tGearDesc.vGearRotation		= _float4{ 0.0f, 90.f, 0.0f, 0.0f };
	tGearDesc.vGearPosition		= _float4{ -463.5970f, -31.4020f, -21.1450f, 1.f };
	CGearOperator* pGear_2 = static_cast<CGearOperator*>(m_pGearPrototype->Clone(&tGearDesc));

	tGearOperatorContainer2.pGearOperator = pGear_2;
	m_GearOperators.push_back(tGearOperatorContainer2);

    return S_OK;
}

HRESULT CGearOperator_Manager::Initialize_Prototype()
{
	m_pGearPrototype = CGearOperator::Create(m_pDevice, m_pContext);
	if (m_pGearPrototype == nullptr)
		return E_FAIL;

	m_pDungeonPlatformPrototype = CDungeonPlatform::Create(m_pDevice, m_pContext);
	if (m_pDungeonPlatformPrototype == nullptr)
		return E_FAIL;

    return S_OK;
}

HRESULT CGearOperator_Manager::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	GEARMGR_DESC* pGearMgrDesc = static_cast<GEARMGR_DESC*>(pArg);

	m_eLevelID = pGearMgrDesc->eGearLevel;

	switch (m_eLevelID)
	{
	case LEVEL_TEMPLE_LION:
		if (FAILED(CGearOperator_Manager::Ready_Dungeon_Temple()))
			return E_FAIL;
		break;

	case LEVEL_STRONGHOLD_ABYSSMAGE:
		if (FAILED(CGearOperator_Manager::Ready_Dungeon_StrongHold()))
			return E_FAIL;
		break;

	default:
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGearOperator_Manager::PreviousTick(_float fTimeDelta)
{
	for (auto& iter : m_GearOperators)
	{
		iter.pGearOperator->PreviousTick(fTimeDelta);
	}

    return S_OK;
}

HRESULT CGearOperator_Manager::Tick(_float fTimeDelta)
{
	for (auto& iter : m_GearOperators)
	{
		iter.pGearOperator->Tick(fTimeDelta);
	}

    return S_OK;
}

HRESULT CGearOperator_Manager::LateTick(_float fTimeDelta)
{
	for (auto& iter : m_GearOperators)
	{
		iter.pGearOperator->LateTick(fTimeDelta);
	}

    return S_OK;
}

HRESULT CGearOperator_Manager::Render()
{
    return S_OK;
}

CGearOperator_Manager* CGearOperator_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGearOperator_Manager* pInstance = new CGearOperator_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CGearOperator_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGearOperator_Manager::Clone(void* pArg)
{
	CGearOperator_Manager* pInstance = new CGearOperator_Manager(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CGearOperator_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGearOperator_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pGearPrototype);
	Safe_Release(m_pDungeonPlatformPrototype);

	for (auto& iter : m_GearOperators)
	{
		Safe_Release(iter.pGearOperator);
	}
}
