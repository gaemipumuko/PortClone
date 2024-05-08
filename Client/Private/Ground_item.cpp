#include "stdafx.h"
#include "Ground_Item.h"

#include "GameInstance.h"
#include "Camera.h"
#include "InteractionBox.h"
#include "Inventory.h"
#include "Player.h"
#include "Effect.h"


CGround_Item::CGround_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDeciderObject(pDevice, pContext)
{
}

CGround_Item::CGround_Item(const CGround_Item& rhs)
	: CDeciderObject(rhs)
{
}

HRESULT CGround_Item::Add_Component(void* pArg)
{
	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Relic_Item"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	///* For.Com_Collider_SPHERE */
	//CBounding_SPHERE::BOUNDING_SPHERE_DESC		BoundingDesc = {};
	//BoundingDesc.fRadius = 1.f;
	//BoundingDesc.vCenter = _float3(0.f, BoundingDesc.fRadius, 0.f);

	//if (FAILED(CGameObject::Add_Component(LEVEL_WORLD, TEXT("Prototype_Component_Collider_SPHERE"),
	//	TEXT("Com_Collider_SPHERE"), (CComponent**)&m_pColliderCom, &BoundingDesc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CGround_Item::Bind_ShaderResources()
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

HRESULT CGround_Item::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGround_Item::Initialize_Clone(void* pArg)
{
	/*if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;*/

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		DROP_ITEM_DESC* tItemDesc = (DROP_ITEM_DESC*)pArg;
		
		m_pItem = CItem::Create_Item(tItemDesc->iItemIndex, tItemDesc->iItemQuantity);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION,tItemDesc->vPos);
	}
	else
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(5.f, 0.f, 5.f, 1.f));

	}

	m_pTransformCom->Set_Scale({ 0.4f, 0.4f, 0.4f, 0.f });

	m_pInteractionBox = (CInteraction_Box*)m_pGameInstance->Find_Object_Prototype(TEXT("Prototype_GameObject_Interaction_Box"))->Clone(nullptr);

	m_pInteractionBox->Add_InteractionBox(m_pItem->Get_Item_Name(), CInteraction_Box::ICON_TALK);

	CEffect::EFFECT_DESC descc = {};
	descc.eEffectID = EFFECT_ETC;
	_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vSelfLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	descc.vSpawnPos = vSelfPos;
	descc.vScale = { 0.8f, 0.8f, 0.8f, 1.f };
	descc.pParent = this;

	_int iRare = m_pItem->Get_Item_Rarity();

		switch (iRare)
	{
	case 0:
		descc.vBaseColor = COLOR_COMMON;
		break;
	case 1:
		descc.vBaseColor = COLOR_UNCOMMON;
		break;
	case 2:
		descc.vBaseColor = COLOR_RARE;
		break;
	case 3:
		descc.vBaseColor = COLOR_EPIC;
		break;
	case 4:
		descc.vBaseColor = COLOR_LEGENDARY;
		break;
	default:
		break;
	}


	m_pItem_Glow = static_cast<CItem_Glow*>(m_pGameInstance->Find_Object_Prototype(L"Prototype_GameObject_Effect_Item_Glow")->Clone(&descc));

	_float fDirX = m_pGameInstance->Get_Float_Random(-1.f, 1.f);
	_float fDirZ = m_pGameInstance->Get_Float_Random(-1.f, 1.f);

	m_vInitialDir = XMVector3Normalize({ fDirX,0.f,fDirZ,0.f });

	m_eRenderType = RTYPE_GAMEPLAY;

	return S_OK;
}

HRESULT CGround_Item::PreviousTick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		CGameObject* pPlayer = m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
		if (pPlayer != nullptr)
		{
			_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

			CTransform* pPlayerTransform =
				static_cast<CTransform*>(static_cast<CPlayer*>(pPlayer)->Get_PlayerInfo(iIndex).pTransform);

			_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
			_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector vToPlayer = vPlayerPos - vPosition;

			XMVector3Length(vToPlayer);
			XMVector3Normalize(vToPlayer);
			if (XMVector3Length(vToPlayer).m128_f32[0] <= 5.f)
			{
				m_pInteractionBox->PreviousTick(fTimeDelta);
			}

			m_pItem_Glow->PreviousTick(fTimeDelta);
		}
	}
	return S_OK;
}

