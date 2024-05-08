#include "stdafx.h"
#include "ElementalReactor_Manager.h"

#include "GameInstance.h"

CElementalReactor_Manager::CElementalReactor_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CElementalReactor_Manager::CElementalReactor_Manager(const CElementalReactor_Manager& rhs)
	: CGameObject(rhs)
{
	m_pReactorPrototype = rhs.m_pReactorPrototype;
	Safe_AddRef(m_pReactorPrototype);

	m_pDungeonDoorPrototype = rhs.m_pDungeonDoorPrototype;
	Safe_AddRef(m_pDungeonDoorPrototype);

	m_pDungeonPlatformPrototype = rhs.m_pDungeonPlatformPrototype;
	Safe_AddRef(m_pDungeonPlatformPrototype);
}

HRESULT CElementalReactor_Manager::Ready_Dungeon_Temple()
{
	return S_OK;
}

HRESULT CElementalReactor_Manager::Ready_Dungeon_StrongHold()
{
	CElementalReactor::REACTOR_DESC		tReactorDesc = { };

	CDungeonDoor::DDOOR_DESC			tDungeonDoorDesc = { };
	CDungeonPlatform::DPLATFORM_DESC	tDungeonPlatformDesc = { };

	/* Reactor - 1 */
	REACTOR_CONTAINER tReactorContainer1 = { };
	tReactorContainer1.iType = REACTOR_CONTAINER::RC_DOOR;

	tReactorDesc.eReactElement = ELEMENT_FIRE;
	tReactorDesc.eReactorLevel = m_eLevelID;
	tReactorDesc.vReactorPosition = _float4{ -370.1770f, -34.5900f, -21.8720f, 1.0000f };
	CElementalReactor* pReactor_1 = static_cast<CElementalReactor*>(m_pReactorPrototype->Clone(&tReactorDesc));

	tDungeonDoorDesc.eDoorLevel = m_eLevelID;
	tDungeonDoorDesc.eTransformDesc.vScale			= _float4{ 0.5f, 0.4f, 0.2f, 0.f };
	tDungeonDoorDesc.eTransformDesc.vRotation		= _float4{ 0.0f, 90.f, 0.0f, 0.f };
	tDungeonDoorDesc.eTransformDesc.vTranslation	= _float4{ -375.0470f, -38.6810f, -34.7270f, 1.f };
	CDungeonDoor* pDungeonDoor_1 = static_cast<CDungeonDoor*>(m_pDungeonDoorPrototype->Clone(&tDungeonDoorDesc));

	tReactorContainer1.pReactor = pReactor_1;
	tReactorContainer1.pDungeonDoor = pDungeonDoor_1;
	m_ElementalReactors.push_back(tReactorContainer1);

	/* Reactor - 2 */
	REACTOR_CONTAINER tReactorContainer2 = { };
	tReactorContainer2.iType = REACTOR_CONTAINER::RC_DOOR | REACTOR_CONTAINER::RC_PLATFORM;
	
	tReactorDesc.eReactElement = ELEMENT_WATER;
	tReactorDesc.eReactorLevel = m_eLevelID;
	tReactorDesc.vReactorPosition = _float4{ -387.8800f, -34.500f, -35.2940f, 1.0000f };
	CElementalReactor* pReactor_2 = static_cast<CElementalReactor*>(m_pReactorPrototype->Clone(&tReactorDesc));

	tDungeonDoorDesc.eDoorLevel = m_eLevelID;
	tDungeonDoorDesc.eTransformDesc.vScale			= _float4{ 0.5f, 0.4f, 0.2f, 0.f };
	tDungeonDoorDesc.eTransformDesc.vRotation		= _float4{ 0.0f, 0.0f, 0.0f, 0.f };
	tDungeonDoorDesc.eTransformDesc.vTranslation	= _float4{ -380.8800f, -38.5900f, -16.2940f, 1.f };
	CDungeonDoor* pDungeonDoor_2 = static_cast<CDungeonDoor*>(m_pDungeonDoorPrototype->Clone(&tDungeonDoorDesc));
	
	tDungeonPlatformDesc.ePlatformLevel = m_eLevelID;
	// tDungeonPlatformDesc.eTransformDesc.vScale			= _float4{ 1.5f, 1.5f, 1.4f, 0.f };
	tDungeonPlatformDesc.eTransformDesc.vScale			= _float4{ 1.f, 1.f, 1.f, 0.f };
	tDungeonPlatformDesc.eTransformDesc.vRotation		= _float4{ 0.0f, 0.0f, 0.0f, 0.f };
	tDungeonPlatformDesc.eTransformDesc.vTranslation	= _float4{ -379.8070f, -42.9070f, -27.0610f, 1.f };
	tDungeonPlatformDesc.vLiftTranslation				= _float4{ -379.8070f, -37.0800f, -27.0610f, 1.f };
	CDungeonPlatform* pDungeonPlatform_2_1 = static_cast<CDungeonPlatform*>(m_pDungeonPlatformPrototype->Clone(&tDungeonPlatformDesc));

	tDungeonPlatformDesc.ePlatformLevel = m_eLevelID;
	tDungeonPlatformDesc.eTransformDesc.vTranslation	= _float4{ -379.8070f, -42.9070f, -20.5980f, 1.f };
	tDungeonPlatformDesc.vLiftTranslation				= _float4{ -379.8070f, -37.0800f, -20.5980f, 1.f };
	CDungeonPlatform* pDungeonPlatform_2_2 = static_cast<CDungeonPlatform*>(m_pDungeonPlatformPrototype->Clone(&tDungeonPlatformDesc));
	
	tReactorContainer2.pReactor = pReactor_2;
	tReactorContainer2.pDungeonDoor = pDungeonDoor_2;
	tReactorContainer2.pDungeonPlatform.push_back(pDungeonPlatform_2_1);
	tReactorContainer2.pDungeonPlatform.push_back(pDungeonPlatform_2_2);
	m_ElementalReactors.push_back(tReactorContainer2);

	/* Reactor - 3 */
	REACTOR_CONTAINER tReactorContainer3 = { };
	tReactorContainer3.iType = REACTOR_CONTAINER::RC_PLATFORM;
	
	tReactorDesc.eReactElement = ELEMENT_EARTH;
	tReactorDesc.eReactorLevel = m_eLevelID;
	tReactorDesc.vReactorPosition = _float4{ -378.5400f, -34.8600f, -4.3930f, 1.0000f };
	CElementalReactor* pReactor_3 = static_cast<CElementalReactor*>(m_pReactorPrototype->Clone(&tReactorDesc));

	tDungeonPlatformDesc.ePlatformLevel = m_eLevelID;
	// tDungeonPlatformDesc.eTransformDesc.vScale			= _float4{ 1.3f, 1.5f, 1.5f, 0.f };
	tDungeonPlatformDesc.eTransformDesc.vScale			= _float4{ 1.f, 1.f, 1.f, 0.f };
	tDungeonPlatformDesc.eTransformDesc.vRotation		= _float4{ 0.0f, 0.0f, 0.0f, 0.f };
	tDungeonPlatformDesc.eTransformDesc.vTranslation	= _float4{ -393.4210f, -42.8830f, -5.484f, 1.f };
	tDungeonPlatformDesc.vLiftTranslation				= _float4{ -393.4210f, -37.0850f, -5.484f, 1.f };
	CDungeonPlatform* pDungeonPlatform_3_1 = static_cast<CDungeonPlatform*>(m_pDungeonPlatformPrototype->Clone(&tDungeonPlatformDesc));

	tReactorContainer3.pReactor = pReactor_3;
	tReactorContainer3.pDungeonPlatform.push_back(pDungeonPlatform_3_1);
	m_ElementalReactors.push_back(tReactorContainer3);

	return S_OK;
}

