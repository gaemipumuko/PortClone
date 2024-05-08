#include "stdafx.h"
#include "GearOperator.h"

#include "Player.h"
#include "Updraft.h"
#include "InteractionBox.h"
#include "DungeonPlatform.h"

#include "Page_Main.h"
#include "UI_Manager.h"
#include "Abyss_Mage.h"

#include "GameInstance.h"

CGearOperator::CGearOperator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CGearOperator::CGearOperator(const CGearOperator& rhs)
	: CGameObject(rhs)
{
}

_bool CGearOperator::isInPlayer()
{
	_bool bisColl = false;
	CGameObject* pGameObject = m_pGameInstance->Find_Object(m_eLevel, TEXT("Layer_Player"));

	if (nullptr != pGameObject)
	{
		CPlayer* pPlayer = (CPlayer*)pGameObject;

		_int iColliderID = pPlayer->Get_BodyCollisionID();

		vector<CGameObject*> CollisionObjects = m_pGameInstance->Find_Collision_Objects(m_pColliderCom, COLL_EVENTREACT_PLAYER);

		if (false == CollisionObjects.empty())
		{
			CCollider* pCollider = (CCollider*)CollisionObjects.front()->Find_Component(TEXT("Com_Collider_OBB"));

			if (nullptr != pCollider)
			{
				bisColl = m_pColliderCom->Intersect(pCollider);
				return bisColl;
			}
		}
	}

	return bisColl;
}

HRESULT CGearOperator::Create_ChallengeBoss()
{
	CGameObject*					pSpawnBox = { nullptr };
	CSpawnBox::SPAWNER_DESC			tSpawnerDesc = { };

	tSpawnerDesc				= { };
	tSpawnerDesc.iLevel			= m_eLevel;
	tSpawnerDesc.fSpawnRange	= 10.f;
	tSpawnerDesc.pMonsterInfo	= &m_SpawnBoxDesc;
	XMStoreFloat4(&tSpawnerDesc.vSpawnerPos, m_pTransformCom[GEAR_OPERATOR]->Get_State(CTransform::STATE_POSITION));

	if (FAILED(m_pGameInstance->Add_Object_Layer(m_eLevel, TEXT("Prototype_GameObject_SpawnBox"), TEXT("Layer_Spawner"), &tSpawnerDesc, &pSpawnBox)))
		return E_FAIL;

	m_strSpawnBoxTag = static_cast<CSpawnBox*>(pSpawnBox)->CSpawnBox::Get_SpawnerTag();
	if (m_SpawnBoxDesc.front().eCreatureId == MONSTER_ABYSS_MAGE_ELEC)
		m_strSpawnBoxTag = L"Layer_Abyss_Mage";

	return S_OK;
}