HRESULT CGround_Item::Tick(_float fTimeDelta)
{
	if (CDeciderObject::Verify_TickValidity() == S_OK)
	{
		XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

		CGameObject* pPlayer = m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
		if (pPlayer != nullptr)
		{
			_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

			CTransform* pPlayerTransform =
				static_cast<CTransform*>(static_cast<CPlayer*>(pPlayer)->Get_PlayerInfo(iIndex).pTransform);

			_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
			_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector vToPlayer = vPlayerPos - vPosition;
			_vector vPos = XMVectorSet(0.7f * static_cast<_float>(cos(45)) * m_fTime * 0.9f, 0.7f * static_cast<_float>(sin(45)) * m_fTime - 1.f / 2.f * 5.5f * m_fTime * 0.9f * m_fTime * 0.9f, 0.f, 0.f);


			m_fTime += fTimeDelta;

			if (m_fTime < 0.5f)
			{
				_vector vSelfPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				_vector vUp = { 0.f,1.f,0.f,0.f };

				vSelfPos += (m_vInitialDir * fTimeDelta * 3.f + vUp * fTimeDelta * cos(m_fTime * 5.f) * 8.f);

				m_pTransformCom->Set_State(CTransform::STATE_POSITION, vSelfPos);
			}
			else
			{
				_vector		vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
				vUp = XMVector3Normalize(vUp);
				vPosition += vUp * (cos(m_fTime) / 100.f);
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
				m_pTransformCom->Rotation(vUp, XMConvertToRadians(m_fTime * 100.f));
			}

			//if (m_pGameInstance->Key_Down('G'))
			//{
			//	_vector		vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
			//	vPosition = vPlayerPos + (XMVector3Normalize(XMVectorSet(0.f,1.f,0.f,0.f)) * 1.f);
			//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
			//	m_fTime = 0.f;
			//	m_fRotatiton += 10.f;

			//	// 각도 (라디안 단위)
			//	float angleInRadians = XMConvertToRadians(m_fRotatiton);
			//	// 회전 행렬 생성
			//	XMMATRIX rotationMatrix = XMMatrixRotationY(angleInRadians);
			//	// 벡터를 회전시키기
			//	XMFLOAT4 rotatedVector;
			//	vPosition = XMVector4Transform(vPosition, rotationMatrix);


			//}


			//if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] > vPlayerPos.m128_f32[1] + 0.13f)
			//{
			//	m_fTime += fTimeDelta;
			//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition +vPos);
			//}

			//m_pTransformCom->Rotation({ 0.f,0.f,1.f,0.f }, XMConvertToRadians(m_fRotatiton));


			XMVector3Length(vToPlayer);
			XMVector3Normalize(vToPlayer);
			if (XMVector3Length(vToPlayer).m128_f32[0] <= 5.f)
			{
				m_pInteractionBox->Tick(fTimeDelta);

				if (m_pGameInstance->Key_Down('F'))
				{


					if (m_pInteractionBox->Get_SelectedIndex() == 0)
					{
						CInventory* pInven = (CInventory*)m_pGameInstance->Find_Object(LEVEL_STATIC, TEXT("Layer_Inventory"));
						pInven->Add_Item(m_pItem);
						m_bDestroy = true;
					}
				}
			}
			m_pItem_Glow->Tick(fTimeDelta);
		}
	}
	return S_OK;
}

HRESULT CGround_Item::LateTick(_float fTimeDelta)
{
	CGameObject* pPlayer = m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player"));
	if (pPlayer != nullptr)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

		_int iIndex = static_cast<_int>(static_cast<CPlayer*>(pPlayer)->Get_CurrentIndex());

		CTransform* pPlayerTransform =
			static_cast<CTransform*>(static_cast<CPlayer*>(pPlayer)->Get_PlayerInfo(iIndex).pTransform);

		_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vToPlayer = vPlayerPos - vPosition;

		XMVector3Length(vToPlayer);
		XMVector3Normalize(vToPlayer);
		if (XMVector3Length(vToPlayer).m128_f32[0] <= 5.f)
		{
			m_pInteractionBox->LateTick(fTimeDelta);
		}
		m_pItem_Glow->LateTick(fTimeDelta);
	}

	return S_OK;
}

HRESULT CGround_Item::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(VTXMESH_NONCULL)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

CGround_Item* CGround_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGround_Item* pInstance = new CGround_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CGround_Item"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGround_Item::Clone(void* pArg)
{
	CGround_Item* pInstance = new CGround_Item(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CGround_Item"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGround_Item::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	if (m_bCloned == true)
	{
		Safe_Release(m_pInteractionBox);
		Safe_Release(m_pItem_Glow);

		if(m_bDestroy == false)
		Safe_Release(m_pItem);
	}
}