HRESULT CElementalReactor_Manager::Initialize_Prototype()
{
	m_pReactorPrototype				= CElementalReactor::Create(m_pDevice, m_pContext);
	if (m_pReactorPrototype == nullptr)
		return E_FAIL;

	m_pDungeonDoorPrototype			= CDungeonDoor::Create(m_pDevice, m_pContext);
	if (m_pDungeonDoorPrototype == nullptr)
		return E_FAIL;

	m_pDungeonPlatformPrototype		= CDungeonPlatform::Create(m_pDevice, m_pContext);
	if (m_pDungeonPlatformPrototype == nullptr)
		return E_FAIL;

	return S_OK;
}

HRESULT CElementalReactor_Manager::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	REACTORMGR_DESC* pReactorMgrDesc = static_cast<REACTORMGR_DESC*>(pArg);

	m_eLevelID = pReactorMgrDesc->eReactorLevel;

	switch (m_eLevelID)
	{
	case LEVEL_TEMPLE_LION:
		if (FAILED(CElementalReactor_Manager::Ready_Dungeon_Temple()))
			return E_FAIL;
		break;

	case LEVEL_STRONGHOLD_ABYSSMAGE:
		if (FAILED(CElementalReactor_Manager::Ready_Dungeon_StrongHold()))
			return E_FAIL;
		break;

	default:
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CElementalReactor_Manager::PreviousTick(_float fTimeDelta)
{
	for (auto& iter : m_ElementalReactors)
	{
		iter.pReactor->PreviousTick(fTimeDelta);

		if ((iter.iType & REACTOR_CONTAINER::RC_DOOR) != FALSE)
		{
			if (iter.pReactor->Get_Reacted())
			{
				iter.pDungeonDoor->Set_Reacted(TRUE);
			}
			iter.pDungeonDoor->PreviousTick(fTimeDelta);
		}

		if ((iter.iType & REACTOR_CONTAINER::RC_PLATFORM) != FALSE)
		{
			for (auto& Platform : iter.pDungeonPlatform)
			{
				if (iter.pReactor->Get_Reacted())
				{
					Platform->Set_Reacted(TRUE);
				}
				Platform->PreviousTick(fTimeDelta);
			}
		}
	}

	return S_OK;
}

HRESULT CElementalReactor_Manager::Tick(_float fTimeDelta)
{
	for (auto& iter : m_ElementalReactors)
	{
		iter.pReactor->Tick(fTimeDelta);

		if ((iter.iType & REACTOR_CONTAINER::RC_DOOR) != FALSE)
			iter.pDungeonDoor->Tick(fTimeDelta);

		if ((iter.iType & REACTOR_CONTAINER::RC_PLATFORM) != FALSE)
		{
			for (auto& Platform : iter.pDungeonPlatform)
				Platform->Tick(fTimeDelta);
		}
	}

	return S_OK;
}

HRESULT CElementalReactor_Manager::LateTick(_float fTimeDelta)
{
	for (auto& iter : m_ElementalReactors)
	{
		iter.pReactor->LateTick(fTimeDelta);

		if ((iter.iType & REACTOR_CONTAINER::RC_DOOR) != FALSE)
			iter.pDungeonDoor->LateTick(fTimeDelta);

		if ((iter.iType & REACTOR_CONTAINER::RC_PLATFORM) != FALSE)
		{
			for (auto& Platform : iter.pDungeonPlatform)
				Platform->LateTick(fTimeDelta);
		}
	}

	return S_OK;
}

HRESULT CElementalReactor_Manager::Render()
{
	return S_OK;
}

CElementalReactor_Manager* CElementalReactor_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CElementalReactor_Manager* pInstance = new CElementalReactor_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CElementalReactor_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CElementalReactor_Manager::Clone(void* pArg)
{
	CElementalReactor_Manager* pInstance = new CElementalReactor_Manager(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CElementalReactor_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CElementalReactor_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pReactorPrototype);
	Safe_Release(m_pDungeonDoorPrototype);
	Safe_Release(m_pDungeonPlatformPrototype);

	for (auto& iter : m_ElementalReactors)
	{
		Safe_Release(iter.pReactor);
		Safe_Release(iter.pDungeonDoor);

		for (auto& Platform : iter.pDungeonPlatform)
			Safe_Release(Platform);
	}
}
