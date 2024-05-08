#include "stdafx.h"
#include "Dungeon.h"
#include "Renderer.h"
#include "GameInstance.h"
#include "DungeonElement.h"

CDungeon::CDungeon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDeciderObject(pDevice, pContext)
{
}

CDungeon::CDungeon(const CDungeon& rhs)
	: CDeciderObject(rhs)
{
}

HRESULT CDungeon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDungeon::Initialize_Clone(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	m_eRenderType = RENDER_TYPE::RTYPE_GAMEPLAY;

	DUNGEON_DESC* pDungeonDesc = (DUNGEON_DESC*)pArg;
	{
		m_eLevel = pDungeonDesc->eLevel;
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Initialize_Dungeon()))
		return E_FAIL;

	if (m_eLevel == LEVEL_STORMTERROR)
	{
		// Set_ElementPassID(0, VTXMESH_DVALIN_PLATFORM_DESTROY);
		// Set_ElementPassID(2, VTXMESH_DVALIN_PLATFORM_DESTROY);
	}

	return S_OK;
}

HRESULT CDungeon::PreviousTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
	}

	return S_OK;
}

HRESULT CDungeon::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
	}

	return S_OK;
}

HRESULT CDungeon::LateTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RG_TERRAIN, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RG_SHADOW, this);
	}

	return S_OK;
}

HRESULT CDungeon::Render()
{
	for (auto& pElement : m_DungeonElements)
	{
		if (FAILED(pElement->Render()))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CDungeon::Render_Shadow(_float4x4* pLightViewProj, _int iNumArray)
{
	for (auto& pElement : m_DungeonElements)
	{
		if (FAILED(pElement->Render_Shadow(pLightViewProj, iNumArray)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CDungeon::Set_ElementPassID(_uint iIndex, PASSID_VTXMESH ePassID)
{
	if (0 > ePassID)
		return E_FAIL;

	return m_DungeonElements[iIndex]->Set_PassID(ePassID);
}

void CDungeon::Reset_ElementPassID()
{
	for (auto& pElement : m_DungeonElements)
		pElement->Set_PassID(VTXMESH_DEFALUT);
}

_bool CDungeon::isCollaps(_uint iIndex)
{
	if (m_DungeonElements[iIndex]->Get_PassID() == VTXMESH_DVALIN_PLATFORM_DESTROY)
		return TRUE;

	return FALSE;
}

HRESULT CDungeon::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDungeon::Initialize_Dungeon()
{
	if (FAILED(Loading_Map()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDungeon::Loading_Map()
{
	wstring szDataPath;

	switch (m_eLevel)
	{
	case Client::LEVEL_TEMPLE_LION:
		szDataPath = TEXT("../../Reference/Resources/Data/Map/TempleData.dat");
		break;
	case Client::LEVEL_STRONGHOLD_ABYSSMAGE:
		szDataPath = TEXT("../../Reference/Resources/Data/Map/StrongHoldData.dat");
		break;
	case Client::LEVEL_QUEST:
		szDataPath = TEXT("../../Reference/Resources/Data/Map/QuestData.dat");
		break;
	case Client::LEVEL_STORMTERROR:
		szDataPath = TEXT("../../Reference/Resources/Data/Map/DvalinData.dat");
		break;
	}

	ifstream InFile(szDataPath, ios::binary);

	if (false == InFile.is_open())
		return E_FAIL;

	_int iNumGameObject = -1;

	InFile.read(reinterpret_cast<char*>(&iNumGameObject), sizeof(_int));

	for (_int i = 0; i < iNumGameObject; i++)
	{
		WORLD_ELEMENT_TYPE eType = WORLD_ELEMENT_END;
		InFile.read(reinterpret_cast<_char*>(&eType), sizeof(WORLD_ELEMENT_TYPE));

		_float4x4 vWorldMatrix;
		InFile.read(reinterpret_cast<_char*>(&vWorldMatrix), sizeof(_float4x4));

		_char chModelTag[MAX_PATH];
		InFile.read(reinterpret_cast<_char*>(&chModelTag), MAX_PATH);

		wstring strModelTag = ConvertCToWC(chModelTag);

		if (ELEMENT_END == eType || true == strModelTag.empty())
			continue;

		CDungeonElement::ELEMENT_DESC ElementDesc = {};
		{
			ElementDesc.eLevel = m_eLevel;
			ElementDesc.eType = eType;
			ElementDesc.vWorldMatrix = vWorldMatrix;
			ElementDesc.strModelTag = strModelTag;
		}

		CDungeonElement* pElement = CDungeonElement::Create(m_pDevice, m_pContext, &ElementDesc);

		if (nullptr == pElement)
			return E_FAIL;

		m_DungeonElements.push_back(pElement);
	}

	InFile.close();

	return S_OK;
}

CDungeon* CDungeon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDungeon* pInstance = new CDungeon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CDungeon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDungeon::Clone(void* pArg)
{
	CDungeon* pInstance = new CDungeon(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CDungeon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDungeon::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);

	for (auto& pElement : m_DungeonElements)
		Safe_Release(pElement);

	m_DungeonElements.clear();
}
