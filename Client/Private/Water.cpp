#include "stdafx.h"
#include "Water.h"
#include "Transform.h"
#include "Shader.h"
#include "VIBuffer_Terrain.h"
#include "Texture.h"
#include "Camera.h"
#include "GameInstance.h"

CWater::CWater(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CWater::Initialize(WATER_DESC* pWaterDesc)
{
	m_iSizeX = pWaterDesc->iSizeX;
	m_iSizeZ = pWaterDesc->iSizeZ;
	m_vWorldMatrix = pWaterDesc->vWorldMatrix;
	m_fGridSpacing = pWaterDesc->fGridSpacing;
	m_vUVScale = pWaterDesc->vUVScale;
	m_fWaveScale = pWaterDesc->fWaveScale;
	m_vNoise = pWaterDesc->vNoise;
	m_vWaveSpeed = pWaterDesc->vWaveSpeed;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Initialize_Wave()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWater::Simulate_Wave(_float fTimeDelta)
{
	if (FAILED(Invalidate_UV(&fTimeDelta)))
		return E_FAIL;

	if (FAILED(Disturb_Wave(&fTimeDelta)))
		return E_FAIL;

	if (FAILED(Compute_Wave(&fTimeDelta)))
		return E_FAIL;

	D3D11_MAPPED_SUBRESOURCE tMSRC = {};

	m_pContext->Map(m_pVIBufferCom->Get_VB(), 0, D3D11_MAP_WRITE_DISCARD, 0, &tMSRC);

	VTXNORTEX* pVertices = (VTXNORTEX*)tMSRC.pData;

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		pVertices[i].vPosition = m_CurSolution[i];
		pVertices[i].vNormal = m_Normals[i];
		pVertices[i].vTexcoord = m_TexCoord[i];
	}

	m_pContext->Unmap(m_pVIBufferCom->Get_VB(), 0);

	return S_OK;
}

HRESULT CWater::Invalidate_Culling()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	_float4 vPosition;
	XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (vPosition.x > 0.0f || vPosition.z > 0.0f)
		m_pVIBufferCom->Culling_QuadTree(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CWater::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(VTXNORTEX_WATER)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWater::Add_Components()
{
	m_pTransformCom = (CTransform*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"));

	if (nullptr == m_pTransformCom)
		return E_FAIL;

	m_pTransformCom->Set_WorldFloat4x4(m_vWorldMatrix);

	m_pShaderCom = (CShader*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"));

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	m_pVIBufferCom = CVIBuffer_Terrain::Create(m_pDevice, m_pContext, m_iSizeX, m_iSizeZ, m_fGridSpacing);

	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	_float4 vPosition;
	XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (vPosition.x > 0.0f || vPosition.z > 0.0f) 
	{
		if (FAILED(m_pVIBufferCom->Initialize_QuadTree()))
			return E_FAIL;
	}

	m_pTextureCom = (CTexture*)m_pGameInstance->Clone_Component_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Water"));

	if (nullptr == m_pTextureCom)
		return E_FAIL;

	return S_OK;
}

HRESULT CWater::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_float fCamFar = static_cast<CCamera*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), L"Layer_Camera"))->Get_CamFar();

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", &fCamFar, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vScale", &m_vUVScale, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Noise", &m_vNoise, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vOffset", &m_vUVOffset, sizeof(_float2))))
		return E_FAIL;

	return S_OK;
}

HRESULT CWater::Initialize_Wave()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	m_iNumVertices = m_pVIBufferCom->Get_NumVertices();
	m_fWaveStep = 0.25f;
	m_fTimeStep = 0.03f;
	m_fSpatialStep = 1.0f;

	_float d = 0.4f * m_fTimeStep + 2.0f;
	_float e = (3.25f * 3.25f) * (m_fTimeStep * m_fTimeStep) / (m_fSpatialStep * m_fSpatialStep);

	m_fK1 = (0.4f * m_fTimeStep - 2.0f) / d;
	m_fK2 = (4.0f - 8.0f * e) / d;
	m_fK3 = (2.0f * e) / d;

	m_PrevSolution.reserve(m_iSizeX * m_iSizeZ);
	m_CurSolution.reserve(m_iSizeX * m_iSizeZ);
	m_Normals.reserve(m_iSizeX * m_iSizeZ);
	m_TexCoord.reserve(m_iSizeX * m_iSizeZ);
	m_TangentX.reserve(m_iSizeX * m_iSizeZ);

	_float3* pVertices = m_pVIBufferCom->Get_VerticesPos();
	_float3* pNormals = m_pVIBufferCom->Get_Normals();
	_float2* pTexCoord = m_pVIBufferCom->Get_TexCoord();

	for (_uint i = 0; i < m_iSizeZ; i++)
	{
		for (_uint j = 0; j < m_iSizeX; j++)
		{
			_uint iIndex = i * m_iSizeX + j;

			_float3 vSolution = pVertices[iIndex];
			_float3 vNormal = pNormals[iIndex];
			_float2 vTexCoord = pTexCoord[iIndex];
			_float3 vTangent = _float3(1.0f, 0.0f, 0.0f);

			m_PrevSolution.push_back(vSolution);
			m_CurSolution.push_back(vSolution);
			m_Normals.push_back(vNormal);
			m_TexCoord.push_back(vTexCoord);
			m_TangentX.push_back(vTangent);
		}
	}

	return S_OK;
}

