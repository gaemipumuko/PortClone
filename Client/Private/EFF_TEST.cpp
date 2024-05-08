#include "stdafx.h"
#include "EFF_TEST.h"

#include "GameInstance.h"
#include "Player.h"

CEff_TEST_CLASS::CEff_TEST_CLASS(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CEff_TEST_CLASS::CEff_TEST_CLASS(const CEff_TEST_CLASS& rhs)
	: CEffect(rhs)
{
}

HRESULT CEff_TEST_CLASS::Add_Component(void* pArg)
{
	__super::Add_Component(pArg);

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Water_Bubble"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CEff_TEST_CLASS::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEff_TEST_CLASS::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEff_TEST_CLASS::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);

		m_EffectDesc.eEffectID = pDesc->eEffectID;
		m_EffectDesc.fLifeTime = pDesc->fLifeTime;
		m_EffectDesc.vSpawnPos = pDesc->vSpawnPos;
		m_EffectDesc.vScale	= pDesc->vScale;

		m_fLifeTime = m_EffectDesc.fLifeTime;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vSpawnPos);
		m_pTransformCom->Set_Scale(m_EffectDesc.vScale);

	}
	else
		return E_FAIL;



	m_fLifeTime = 3.f;

	return S_OK;
}

HRESULT CEff_TEST_CLASS::PreviousTick(_float fTimeDelta)
{

	return S_OK;
}

HRESULT CEff_TEST_CLASS::Tick(_float fTimeDelta)
{	
	//m_fLifeTime -= fTimeDelta;
	//if (m_fLifeTime <= 0.f)
	//{
	//	Set_Destroy(TRUE);
	//	Set_Dead(TRUE);
	//}

	m_fTimeAcc += fTimeDelta;
	m_fColorRate0 += log(m_fTimeAcc / 10 + 1.f) * 4.f;

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	if (m_pGameInstance->Key_Down(VK_UP))
	{
		if(m_fAlpha<255.f)
		m_fAlpha += 5.f;

		cout << "Current a : " << m_fAlpha << endl;
	}
	else if(m_pGameInstance->Key_Down(VK_DOWN))
	{
		if (m_fAlpha > 0.f)
		m_fAlpha -= 5.f;
		cout << "Current a : " << m_fAlpha << endl;
	}
	else if (m_pGameInstance->Key_Down('Z'))
	{
		m_bRchannel = !m_bRchannel;
		cout << "SHOW R : " << m_bRchannel << endl;
	}
	else if (m_pGameInstance->Key_Down('X'))
	{
		m_bGchannel = !m_bGchannel;
		cout << "SHOW G : " << m_bGchannel << endl;
	}
	else if (m_pGameInstance->Key_Down('C'))
	{
		m_bBchannel = !m_bBchannel;
		cout << "SHOW B : " << m_bBchannel << endl;
	}
	else if (m_pGameInstance->Key_Down(VK_LEFT))
	{
		m_eTextureChannel--;
		if (m_eTextureChannel == -1)
			m_eTextureChannel = BLUE;
		cout << "Currently Modifying Channel : " << (TEX_CHANNELS)m_eTextureChannel << endl;
	}
	else if (m_pGameInstance->Key_Down(VK_RIGHT))
	{
		m_eTextureChannel++;
		if (m_eTextureChannel == CHANNEL_END)
			m_eTextureChannel = RED;
		cout << "Currently Modifying Channel : " << (TEX_CHANNELS)m_eTextureChannel << endl;
	}

	return S_OK;
}

HRESULT CEff_TEST_CLASS::LateTick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix());

	//m_pTransformCom->Look_At_Position(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()));

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_POSTEFFECT, this);

	return S_OK;
}

HRESULT CEff_TEST_CLASS::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

		//if (FAILED(m_pShaderCom->Bind_RawValue("g_ColorRate", &m_fTimeAcc, sizeof(_float))))
		//	return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(VTXTEX_ALPHA)))
			return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

CEff_TEST_CLASS* CEff_TEST_CLASS::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEff_TEST_CLASS* pInstance = new CEff_TEST_CLASS(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CEff_TEST_CLASS"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEff_TEST_CLASS::Clone(void* pArg)
{
	CEff_TEST_CLASS* pInstance = new CEff_TEST_CLASS(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CEff_TEST_CLASS"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEff_TEST_CLASS::Free()
{
	__super::Free();

	if (m_bCloned == true)
	{
		Safe_Release(m_pShaderCom);
		Safe_Release(m_pTextureCom);
		Safe_Release(m_pVIBufferCom);
	}
}