#include "stdafx.h"
#include "AndriusSword.h"

#include "GameInstance.h"
#include "InteractionBox.h"
#include "Player.h"

CAndriusSword::CAndriusSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCreature(pDevice, pContext)
{
}

CAndriusSword::CAndriusSword(const CAndriusSword& rhs)
	: CCreature(rhs)
{
}

HRESULT CAndriusSword::Add_Components(void* pArg)
{
	if (FAILED(__super::Add_Component(pArg)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_AndriusSword"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAndriusSword::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float fCamFar = static_cast<CCamera*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"))->Get_CamFar();
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fCamFar, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CAndriusSword::Add_Parts(void* pArg)
{
	return S_OK;
}

HRESULT CAndriusSword::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAndriusSword::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(CAndriusSword::Add_Components(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(953.0f, 0.f, 187.0f, 1.0f));

	m_pInteractionBox = (CInteraction_Box*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Interaction_Box"))->Clone(nullptr);

	m_pInteractionBox->Add_InteractionBox(TEXT("시련시작"), CInteraction_Box::ICON_TALK);

	return S_OK;
}

HRESULT CAndriusSword::PreviousTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_RenderValidity() == S_OK)
	{
		if (FAILED(__super::PreviousTick(fTimeDelta)))
			return E_FAIL;

		m_pInteractionBox->PreviousTick(fTimeDelta);
	}

	return S_OK;
}

HRESULT CAndriusSword::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_RenderValidity() == S_OK)
	{
		XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

		if (FAILED(__super::Tick(fTimeDelta)))
			return E_FAIL;

		//플레이어 위치 찾아서 5.f 안에 들어오면 상호작용 가능
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
		_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

		CTransform* pPlayerTransform =
			static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

		_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vToPlayer = vPlayerPos - vPosition;
		XMVector3Length(vToPlayer);
		XMVector3Normalize(vToPlayer);
		if (XMVector3Length(vToPlayer).m128_f32[0] <= 5.f)
		{
			m_pInteractionBox->Tick(fTimeDelta);

			if (m_pGameInstance->Key_Down('F'))
			{

				if (m_pInteractionBox->Get_SelectedIndex() == 0)
				{
					//상호작용시 검 사라지면서 발생할 동작
					//m_bDestroy = true;

					OBJECT_DESC	ObjDesc = {};
					ObjDesc.vInitPos = XMVectorSet(953.0f, 0.f, 187.0f, 1.0f);
					ObjDesc.iObjectType = MONSTER_ANDRIUS;
					if (FAILED(m_pGameInstance->Add_Object_Layer(LEVEL_WORLD, TEXT("Prototype_GameObject_Andrius"), L"Layer_Boss", &ObjDesc)))
						return E_FAIL;

					m_pGameInstance->Register_Interaction(L"Hili_Normal");

					m_pInteractionBox->Clear_Boxes();

					m_bDestroy = TRUE;
				}
			}
		}
	}

	return S_OK;
}

HRESULT CAndriusSword::LateTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_RenderValidity() == S_OK)
	{
		if (FAILED(__super::LateTick(fTimeDelta)))
			return E_FAIL;

		m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);


		//플레이어와 가까울 때만 LateTick
		CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")));
		_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

		CTransform* pPlayerTransform =
			static_cast<CTransform*>(pPlayer->Get_PlayerInfo(iIndex).pTransform);

		_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vToPlayer = vPlayerPos - vPosition;

		XMVector3Length(vToPlayer);
		XMVector3Normalize(vToPlayer);
		if (XMVector3Length(vToPlayer).m128_f32[0] <= 5.f)
		{
			m_pInteractionBox->LateTick(fTimeDelta);
		}
	}

	return S_OK;
}

HRESULT CAndriusSword::Render()
{
	if (FAILED(CAndriusSword::Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint siMesh = 0; siMesh < iNumMeshes; siMesh++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(siMesh), TEXTURETYPE::aiTextureType_DIFFUSE)))
			continue;

		if (FAILED(m_pShaderCom->Begin(VTXMESH_DEFALUT)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(siMesh)))
			return E_FAIL;
	}

	return S_OK;
}

CAndriusSword* CAndriusSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAndriusSword* pInstance = new CAndriusSword(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CAndriusSword"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAndriusSword::Clone(void* pArg)
{
	CAndriusSword* pInstance = new CAndriusSword(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CAndriusSword"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAndriusSword::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pInteractionBox);
}
