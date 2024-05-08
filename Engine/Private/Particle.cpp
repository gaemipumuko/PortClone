#include "Particle.h"

#include "GameInstance.h"

CParticle::CParticle()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

CParticle::CParticle(const CParticle& rhs)
	: m_pGameInstance(rhs.m_pGameInstance)
	, m_tParticleInfo(rhs.m_tParticleInfo)
	, m_bCloned(TRUE)
{
}

HRESULT CParticle::Invalidate_Instance(_float fTimeDelta)
{
	if (m_bDuration == TRUE)
	{
		m_fDuration -= fTimeDelta;
		if (m_fDuration < 0.f)
			m_fDuration = 0.f;
	}
	if (m_bDelayStop == TRUE)
	{
		m_fDelay -= fTimeDelta;
	}
	if (m_bFade == TRUE)
	{
		m_fTimeAcc += fTimeDelta;
	}
	else
	{
		m_fTimeAcc = -1.f;
	}

	if (m_tParticleInfo.bFrameAnimation == TRUE)
	{
		m_fCurrCycle -= fTimeDelta;
		if (m_fCurrCycle < 0.f)
		{
			m_fCurrCycle = m_fMaxCycle;
			
			m_tParticleInfo.iCurrParticleRowCol.x += 1;

			if (m_tParticleInfo.iCurrParticleRowCol.x >= m_tParticleInfo.iNumParticleRowCol.x)
			{
				m_tParticleInfo.iCurrParticleRowCol.x = 0;
				m_tParticleInfo.iCurrParticleRowCol.y += 1;
			}

			if (m_tParticleInfo.iCurrParticleRowCol.y >= m_tParticleInfo.iNumParticleRowCol.y)
			{
				m_tParticleInfo.iCurrParticleRowCol.y = 0;
			}
		}
	}

	_matrix CombinedMatrix = XMMatrixIdentity();
	
	if (m_pParentWorldMatrix != nullptr)
		CombinedMatrix = XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) * XMLoadFloat4x4(&m_pParentWorldMatrix->Get_WorldFloat4x4());
	
	if (m_pParentBoneMatrix != nullptr)
		CombinedMatrix = XMLoadFloat4x4(m_pParentBoneMatrix) * CombinedMatrix;
	
	/* Correction */
	_vector vCorrection = XMVector4Transform(XMLoadFloat4(&m_tParticleInfo.vCorrectionVector), CombinedMatrix);
	CombinedMatrix.r[3].m128_f32[0] -= vCorrection.m128_f32[0];
	CombinedMatrix.r[3].m128_f32[1] -= vCorrection.m128_f32[1];
	CombinedMatrix.r[3].m128_f32[2] -= vCorrection.m128_f32[2];
	
	if (m_tParticleInfo.bTrail == TRUE)
	{
		memcpy(&m_tParticleInfo.vTrailPosition, &CombinedMatrix.r[3], sizeof _float4);
	
		XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	}
	else
		XMStoreFloat4x4(&m_WorldMatrix, CombinedMatrix);

	m_pVIBufferCom->Invalidate_Instance(fTimeDelta, m_tParticleInfo);

	return S_OK;
}

HRESULT CParticle::Add_Component(void* pArg)
{
	PARTICLE_DESC* pDesc = static_cast<PARTICLE_DESC*>(pArg);

	m_pShaderCom = pDesc->pShaderCom;
	Safe_AddRef(m_pShaderCom);

	m_pTextureCom = pDesc->pTextureCom;
	Safe_AddRef(m_pTextureCom);

	m_pVIBufferCom = static_cast<CVIBuffer_Particle_Instancing*>(pDesc->pVIBufferCom->Clone(&m_tParticleInfo));

	return S_OK;
}