HRESULT CGearOperator::Add_Component(void* pArg)
{
	GEAR_DESC* pGearOperatorDesc = static_cast<GEAR_DESC*>(pArg);

	/* For.Com_Model_Operator */
	if (FAILED(CGameObject::Add_Component(pGearOperatorDesc->eGearLevel, TEXT("Prototype_Component_Model_Gear_Operator"),
		TEXT("Com_Model_Operator"), (CComponent**)&m_pModelCom[GEAR_OPERATOR])))
		return E_FAIL;

	/* For.Com_Model_Rune */
	if (FAILED(CGameObject::Add_Component(pGearOperatorDesc->eGearLevel, TEXT("Prototype_Component_Model_Gear_RuneCylinder"),
		TEXT("Com_Model_Rune"), (CComponent**)&m_pModelCom[GEAR_RUNE])))
		return E_FAIL;

	/* For.Com_Shader_Operator */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader_Operator"), (CComponent**)&m_pShaderCom[GEAR_OPERATOR])))
		return E_FAIL;

	/* For.Com_Shader_Rune */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader_Rune"), (CComponent**)&m_pShaderCom[GEAR_RUNE])))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform_Operator */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform_Operator"), (CComponent**)&m_pTransformCom[GEAR_OPERATOR])))
		return E_FAIL;

	/* For.Com_Transform_Rune */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform_Rune"), (CComponent**)&m_pTransformCom[GEAR_RUNE])))
		return E_FAIL;

	/* For.Com_Collider_OBB */
	CBounding_SPHERE::BOUNDING_SPHERE_DESC tBoundingDesc = {};
	tBoundingDesc.fRadius = 1.5f;
	tBoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		TEXT("Com_Collider_SPHERE"), (CComponent**)&m_pColliderCom, &tBoundingDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGearOperator::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CGearOperator::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGearOperator::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	GEAR_DESC* pGearDesc = static_cast<GEAR_DESC*>(pArg);

	m_eLevel	= pGearDesc->eGearLevel;
	m_bSealed	= pGearDesc->bSealed;
	m_eOperatorType = pGearDesc->eOperatorType;
	m_eChallengeType = pGearDesc->eChallengeType;
	m_strSpawnBoxTag = pGearDesc->strSpawnBoxTag;
	m_pDungeonPlatform = pGearDesc->pPlatform;

	if (m_eChallengeType == CHL_INTERACTION)
		m_SpawnBoxDesc = pGearDesc->SpawnBoxDesc;

	m_pTransformCom[GEAR_OPERATOR]->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pGearDesc->vGearPosition));
	m_pTransformCom[GEAR_OPERATOR]->Rotations(
		vector<_vector>{ { 1.f, 0.f, 0.f, 0.f}, { 0.f, 1.f, 0.f, 0.f }, { 0.f, 0.f, 1.f, 0.f } },
		vector<_float>{
			XMConvertToRadians(pGearDesc->vGearRotation.x),
			XMConvertToRadians(pGearDesc->vGearRotation.y),
			XMConvertToRadians(pGearDesc->vGearRotation.z) });

	m_pTransformCom[GEAR_RUNE]->Set_Scale({ 0.1f, 0.025f, 0.1f, 0.f });
	m_pTransformCom[GEAR_RUNE]->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pGearDesc->vGearPosition) + _vector{ 0.f, 0.5f, 0.f, 0.f });

	m_pColliderCom->Update(m_pTransformCom[GEAR_OPERATOR]->Get_WorldMatrix());

	/* InteractionBox */
	m_pInteractionBox = static_cast<CInteraction_Box*>(m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Interaction_Box"))->Clone(nullptr));
	m_pInteractionBox->Add_InteractionBox(pGearDesc->strInteraction, CInteraction_Box::ICON_TALK);

	return S_OK;
}