HRESULT CWater::Invalidate_UV(_float* pTimeDelta)
{
	if (nullptr == pTimeDelta)
		return E_FAIL;

	m_vUVOffset.x += m_vWaveSpeed.x * (*pTimeDelta);
	m_vUVOffset.y += m_vWaveSpeed.y * (*pTimeDelta);

	return S_OK;
}

HRESULT CWater::Disturb_Wave(_float* pTimeDelta)
{
	if (nullptr == pTimeDelta)
		return E_FAIL;

	//m_fWaveTime += *pTimeDelta;
	m_fWaveTime += 0.02f;

	if (m_fWaveTime > m_fWaveStep) 
	{
		m_fWaveTime = 0.0f;

		_uint iRow = 5 + rand() % (m_iSizeZ - 10);
		_uint iCol = 5 + rand() % (m_iSizeX - 10);

		//_float fMagnitude = (m_fWaveScale + (_float)(rand() % 10) / 10.0f) * (*pTimeDelta);
		_float fMagnitude = (m_fWaveScale + (_float)(rand() % 10) / 10.0f) * 0.02f;

		float fHalfMagnitude = 0.5f * fMagnitude;

		// Disturb the ijth vertex height and its neighbors.
		m_CurSolution[iRow * m_iSizeX + iCol].y += fMagnitude;
		m_CurSolution[iRow * m_iSizeX + iCol + 1].y += fHalfMagnitude;
		m_CurSolution[iRow * m_iSizeX + iCol - 1].y += fHalfMagnitude;
		m_CurSolution[(iRow + 1) * m_iSizeX + iCol].y += fHalfMagnitude;
		m_CurSolution[(iRow - 1) * m_iSizeX + iCol].y += fHalfMagnitude;
	}

	return S_OK;
}

HRESULT CWater::Compute_Wave(_float* pTimeDelta)
{
	if (nullptr == pTimeDelta)
		return E_FAIL;

	//m_fTime += (*pTimeDelta);
	m_fTime += 0.02f;

	if (m_fTime >= m_fTimeStep)
	{
		for (_uint i = 1; i < m_iSizeZ - 1; i++)
		{
			for (_uint j = 1; j < m_iSizeX - 1; j++)
			{
				m_PrevSolution[i * m_iSizeX + j].y =
					m_fK1 * m_PrevSolution[i * m_iSizeX + j].y +
					m_fK2 * m_CurSolution[i * m_iSizeX + j].y +
					m_fK3 * (m_CurSolution[(i + 1) * m_iSizeX + j].y +
						m_CurSolution[(i - 1) * m_iSizeX + j].y +
						m_CurSolution[i * m_iSizeX + j + 1].y +
						m_CurSolution[i * m_iSizeX + j - 1].y);
			}
		}

		std::swap(m_PrevSolution, m_CurSolution);
		m_fTime = 0.0f;

		for (_uint i = 1; i < m_iSizeZ - 1; i++)
		{
			for (_uint j = 1; j < m_iSizeX - 1; j++)
			{
				_float fLeft = m_CurSolution[i * m_iSizeX + j - 1].y;
				_float fRight = m_CurSolution[i * m_iSizeX + j + 1].y;
				_float fTop = m_CurSolution[(i - 1) * m_iSizeX + j].y;
				_float fBottom = m_CurSolution[(i + 1) * m_iSizeX + j].y;
				m_Normals[i * m_iSizeX + j].x = -fRight + fLeft;
				m_Normals[i * m_iSizeX + j].y = 2.0f * m_fSpatialStep;
				m_Normals[i * m_iSizeX + j].z = fBottom - fTop;

				XMVECTOR Normal = XMVector3Normalize(XMLoadFloat3(&m_Normals[i * m_iSizeX + j]));
				XMStoreFloat3(&m_Normals[i * m_iSizeX + j], Normal);



				m_TangentX[i * m_iSizeX + j] = XMFLOAT3(2.0f * m_fSpatialStep, fRight - fLeft, 0.0f);
				XMVECTOR TangentX = XMVector3Normalize(XMLoadFloat3(&m_TangentX[i * m_iSizeX + j]));
				XMStoreFloat3(&m_TangentX[i * m_iSizeX + j], TangentX);
			}
		}

		for (_uint i = 0; i < m_iNumVertices; i++)
		{
			m_TexCoord[i].x = 0.5f + m_CurSolution[i].x / m_iSizeX;
			m_TexCoord[i].y = 0.5f - m_CurSolution[i].z / m_iSizeZ;
		}
	}

	return S_OK;
}

CWater* CWater::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, WATER_DESC* pWaterDesc)
{
	CWater* pInstance = new CWater(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pWaterDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CWater"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWater::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}