HRESULT CParticle::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResources(m_pShaderCom, "g_Textures")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &m_iParticleIdx, sizeof(_int))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iNumParticleRowCol", &m_tParticleInfo.iNumParticleRowCol, sizeof(XMINT2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iCurrParticleRowCol", &m_tParticleInfo.iCurrParticleRowCol, sizeof(XMINT2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_tParticleInfo.vParticleColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAngle", &m_tParticleInfo.fAngle, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle::Initialize_Prototype(PARTICLE_INFO tParticleInfo)
{
	memcpy(&m_tParticleInfo, &tParticleInfo, sizeof PARTICLE_INFO);

	if (XMVector4Equal(m_tParticleInfo.vParticleColor, { 0.f, 0.f, 0.f, 0.f }))
	{
		m_tParticleInfo.vParticleColor.m128_f32[0] = m_pGameInstance->Get_Float_Random(0.f, 1.f);
		m_tParticleInfo.vParticleColor.m128_f32[1] = m_pGameInstance->Get_Float_Random(0.f, 1.f);
		m_tParticleInfo.vParticleColor.m128_f32[2] = m_pGameInstance->Get_Float_Random(0.f, 1.f);
		m_tParticleInfo.vParticleColor.m128_f32[3] = 1.f;
	}

	return S_OK;
}

HRESULT CParticle::Initialize_Clone(void* pArg)
{
	/*
	구 확산, 직선 확산

	방향을 가지는 직선 확산
	
	중력의 영햑을 받는지에 대한 여부
	디스토션 효과
	
	몸에 따라다닐지 / 안따라다닐지
	*/

	PARTICLE_DESC* pDesc = static_cast<PARTICLE_DESC*>(pArg);

	m_iShaderPass = pDesc->iShaderPass;
	m_iParticleIdx = pDesc->iParticleIdx;

	m_fDuration = pDesc->fDuration;
	if (m_fDuration != -1.f)
		m_bDuration = TRUE;
	
	if (pDesc->pParentBoneMatrix != nullptr)
	{
		m_pParentBoneMatrix = pDesc->pParentBoneMatrix;
	}
	if (pDesc->pParentWorldMatrix != nullptr)
	{
		m_pParentWorldMatrix = pDesc->pParentWorldMatrix;
	}
	
	_matrix CombinedMatrix = XMMatrixIdentity();
	
	if (m_pParentWorldMatrix != nullptr)
		CombinedMatrix = XMMatrixRotationAxis({ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(180.f)) * XMLoadFloat4x4(&m_pParentWorldMatrix->Get_WorldFloat4x4());
	
	if (m_pParentBoneMatrix != nullptr)
		CombinedMatrix = XMLoadFloat4x4(m_pParentBoneMatrix) * CombinedMatrix;
	
	/* Correction */
	_vector vCorrection = XMVector4Transform(XMLoadFloat4(&m_tParticleInfo.vCorrectionVector), CombinedMatrix);
	CombinedMatrix.r[3].m128_f32[0] -= vCorrection.m128_f32[0];
	CombinedMatrix.r[3].m128_f32[1] -= vCorrection.m128_f32[1];
	CombinedMatrix.r[3].m128_f32[2] -= vCorrection.m128_f32[2];
	
	XMStoreFloat4x4(&m_WorldMatrix, CombinedMatrix);

	if (false == (m_tParticleInfo.iOption & (0x1) << PARTICLE_INFO::PARTICLE_OPTION::PARTICLE_OPTION_COMBINED_TARGET))
		m_tParticleInfo.bTrail = TRUE;
	
	if (m_tParticleInfo.bTrail == TRUE)
		memcpy(&m_tParticleInfo.vTrailPosition, m_WorldMatrix.m[3], sizeof _float4);
	
	if (FAILED(CParticle::Add_Component(pArg)))
		return E_FAIL;

	/* Frame Anim */
	if (m_tParticleInfo.bFrameAnimation == TRUE)
	{
		m_fMaxCycle = m_tParticleInfo.fFrameAnimCycle;
		m_fCurrCycle = m_tParticleInfo.fFrameAnimCycle;
	}

	m_bFade = m_tParticleInfo.bFade;

	return S_OK;
}

HRESULT CParticle::Render()
{
	if (FAILED(CParticle::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_iShaderPass)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

CParticle* CParticle::Create(PARTICLE_INFO tParticleInfo)
{
	CParticle* pInstance = new CParticle();

	if (FAILED(pInstance->Initialize_Prototype(tParticleInfo)))
	{
		MSG_BOX(TEXT("Failed to Create : CParticle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CParticle* CParticle::Clone(void* pArg)
{
	CParticle* pInstance = new CParticle(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CParticle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle::Free()
{
	__super::Free();

	if (!m_bCloned)
	{
		Safe_Release(m_pGameInstance);
	}
	else
	{
		Safe_Release(m_pShaderCom);
		Safe_Release(m_pTextureCom);
		Safe_Release(m_pVIBufferCom);
	}
}
