#include "stdafx.h"
#include "World.h"
#include "Terrain.h"
#include "WorldElement.h"
#include "Renderer.h"
#include "GameInstance.h"
#include "Instancing_Element.h"
#include "Water.h"
#include "SpawnBox.h"
#include "Camera_Main.h"
#include "Player.h"

CWorld::CWorld(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDeciderObject(pDevice, pContext)
{
}

CWorld::CWorld(const CWorld& rhs)
	: CDeciderObject(rhs)
{
}

HRESULT CWorld::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWorld::Initialize_Clone(void* pArg)
{
	m_byFlag		= WT_TERRAIN | WT_ELEMENT | WT_INSTANCING | WT_WATER;
	//m_byFlag		= WT_TERRAIN;
	m_eRenderType	= RENDER_TYPE::RTYPE_GAMEPLAY;

	if (FAILED(Add_Components()))
		return E_FAIL;
	
	if (FAILED(Initialize_World()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWorld::PreviousTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
	}

	return S_OK;
}

HRESULT CWorld::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		if (m_pGameInstance->Key_Down('Z'))
			m_bDebugRender = !m_bDebugRender;
	}

	if ((m_byFlag & WT_WATER) != FALSE)
	{
		for (auto& pWater : m_Waters)
		{
			if (FAILED(pWater->Simulate_Wave(fTimeDelta)))
				return E_FAIL;
		}
	}

	if ((m_byFlag & WT_INSTANCING) != FALSE)
	{
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
		
		if (nullptr != pPlayer) 
		{
			_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

			CTransform* pPlayerTransform =
				static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

			if (nullptr != pPlayerTransform)
			{
				_float4 vPosition;
				XMStoreFloat4(&vPosition, pPlayerTransform->Get_State(CTransform::STATE_POSITION));

				m_pInstancing_Elements->Update(&vPosition);
			}
		}
	}

	return S_OK;
}

HRESULT CWorld::LateTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_RenderValidity() == S_OK)
	{
		if ((m_byFlag & WT_TERRAIN) != FALSE)
		{
			if (FAILED(m_pWorldTerrain->Invalidate_Culling()))
				return E_FAIL;
		}

		if ((m_byFlag & WT_WATER) != FALSE)
		{
			for (auto& pWater : m_Waters)
			{
				if (FAILED(pWater->Invalidate_Culling()))
					return E_FAIL;
			}
		}

		if (FAILED(Invalidate_Culling()))
			return E_FAIL;

		m_pRendererCom->Add_RenderGroup(CRenderer::RG_TERRAIN, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RG_SHADOW, this);

#ifdef _DEBUG
		if (true == m_bDebugRender)
		{
			CGameObject* pPlayer = m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
			CTransform* pTransform = (CTransform*)pPlayer->Find_Component(TEXT("Com_Transform"));

			_float4 vPlayerPos;
			XMStoreFloat4(&vPlayerPos, pTransform->Get_State(CTransform::STATE_POSITION));

			for (size_t i = 0; i < m_Elements.size(); i++) 
			{
				_float4 vElementPos = m_Elements[i]->Get_Position();

				_float4 vDirection;
				XMStoreFloat4(&vDirection, XMLoadFloat4(&vElementPos) - XMLoadFloat4(&vPlayerPos));

				_float fDistance = XMVectorGetX(XMVector4Length(XMLoadFloat4(&vDirection)));

				if (fDistance < 10.0f)
					m_pRendererCom->Add_DebugComponent(m_Elements[i]->Get_Collider());
			}
		}
#endif
	}

	return S_OK;
}

HRESULT CWorld::Render()
{
	if ((m_byFlag & WT_TERRAIN) != FALSE)
	{
		if (FAILED(m_pWorldTerrain->Render()))
			return E_FAIL;
	}

	if ((m_byFlag & WT_ELEMENT) != FALSE)
	{
		for (auto& pElement : m_Elements)
		{
			if (FAILED(pElement->Render()))
				return E_FAIL;
		}
	}

	if ((m_byFlag & WT_INSTANCING) != FALSE)
	{
		if (FAILED(m_pInstancing_Elements->Render()))
			return E_FAIL;
	}

	if ((m_byFlag & WT_WATER) != FALSE)
	{
		for (auto& pWater : m_Waters)
		{
			if (FAILED(pWater->Render()))
				return E_FAIL;
		}
	}
	
	return S_OK;
}

