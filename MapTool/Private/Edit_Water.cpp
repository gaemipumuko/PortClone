#include "Edit_Water.h"
#include "Transform.h"
#include "Renderer.h"
#include "Shader.h"
#include "VIBuffer_Terrain.h"
#include "Texture.h"
#include "Bounding_AABB.h"
#include "Collider.h"
#include "GameInstance.h"
#include "Camera.h"

CEdit_Water::CEdit_Water(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEdit_Water::CEdit_Water(const CEdit_Water& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEdit_Water::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEdit_Water::Initialize_Clone(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	WATER_DESC* pWaterDesc = (WATER_DESC*)pArg;
	{
		m_iSizeX = pWaterDesc->iSizeX;
		m_iSizeZ = pWaterDesc->iSizeZ;
		m_fGridSpacing = pWaterDesc->fGridSpacing;

		m_vUVScale = pWaterDesc->vUVScale;
		m_fWaveScale = pWaterDesc->fWaveScale;
		m_vNoise = pWaterDesc->vNoise;
		m_vWaveSpeed = pWaterDesc->vWaveSpeed;
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_WorldFloat4x4(pWaterDesc->vWorldMatrix);

	if (FAILED(Initialize_Wave()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEdit_Water::PreviousTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CEdit_Water::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down('C'))
		m_bWire = !m_bWire;

	if (m_pGameInstance->Key_Down('V'))
		m_bShow = !m_bShow;

	Invalidate_UV(fTimeDelta);

	if (FAILED(Disturb_Wave(fTimeDelta)))
		return E_FAIL;

	if (FAILED(Simulation_Wave(fTimeDelta)))
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

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

HRESULT CEdit_Water::LateTick(_float fTimeDelta)
{
	m_pVIBufferCom->Culling_QuadTree(m_pTransformCom->Get_WorldMatrix());

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);

	return S_OK;
}

HRESULT CEdit_Water::Render()
{
	if (true == m_bShow) 
	{
		if (true == m_bWire) 
		{
			if (FAILED(Bind_ShaderResources()))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(1)))
				return E_FAIL;

			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}

		else 
		{
			if (FAILED(Bind_ShaderResources()))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(3)))
				return E_FAIL;

			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}
	}

	return S_OK;
}

_float4 CEdit_Water::Get_Position()
{
	_float4 vPosition;
	XMStoreFloat4(&vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return vPosition;
}

_float4x4 CEdit_Water::Get_WorldMatrix()
{
	return m_pTransformCom->Get_WorldFloat4x4();
}

void CEdit_Water::Set_WaveScale(_float* pWaveScale)
{
	m_fWaveScale = *pWaveScale;
}

void CEdit_Water::Set_UVScale(_float* pUVScale)
{
	m_vUVScale.x = *(pUVScale);
	m_vUVScale.y = *(pUVScale + 1);
	m_vUVScale.z = *(pUVScale + 2);
}

void CEdit_Water::Set_Noise(_float* pNoise)
{
	m_vNoise.x = *(pNoise);
	m_vNoise.y = *(pNoise + 1);
	m_vNoise.z = *(pNoise + 2);
}

void CEdit_Water::Set_WaveSpeed(_float* pSpeed)
{
	m_vWaveSpeed.x = *(pSpeed);
	m_vWaveSpeed.y = *(pSpeed + 1);
}

HRESULT CEdit_Water::Add_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Terrain"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	m_pVIBufferCom = CVIBuffer_Terrain::Create(m_pDevice, m_pContext, m_iSizeX, m_iSizeZ, m_fGridSpacing);
	m_Components.emplace(TEXT("Com_VIBuffer"), m_pVIBufferCom);

	Safe_AddRef(m_pVIBufferCom);

	m_pVIBufferCom->Initialize_QuadTree();

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_EDIT, TEXT("Prototype_Component_Texture_Water"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC tBoundingDesc = {};
	{
		tBoundingDesc.vCenter = _float3(0.0f, 0.0f, 0.0f);
		tBoundingDesc.vExtents = _float3(0.5f, 0.5f, 0.5f);
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &tBoundingDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEdit_Water::Bind_ShaderResources()
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

HRESULT CEdit_Water::Initialize_Wave()
{
	m_iNumVertices = m_pVIBufferCom->Get_NumVertices();

	m_fWaveStep = 0.25f;

	/*
	* dx = 1.0f
	* dt = 0.03f
	* speed = 3.25f
	* damping = 0.4f
	*/

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

	//_float fHalfWidth = (m_iSizeX - 1) * m_fSpatialStep * 0.5f;
	//_float fHalfDepth = (m_iSizeZ - 1) * m_fSpatialStep * 0.5f;

	_float3* pVertices = m_pVIBufferCom->Get_VerticesPos();
	_float3* pNormals = m_pVIBufferCom->Get_Normals();
	_float2* pTexCoord = m_pVIBufferCom->Get_TexCoord();

	for (_uint i = 0; i < m_iSizeZ; i++) 
	{
		//_float fZ = fHalfDepth - i * m_fSpatialStep;

		for (_uint j = 0; j < m_iSizeX; j++) 
		{
			//_float fX = -fHalfWidth + j * m_fSpatialStep;
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

void CEdit_Water::Invalidate_UV(_float fTimeDelta)
{
	m_vUVOffset.x += m_vWaveSpeed.x * fTimeDelta;
	m_vUVOffset.y += m_vWaveSpeed.y * fTimeDelta;
}

HRESULT CEdit_Water::Disturb_Wave(_float fTimeDelta)
{
	m_fWaveTime += fTimeDelta;

	if (m_fWaveTime > m_fWaveStep) 
	{
		m_fWaveTime = 0.0f;

		_uint iRow = 5 + rand() % (m_iSizeZ - 10);
		_uint iCol = 5 + rand() % (m_iSizeX - 10);

		_float fMagnitude = (m_fWaveScale + (_float)(rand() % 10) / 10.0f) * fTimeDelta;

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

HRESULT CEdit_Water::Simulation_Wave(_float fTimeDelta)
{
	m_fTime += fTimeDelta;

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

CEdit_Water* CEdit_Water::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEdit_Water* pInstance = new CEdit_Water(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CEdit_Water"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEdit_Water::Clone(void* pArg)
{
	CEdit_Water* pInstance = new CEdit_Water(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEdit_Water"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEdit_Water::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);

	m_PrevSolution.clear();
	m_CurSolution.clear();
	m_Normals.clear();
	m_TangentX.clear();
	m_TexCoord.clear();
}