HRESULT CGearOperator::PreviousTick(_float fTimeDelta)
{
	if (m_eChallengeType == CHL_SEALEDAUTO)
	{
		switch (m_byStep)
		{
		case 0:
			m_pModelCom[GEAR_OPERATOR]->Set_Animation(0);
			m_pModelCom[GEAR_OPERATOR]->Play_Animation(TRUE, m_pTransformCom[GEAR_OPERATOR], fTimeDelta);
			if (m_pGameInstance->Find_Objects(m_eLevel, m_strSpawnBoxTag) != nullptr)
				++m_byStep;
			break;

		case 1:
			m_pModelCom[GEAR_OPERATOR]->Set_Animation(0);
			m_pModelCom[GEAR_OPERATOR]->Play_Animation(TRUE, m_pTransformCom[GEAR_OPERATOR], fTimeDelta);
			if (m_pGameInstance->Find_Objects(m_eLevel, m_strSpawnBoxTag) != nullptr)
			{
				if (m_pGameInstance->Find_Objects(m_eLevel, m_strSpawnBoxTag)->size() < 1)
				{
					++m_byStep;
					m_pGameInstance->PlaySoundW(L"GearSolve.mp3");
				}
			}
			break;

		case 2:
		{
			m_pModelCom[GEAR_OPERATOR]->Set_Animation(2);
			_float fRate = m_pModelCom[GEAR_OPERATOR]->Play_Animation(FALSE, m_pTransformCom[GEAR_OPERATOR], fTimeDelta);
			if (fRate > m_pModelCom[GEAR_OPERATOR]->Get_AnimationDuration(2))
				++m_byStep;
		}
			break;

		case 3:
			m_bSealed = FALSE;
			m_pModelCom[GEAR_OPERATOR]->Set_Animation(1);
			m_pModelCom[GEAR_OPERATOR]->Play_Animation(TRUE, m_pTransformCom[GEAR_OPERATOR], fTimeDelta);
			break;

		default:
			break;
		}
	}

	else if (m_eChallengeType == CHL_INTERACTION)
	{
		CUI_Manager* pUIManager = static_cast<CUI_Manager*>(m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_UIManager"), 0));
		CPage_Main* m_pPage_Main = static_cast<CPage_Main*>(pUIManager->Get_UI_Page(PAGE_MAIN));

		switch (m_byStep)
		{
		case 0:
			m_pPage_Main->Display_Timer_UI(FALSE);
			m_pModelCom[GEAR_OPERATOR]->Set_Animation(0);
			m_pModelCom[GEAR_OPERATOR]->Play_Animation(TRUE, m_pTransformCom[GEAR_OPERATOR], fTimeDelta);
			if (m_bOperated == TRUE)
			{
				m_fTimer = 60.f;

				if (FAILED(CGearOperator::Create_ChallengeBoss()))
					return E_FAIL;

				m_bSealed = TRUE;
				++m_byStep;
			}
			break;

		case 1:
			m_pPage_Main->Display_Timer_UI(TRUE);
			m_pModelCom[GEAR_OPERATOR]->Set_Animation(0);
			m_pModelCom[GEAR_OPERATOR]->Play_Animation(TRUE, m_pTransformCom[GEAR_OPERATOR], fTimeDelta);
			if (m_pGameInstance->Find_Objects(m_eLevel, m_strSpawnBoxTag) != nullptr)
			{
				m_fTimer -= fTimeDelta;

				m_pPage_Main->Update_Timer(max(m_fTimer, 0.f));

				if (m_fTimer < 0.f)
				{
					list<CGameObject*>* ChallengeBosses = m_pGameInstance->Find_Objects(m_eLevel, m_strSpawnBoxTag);
					for (auto& iter : *ChallengeBosses)
					{
						static_cast<CAbyss_Mage*>(iter)->Set_DeadState();
					}

					m_byStep = 0;
					m_bSealed = FALSE;
					m_bOperated = FALSE;
				}

				if (m_pGameInstance->Find_Objects(m_eLevel, m_strSpawnBoxTag)->size() < 1)
				{
					m_bOperated = FALSE;
					m_pGameInstance->PlaySoundW(L"GearSolve.mp3");
					m_pGameInstance->PlaySoundW(L"Zhongli_Burst");
					m_pInteractionBox->Clear_Boxes();
					m_pInteractionBox->Add_InteractionBox(L"작동하기", CInteraction_Box::ICON_TALK);

					++m_byStep;
				}
			}
			break;

		case 2:
		{
			m_pPage_Main->Display_Timer_UI(FALSE);
			m_pModelCom[GEAR_OPERATOR]->Set_Animation(2);
			_float fRate = m_pModelCom[GEAR_OPERATOR]->Play_Animation(FALSE, m_pTransformCom[GEAR_OPERATOR], fTimeDelta);
			if (fRate > m_pModelCom[GEAR_OPERATOR]->Get_AnimationDuration(2))
			{
				m_bSealed = FALSE;

				++m_byStep;
			}
		}
			break;

		case 3:
			m_pModelCom[GEAR_OPERATOR]->Set_Animation(1);
			m_pModelCom[GEAR_OPERATOR]->Play_Animation(TRUE, m_pTransformCom[GEAR_OPERATOR], fTimeDelta);
			break;

		default:
			break;
		}
	}

	if (m_pDungeonPlatform != nullptr)
		m_pDungeonPlatform->PreviousTick(fTimeDelta);

	return S_OK;
}

HRESULT CGearOperator::Tick(_float fTimeDelta)
{
	m_vFrame.x = fmod(m_vFrame.x + fTimeDelta * 0.25f, 1.f);

	if ((CGearOperator::isInPlayer() == TRUE) && (m_bOperated == FALSE) && (m_bSealed == FALSE))
	{
		m_pInteractionBox->Tick(fTimeDelta);

		if (m_pGameInstance->Key_Down('F'))
		{
			m_bOperated = TRUE;
			m_pGameInstance->PlaySoundW(L"InteractionBox.mp3", 0.7f);

			switch (m_eOperatorType)
			{
			case OPR_WINDFIELD:
			{
				CUpdraft::UPDRAFT_DESC tDesc = { };
				XMStoreFloat4(&tDesc.vPosition, m_pTransformCom[GEAR_OPERATOR]->Get_State(CTransform::STATE_POSITION));
				tDesc.vColliderSize = _float3(2.f, 6.f, 2.f);

				if (FAILED(m_pGameInstance->Add_Object_Layer(m_eLevel, L"Prototype_GameObject_Updraft", L"Layer_Updraft", &tDesc)))
					return E_FAIL;
			}
				break;

			case OPR_PLATFORM:
				if (m_byStep == 3)
				{
					if (m_pDungeonPlatform != nullptr)
						m_pDungeonPlatform->Set_Reacted(TRUE);
				}
				break;
			}
		}
	}

	if (m_pDungeonPlatform != nullptr)
		m_pDungeonPlatform->Tick(fTimeDelta);

	return S_OK;
}

HRESULT CGearOperator::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

	if (CGearOperator::isInPlayer() == TRUE && (m_bOperated == FALSE) && (m_bSealed == FALSE))
	{
		m_pInteractionBox->LateTick(fTimeDelta);
	}

	if (m_pDungeonPlatform != nullptr)
		m_pDungeonPlatform->LateTick(fTimeDelta);

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif

	return S_OK;
}