HRESULT CWorld::Render_Shadow(_float4x4* pLightViewProj, _int iNumArray)
{
	if ((m_byFlag & WT_ELEMENT) != FALSE)
	{
		for (auto& pElement : m_Elements)
		{
			if (FAILED(pElement->Render_Shadow(pLightViewProj, iNumArray)))
				return E_FAIL;
		}
	}

	if ((m_byFlag & WT_INSTANCING) != FALSE)
	{
		if (FAILED(m_pInstancing_Elements->Render_Shadow(pLightViewProj, iNumArray)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CWorld::Add_Components()
{
	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWorld::Initialize_World()
{
	if ((m_byFlag & WT_TERRAIN) != FALSE)
	{
		if (FAILED(Initialize_Terrain()))
			return E_FAIL;
	}

	if ((m_byFlag & WT_ELEMENT) != FALSE)
	{
		if (FAILED(Initialize_Element()))
			return E_FAIL;
	}

	if ((m_byFlag & WT_INSTANCING) != FALSE)
	{
		if (FAILED(Initialize_Instancing()))
			return E_FAIL;
	}

	if ((m_byFlag & WT_WATER) != FALSE)
	{
		if (FAILED(Initialize_Water()))
			return E_FAIL;
	}

	if (FAILED(Initialize_Spawner()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWorld::Initialize_Terrain()
{
	m_pWorldTerrain = CTerrain::Create(m_pDevice, m_pContext);

	if (nullptr == m_pWorldTerrain)
		return E_FAIL;

	return S_OK;
}

HRESULT CWorld::Initialize_Element()
{
	ifstream InFile(TEXT("../../Reference/Resources/Data/Map/WorldData.dat"), ios::binary);

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

		CWorldElement::ELEMENT_DESC ElementDesc = {};
		{
			ElementDesc.eLevel = LEVEL_WORLD;
			ElementDesc.eType = eType;
			ElementDesc.vWorldMatrix = vWorldMatrix;
			ElementDesc.strModelTag = strModelTag;
		}

		CWorldElement* pElement = CWorldElement::Create(m_pDevice, m_pContext, &ElementDesc);

		if (nullptr == pElement)
			return E_FAIL;

		m_Elements.push_back(pElement);
	}

	InFile.close();

	return S_OK;
}

HRESULT CWorld::Initialize_Instancing()
{
	m_pInstancing_Elements = CInstancing_Element::Create(m_pDevice, m_pContext, LEVEL_WORLD);

	if (nullptr == m_pInstancing_Elements)
		return E_FAIL;

	return S_OK;
}

HRESULT CWorld::Initialize_Water()
{
	ifstream InFile(TEXT("../../Reference/Resources/Data/Map/WorldWater.dat"), ios::binary);

	if (false == InFile.is_open())
		return E_FAIL;

	_uint iNumWater;
	InFile.read(reinterpret_cast<_char*>(&iNumWater), sizeof(_uint));

	_uint iSizeX;
	_uint iSizeZ;
	_float4x4 vWorldMatrix;
	_float fGridSpacing;
	_float fWaveScale;
	_float3 vUVScale;
	_float3 vNoise;
	_float2 vWaveSpeed;

	for (_uint i = 0; i < iNumWater; i++)
	{
		InFile.read(reinterpret_cast<_char*>(&iSizeX), sizeof(_uint));
		InFile.read(reinterpret_cast<_char*>(&iSizeZ), sizeof(_uint));
		InFile.read(reinterpret_cast<_char*>(&vWorldMatrix), sizeof(_float4x4));
		InFile.read(reinterpret_cast<_char*>(&fGridSpacing), sizeof(_float));
		InFile.read(reinterpret_cast<_char*>(&fWaveScale), sizeof(_float));
		InFile.read(reinterpret_cast<_char*>(&vUVScale), sizeof(_float3));
		InFile.read(reinterpret_cast<_char*>(&vNoise), sizeof(_float3));
		InFile.read(reinterpret_cast<_char*>(&vWaveSpeed), sizeof(_float2));

		CWater::WATER_DESC tWaterDesc = {};
		{
			tWaterDesc.iSizeX = iSizeX;
			tWaterDesc.iSizeZ = iSizeZ;
			tWaterDesc.vWorldMatrix = vWorldMatrix;
			tWaterDesc.fGridSpacing = fGridSpacing;
			tWaterDesc.fWaveScale = fWaveScale;
			tWaterDesc.vUVScale = vUVScale;
			tWaterDesc.vNoise = vNoise;
			tWaterDesc.vWaveSpeed = vWaveSpeed;
		}

		CWater* pWater = CWater::Create(m_pDevice, m_pContext, &tWaterDesc);

		if (nullptr == pWater)
			return E_FAIL;

		m_Waters.push_back(pWater);
	}

	return S_OK;
}

HRESULT CWorld::Initialize_Spawner()
{
	ifstream InFile(TEXT("../../Reference/Resources/Data/Map/Spawner_World.dat"), ios::binary);

	if (false == InFile.is_open())
		return E_FAIL;;

	_uint iNumSpawner;
	InFile.read(reinterpret_cast<_char*>(&iNumSpawner), sizeof(_uint));

	for (_uint i = 0; i < iNumSpawner; i++)
	{
		_float4 vSpawnPos;
		_float fSpawnerRange;
		_uint iNumMonster;

		InFile.read(reinterpret_cast<_char*>(&vSpawnPos), sizeof(_float4));
		InFile.read(reinterpret_cast<_char*>(&fSpawnerRange), sizeof(_float));
		InFile.read(reinterpret_cast<_char*>(&iNumMonster), sizeof(_uint));

		vector<CSpawnBox::MONSTER_INFO> MonsterInfos;

		for (_uint j = 0; j < iNumMonster; j++)
		{
			CSpawnBox::MONSTER_INFO tMonsterInfo = {};
			{
				InFile.read(reinterpret_cast<_char*>(&tMonsterInfo.eCreatureId), sizeof(CREATUREID));
				InFile.read(reinterpret_cast<_char*>(&tMonsterInfo.vSpawnPosition), sizeof(_float4));
			}

			MonsterInfos.push_back(tMonsterInfo);
		}

		CSpawnBox::SPAWNER_DESC tSpawnerDesc = {};
		{
			tSpawnerDesc.iLevel = LEVEL_WORLD;
			tSpawnerDesc.vSpawnerPos = vSpawnPos;
			tSpawnerDesc.fSpawnRange = fSpawnerRange;
			tSpawnerDesc.pMonsterInfo = &MonsterInfos;
		}

		if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_SpawnBox"), TEXT("Layer_SpawnBox"), &tSpawnerDesc)))
			return E_FAIL;
	}

	InFile.close();

	return S_OK;
}

HRESULT CWorld::Invalidate_Culling()
{
	CGameObject* pGameObject = m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Camera"));

	if (nullptr == pGameObject)
		return E_FAIL;

	CCamera_Main* pCamera = (CCamera_Main*)pGameObject;

	for (size_t i = 0; i < m_Elements.size(); i++) 
	{
		_bool bicColl = false;

		bicColl = pCamera->Culling_Frustum(m_Elements[i]->Get_Collider());

		m_Elements[i]->Set_Show(bicColl);
	}

	return S_OK;
}

CWorld* CWorld::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWorld* pInstance = new CWorld(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CWorld"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWorld::Clone(void* pArg)
{
	CWorld* pInstance = new CWorld(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CWorld"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWorld::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pWorldTerrain);

	for (auto& pElement : m_Elements)
		Safe_Release(pElement);

	m_Elements.clear();

	Safe_Release(m_pInstancing_Elements);

	for (auto& pWater : m_Waters)
		Safe_Release(pWater);

	m_Waters.clear();
}