HRESULT CGearOperator::Render()
{
	_uint		iNumMeshes = { };

	/* Gear Operator */
	if (FAILED(m_pShaderCom[GEAR_OPERATOR]->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom[GEAR_OPERATOR]->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom[GEAR_OPERATOR]->Bind_Matrix("g_WorldMatrix", m_pTransformCom[GEAR_OPERATOR]->Get_WorldFloat4x4())))
		return E_FAIL;

	iNumMeshes = m_pModelCom[GEAR_OPERATOR]->Get_NumMeshes();
	for (size_t siMesh = 0; siMesh < iNumMeshes; siMesh++)
	{
		if (FAILED(m_pModelCom[GEAR_OPERATOR]->Bind_Material(m_pShaderCom[GEAR_OPERATOR], "g_DiffuseTexture", static_cast<_uint>(siMesh), TEXTURETYPE::aiTextureType_DIFFUSE)))
			continue;

		if (FAILED(m_pModelCom[GEAR_OPERATOR]->Bind_BoneMatrices(static_cast<_uint>(siMesh), m_pShaderCom[GEAR_OPERATOR], "g_BoneMatrices")))
			return E_FAIL;

		if (FAILED(m_pShaderCom[GEAR_OPERATOR]->Begin(VTXANIMMESH_DUNGEONOBJECT)))
			return E_FAIL;

		if (FAILED(m_pModelCom[GEAR_OPERATOR]->Render(static_cast<_uint>(siMesh))))
			return E_FAIL;
	}

	/* Gear Rune */
	if (m_bSealed == TRUE)
	{
		if (FAILED(m_pShaderCom[GEAR_RUNE]->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
			return E_FAIL;

		if (FAILED(m_pShaderCom[GEAR_RUNE]->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
			return E_FAIL;

		if (FAILED(m_pShaderCom[GEAR_RUNE]->Bind_Matrix("g_WorldMatrix", m_pTransformCom[GEAR_RUNE]->Get_WorldFloat4x4())))
			return E_FAIL;

		if (FAILED(m_pShaderCom[GEAR_RUNE]->Bind_RawValue("g_vUVRate", &m_vFrame, sizeof(_float4))))
			return E_FAIL;

		iNumMeshes = m_pModelCom[GEAR_RUNE]->Get_NumMeshes();
		for (size_t siMesh = 0; siMesh < iNumMeshes; siMesh++)
		{
			if (FAILED(m_pModelCom[GEAR_RUNE]->Bind_Material(m_pShaderCom[GEAR_RUNE], "g_DiffuseTexture", static_cast<_uint>(siMesh), TEXTURETYPE::aiTextureType_DIFFUSE)))
				continue;

			if (FAILED(m_pShaderCom[GEAR_RUNE]->Begin(VTXMESH_RUNE_SEALED)))
				return E_FAIL;

			if (FAILED(m_pModelCom[GEAR_RUNE]->Render(static_cast<_uint>(siMesh))))
				return E_FAIL;
		}
	}

	return S_OK;
}

CGearOperator* CGearOperator::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGearOperator* pInstance = new CGearOperator(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CGearOperator"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGearOperator::Clone(void* pArg)
{
	CGearOperator* pInstance = new CGearOperator(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CGearOperator"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGearOperator::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom[GEAR_OPERATOR]);
	Safe_Release(m_pModelCom[GEAR_RUNE]);

	Safe_Release(m_pShaderCom[GEAR_OPERATOR]);
	Safe_Release(m_pShaderCom[GEAR_RUNE]);

	Safe_Release(m_pTransformCom[GEAR_OPERATOR]);
	Safe_Release(m_pTransformCom[GEAR_RUNE]);

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pInteractionBox);
	Safe_Release(m_pDungeonPlatform);
}
