#include "VIBuffer_Particle_Instancing.h"

#include "GameInstance.h"

#define FADETIME 0.5f

CVIBuffer_Particle_Instancing::CVIBuffer_Particle_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instancing(pDevice, pContext)
{
}

CVIBuffer_Particle_Instancing::CVIBuffer_Particle_Instancing(const CVIBuffer_Particle_Instancing& rhs)
	: CVIBuffer_Instancing(rhs)
{
}

HRESULT CVIBuffer_Particle_Instancing::Invalidate_Instance(_float fTimeDelta, PARTICLE_INFO tParticleInfo)
{
	_int	iNumCreation = { 0 };

	if (tParticleInfo.iParticleEmitRate != 0)
	{
		m_fNextCreateTime -= fTimeDelta;

		if (m_fNextCreateTime < 0.f)
		{
			m_fNextCreateTime = tParticleInfo.fParticleCreateTime;
			iNumCreation = tParticleInfo.iParticleEmitRate;
		}
	}

	D3D11_MAPPED_SUBRESOURCE		SubResource = {};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t iIndex = 0; iIndex < tParticleInfo.iMaxParticle; ++iIndex)
	{
		if (tParticleInfo.iParticleEmitRate == 0)
		{
			if (true == OptionCheck(tParticleInfo.iOption, PARTICLE_INFO::PARTICLE_OPTION_ROTATION)) /* 중심을 기준으로 회전 */
			{
				_float4 vCurrentPosition = static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vPosition; /////////////
				_vector vTemp = XMLoadFloat4(&tParticleInfo.vParticlePosition) + XMLoadFloat4(&tParticleInfo.vTrailPosition);
				vTemp.m128_f32[3] = 0.f;
				XMStoreFloat4(&vCurrentPosition, XMLoadFloat4(&vCurrentPosition) - vTemp);

				_matrix RotationMatrixX = XMMatrixRotationX(XMConvertToRadians(tParticleInfo.vRotationXYZ.x) * fTimeDelta);
				_matrix RotationMatrixY = XMMatrixRotationY(XMConvertToRadians(tParticleInfo.vRotationXYZ.y) * fTimeDelta);
				_matrix RotationMatrixZ = XMMatrixRotationZ(XMConvertToRadians(tParticleInfo.vRotationXYZ.z) * fTimeDelta);

				_matrix LocalRotationMatrix = RotationMatrixX * RotationMatrixY * RotationMatrixZ;

				_float4 RotationPos = {};
				XMStoreFloat4(&RotationPos, XMVector3TransformCoord((XMLoadFloat4(&vCurrentPosition)), LocalRotationMatrix) + vTemp);

				static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vPosition = RotationPos;
			}

			static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vPosition.x += m_pDirections[iIndex].x * m_pSpeeds[iIndex] * fTimeDelta;
			static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vPosition.y += m_pDirections[iIndex].y * m_pSpeeds[iIndex] * fTimeDelta;
			static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vPosition.z += m_pDirections[iIndex].z * m_pSpeeds[iIndex] * fTimeDelta;

			if (true == OptionCheck(tParticleInfo.iOption, PARTICLE_INFO::PARTICLE_OPTION::PARTICLE_OPTION_ACCEL))
				m_pSpeeds[iIndex] = max(m_pSpeeds[iIndex] + tParticleInfo.fAccel, 0.f);


			static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vRight.z = 1.f; /////////////////////////////////////////////////

			m_pLifeTimes[iIndex].x += fTimeDelta;
			if (m_pLifeTimes[iIndex].y < m_pLifeTimes[iIndex].x)
			{
				if (m_pLifeTimes[iIndex].y + FADETIME < m_pLifeTimes[iIndex].x)
				{
					m_pLifeTimes[iIndex].x = 0.f;
					if (nullptr != m_pGravityTime)
						m_pGravityTime[iIndex] = 0.f;

					_float fPosX = { }, fPosY = { }, fPosZ = { };
					_float fDirX = { }, fDirY = { }, fDirZ = { };

					if (tParticleInfo.eType == PARTICLE_INFO::PARTICLE_TYPE::PARTICLE_SPHERE_DIV)
					{
						_float fRadius = m_pGameInstance->Get_Float_Random(0.f, tParticleInfo.vParticlePosition.w);
						_float fPhi = m_pGameInstance->Get_Float_Random(0.f, 1.f) * XM_2PI;
						_float fTheta = XMScalarACos(m_pGameInstance->Get_Float_Random(0.f, 1.f) * 2.f - 1.f);

						fPosX = tParticleInfo.vParticlePosition.x + tParticleInfo.vTrailPosition.x + fRadius * sin(fPhi) * cos(fTheta);
						fPosY = tParticleInfo.vParticlePosition.y + tParticleInfo.vTrailPosition.y + fRadius * sin(fPhi) * sin(fTheta);
						fPosZ = tParticleInfo.vParticlePosition.z + tParticleInfo.vTrailPosition.z + fRadius * cos(fPhi);
					}
					else if (tParticleInfo.eType == PARTICLE_INFO::PARTICLE_TYPE::PARTICLE_SPHERE_CONV)
					{
						_float fRadius = m_pGameInstance->Get_Float_Random(tParticleInfo.vParticlePosition.w * 0.6f, tParticleInfo.vParticlePosition.w);
						_float fPhi = m_pGameInstance->Get_Float_Random(0.f, 1.f) * XM_2PI;
						_float fTheta = XMScalarACos(m_pGameInstance->Get_Float_Random(0.f, 1.f) * 2.f - 1.f);

						fPosX = tParticleInfo.vParticlePosition.x + tParticleInfo.vTrailPosition.x + fRadius * sin(fPhi) * cos(fTheta);
						fPosY = tParticleInfo.vParticlePosition.y + tParticleInfo.vTrailPosition.y + fRadius * sin(fPhi) * sin(fTheta);
						fPosZ = tParticleInfo.vParticlePosition.z + tParticleInfo.vTrailPosition.z + fRadius * cos(fPhi);

						_float3 vCenDir = {};
						_vector vCenter = XMLoadFloat4(&tParticleInfo.vParticlePosition) + XMLoadFloat4(&tParticleInfo.vTrailPosition);
						_vector vFirstPos = XMVectorSet(fPosX, fPosY, fPosZ, 0.f);
						XMStoreFloat3(&vCenDir, vCenter - vFirstPos);

						fDirX = vCenDir.x;
						fDirY = vCenDir.y;
						fDirZ = vCenDir.z;

						m_pDirections[iIndex] = _float3(fDirX, fDirY, fDirZ);
					}
					static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vPosition.x = fPosX;
					static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vPosition.y = fPosY;
					static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vPosition.z = fPosZ;

					_float fSize = m_pGameInstance->Get_Float_Random(tParticleInfo.vParticleSize.x, tParticleInfo.vParticleSize.y);
					static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vRight.z = 1.f;
					static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vRight.x = fSize;
					static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vUp.y = fSize;
				}
				else
				{
					if (nullptr != m_pGravityTime)
						m_pGravityTime[iIndex] = 0.f;
					static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vRight.x *= 0.99f;
					static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vUp.y *= 0.99f;
					static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vRight.z = max(0.f, m_pLifeTimes[iIndex].y + FADETIME - m_pLifeTimes[iIndex].x) * (1.f / FADETIME);
				}
				
			}
		}
		else
		{
			if (m_pValids[iIndex] == TRUE)
			{
				if (true == OptionCheck(tParticleInfo.iOption, PARTICLE_INFO::PARTICLE_OPTION_GRAVITY)) /* 중력 */
				{
					m_pGravityTime[iIndex] += fTimeDelta;
					static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vPosition.y -= tParticleInfo.fGravAcc * m_pGravityTime[iIndex] * fTimeDelta;
				}

				if (true == OptionCheck(tParticleInfo.iOption, PARTICLE_INFO::PARTICLE_OPTION_ROTATION)) /* 중심을 기준으로 회전 */
				{
					_float4 vCurrentPosition = static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vPosition; /////////////
					_vector vTemp = XMLoadFloat4(&tParticleInfo.vParticlePosition) + XMLoadFloat4(&tParticleInfo.vTrailPosition);
					vTemp.m128_f32[3] = 0.f;
					XMStoreFloat4(&vCurrentPosition, XMLoadFloat4(&vCurrentPosition) - vTemp);

					_matrix RotationMatrixX = XMMatrixRotationX(XMConvertToRadians(tParticleInfo.vRotationXYZ.x) * fTimeDelta);
					_matrix RotationMatrixY = XMMatrixRotationY(XMConvertToRadians(tParticleInfo.vRotationXYZ.y) * fTimeDelta);
					_matrix RotationMatrixZ = XMMatrixRotationZ(XMConvertToRadians(tParticleInfo.vRotationXYZ.z) * fTimeDelta);

					_matrix LocalRotationMatrix = RotationMatrixX * RotationMatrixY * RotationMatrixZ;

					_float4 RotationPos = {};
					XMStoreFloat4(&RotationPos, XMVector3TransformCoord((XMLoadFloat4(&vCurrentPosition)), LocalRotationMatrix) + vTemp);

					static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vPosition = RotationPos;
				}

				static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vPosition.x += m_pDirections[iIndex].x * m_pSpeeds[iIndex] * fTimeDelta;
				static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vPosition.y += m_pDirections[iIndex].y * m_pSpeeds[iIndex] * fTimeDelta;
				static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vPosition.z += m_pDirections[iIndex].z * m_pSpeeds[iIndex] * fTimeDelta;

				if (true == OptionCheck(tParticleInfo.iOption, PARTICLE_INFO::PARTICLE_OPTION::PARTICLE_OPTION_ACCEL))
					m_pSpeeds[iIndex] = max(m_pSpeeds[iIndex] + tParticleInfo.fAccel, 0.f);

				static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vRight.z = 1.f; /////////////////////////////////////////////////

				m_pLifeTimes[iIndex].x += fTimeDelta;
				if (m_pLifeTimes[iIndex].y < m_pLifeTimes[iIndex].x)
				{
					if (m_pLifeTimes[iIndex].y + FADETIME < m_pLifeTimes[iIndex].x)
					{
						static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vRight = _float4(0.f, 0.f, 0.f, 0.f);
						static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vUp = _float4(0.f, 0.f, 0.f, 0.f);
						static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vLook = _float4(0.f, 0.f, 0.f, 0.f);
						static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vPosition = _float4(0.f, 0.f, 0.f, 0.f);

						m_pSpeeds[iIndex] = m_pGameInstance->Get_Float_Random(tParticleInfo.vParticleSpeed.x, tParticleInfo.vParticleSpeed.y);
						m_pLifeTimes[iIndex].x = 0.f;
						m_pValids[iIndex] = FALSE;
						if(nullptr != m_pGravityTime)
							m_pGravityTime[iIndex] = 0.f;

						_float fSize = m_pGameInstance->Get_Float_Random(tParticleInfo.vParticleSize.x, tParticleInfo.vParticleSize.y);
						m_pSpeeds[iIndex] = m_pGameInstance->Get_Float_Random(tParticleInfo.vParticleSpeed.x, tParticleInfo.vParticleSpeed.y);
						static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vRight.z = 1.f;
					}
					else
					{
						static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vRight.x *= 0.99f;
						static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vUp.y *= 0.99f;
						static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vRight.z = max(0.f, m_pLifeTimes[iIndex].y + FADETIME - m_pLifeTimes[iIndex].x) * (1.f / FADETIME);
					}
				}
			}
			else
			{
				if (iNumCreation > 0)
				{
					_float fPosX = { }, fPosY = { }, fPosZ = { };
					_float fDirX = { }, fDirY = { }, fDirZ = { };

					if (tParticleInfo.eType == PARTICLE_INFO::PARTICLE_TYPE::PARTICLE_SPHERE_DIV)
					{
						_float fRadius = m_pGameInstance->Get_Float_Random(0.f, tParticleInfo.vParticlePosition.w);
						_float fPhi = m_pGameInstance->Get_Float_Random(0.f, 1.f) * XM_2PI;
						_float fTheta = XMScalarACos(m_pGameInstance->Get_Float_Random(0.f, 1.f) * 2.f - 1.f);

						fPosX = tParticleInfo.vParticlePosition.x + tParticleInfo.vTrailPosition.x + fRadius * sin(fPhi) * cos(fTheta);
						fPosY = tParticleInfo.vParticlePosition.y + tParticleInfo.vTrailPosition.y + fRadius * sin(fPhi) * sin(fTheta);
						fPosZ = tParticleInfo.vParticlePosition.z + tParticleInfo.vTrailPosition.z + fRadius * cos(fPhi);
					}
					else if (tParticleInfo.eType == PARTICLE_INFO::PARTICLE_TYPE::PARTICLE_SPHERE_CONV)
					{
						_float fRadius = m_pGameInstance->Get_Float_Random(tParticleInfo.vParticlePosition.w * 0.6f, tParticleInfo.vParticlePosition.w);
						_float fPhi = m_pGameInstance->Get_Float_Random(0.f, 1.f) * XM_2PI;
						_float fTheta = XMScalarACos(m_pGameInstance->Get_Float_Random(0.f, 1.f) * 2.f - 1.f);

						fPosX = tParticleInfo.vParticlePosition.x + tParticleInfo.vTrailPosition.x + fRadius * sin(fPhi) * cos(fTheta);
						fPosY = tParticleInfo.vParticlePosition.y + tParticleInfo.vTrailPosition.y + fRadius * sin(fPhi) * sin(fTheta);
						fPosZ = tParticleInfo.vParticlePosition.z + tParticleInfo.vTrailPosition.z + fRadius * cos(fPhi);

						_float3 vCenDir = {};
						_vector vCenter = XMLoadFloat4(&tParticleInfo.vParticlePosition) + XMLoadFloat4(&tParticleInfo.vTrailPosition); ////////////////////////////
						_vector vFirstPos = XMVectorSet(fPosX, fPosY, fPosZ, 0.f);
						XMStoreFloat3(&vCenDir, vCenter - vFirstPos);

						fDirX = vCenDir.x;
						fDirY = vCenDir.y;
						fDirZ = vCenDir.z;

						m_pDirections[iIndex] = _float3(fDirX, fDirY, fDirZ);
					}

					_float fScale = m_pGameInstance->Get_Float_Random(tParticleInfo.vParticleSize.x, tParticleInfo.vParticleSize.y);

					static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vRight = _float4(fScale, 0.f, 0.f, 0.f);
					static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vUp = _float4(0.f, fScale, 0.f, 0.f);
					static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vLook = _float4(0.f, 0.f, 1.f, 0.f);
					static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vPosition = _float4(fPosX, fPosY, fPosZ, 1.f);
					m_pSpeeds[iIndex] = m_pGameInstance->Get_Float_Random(tParticleInfo.vParticleSpeed.x, tParticleInfo.vParticleSpeed.y);

					m_pValids[iIndex] = TRUE;
					--iNumCreation;
				}
			}
		}

		/* Specific Value (For. Shader) */
		static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vUp.w		= m_pGameInstance->Get_Float_Random(0.f, 1.f);		// UNORM Random
		static_cast<VTXINSTANCE*>(SubResource.pData)[iIndex].vLook.w	= m_pLifeTimes[iIndex].x / m_pLifeTimes[iIndex].y;	// UNORM LifeTime
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	return S_OK;
}

HRESULT CVIBuffer_Particle_Instancing::Initialize_Prototype(_uint iNumInstance)
{
	m_iNumVertexBuffers = 2;

	m_iInstanceStride = sizeof(VTXINSTANCE);
	m_iVertexStride = sizeof(VTXPOINT);
	m_iIndexStride = 2;

	m_iNumInstance = iNumInstance;
	m_iIndexCountPerInstance = 1;

	m_iNumVertices = 1; /* 정점 1개 */
	m_iNumIndices = m_iNumInstance * m_iIndexCountPerInstance; /* 인덱스 개수 */

	m_eIndexFormat = DXGI_FORMAT_R16_UINT; /* 16비트(2바이트) 부호 없는 정수 */
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST; /* 꼭지점 데이터를 포인트 목록으로 해석 */

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	ZeroMemory(&m_InitialData, sizeof m_InitialData);

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;	/* 버퍼의 총 크기 */
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;					/* 정적(DEFAULT) or 동적 버퍼(DYNAMIC) */
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;			/* 무슨 용도로 장치에 바인딩할 것인지 */
	m_BufferDesc.CPUAccessFlags = 0;							/* 동적 버퍼일 경우 CPU가 값을 읽거나 쓸 수 있을 것인지 (정적 : 0) */
	m_BufferDesc.MiscFlags = 0;									/* (정적 : 0) */
	m_BufferDesc.StructureByteStride = m_iVertexStride;			/* 정점 하나의 크기 */

	VTXPOINT* pVertices = new VTXPOINT[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOINT) * m_iNumVertices);

	pVertices->vPosition	= _float3(0.f, 0.f, 0.f);
	pVertices->vPSize		= _float2(1.f, 1.f);

	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
#pragma endregion

#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	ZeroMemory(&m_InitialData, sizeof m_InitialData);

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iIndexStride;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion

#pragma region INSTANCE_BUFFER
	ZeroMemory(&m_InstanceBufferDesc, sizeof m_InstanceBufferDesc);
	ZeroMemory(&m_InstanceInitialData, sizeof m_InstanceInitialData);

	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;

	VTXINSTANCE* pVertexInstance = new VTXINSTANCE[m_iNumInstance];
	ZeroMemory(pVertexInstance, sizeof(VTXINSTANCE) * m_iNumInstance);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertexInstance[i].vRight = _float4(0.1f, 0.f, 0.f, 0.f);
		pVertexInstance[i].vUp = _float4(0.f, 0.1f, 0.f, 0.f);
		pVertexInstance[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		pVertexInstance[i].vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	}

	m_InstanceInitialData.pSysMem = pVertexInstance;
#pragma endregion

    return S_OK;
}

HRESULT CVIBuffer_Particle_Instancing::Initialize_Clone(void* pArg)
{
	PARTICLE_INFO* pParticleInfo = static_cast<PARTICLE_INFO*>(pArg);
	if (pParticleInfo != nullptr)
	{
		m_pValids = new _bool[pParticleInfo->iMaxParticle];
		fill_n(m_pValids, pParticleInfo->iMaxParticle, FALSE);

		m_pSpeeds = new _float[pParticleInfo->iMaxParticle];
		m_pLifeTimes = new _float2[pParticleInfo->iMaxParticle];
		m_pDirections = new _float3[pParticleInfo->iMaxParticle];

		if (true == OptionCheck(pParticleInfo->iOption, PARTICLE_INFO::PARTICLE_OPTION::PARTICLE_OPTION_GRAVITY))
		{
			m_pGravityTime = new _float[pParticleInfo->iMaxParticle];
			fill_n(m_pGravityTime, pParticleInfo->iMaxParticle, 0.f);
		}

		VTXINSTANCE* pVertices = (VTXINSTANCE*)m_InstanceInitialData.pSysMem;

		_int iNumCreation = { 0 };
		m_fNextCreateTime = pParticleInfo->fParticleCreateTime;

		for (size_t iIndex = 0; iIndex < MAX_PARTICLE; ++iIndex)
		{
			if (iIndex < pParticleInfo->iMaxParticle)
			{
				if (pParticleInfo->iParticleEmitRate == 0 || iNumCreation < pParticleInfo->iParticleEmitRate)
				{
					_float fPosX = { }, fPosY = { }, fPosZ = { };
					_float fDirX = { }, fDirY = { }, fDirZ = { };
					_float fScale = m_pGameInstance->Get_Float_Random(pParticleInfo->vParticleSize.x, pParticleInfo->vParticleSize.y);
					_float fSpeed = m_pGameInstance->Get_Float_Random(pParticleInfo->vParticleSpeed.x, pParticleInfo->vParticleSpeed.y);
					_float fLifeTime = m_pGameInstance->Get_Float_Random(pParticleInfo->vParticleLifeTime.x, pParticleInfo->vParticleLifeTime.y);

					if (pParticleInfo->eType == PARTICLE_INFO::PARTICLE_TYPE::PARTICLE_SPHERE_DIV)
					{
						_float fRadius = m_pGameInstance->Get_Float_Random(0.f, pParticleInfo->vParticlePosition.w);
						_float fPhi = m_pGameInstance->Get_Float_Random(0.f, 1.f) * XM_2PI;
						_float fTheta = XMScalarACos(m_pGameInstance->Get_Float_Random(0.f, 1.f) * 2.f - 1.f);

						fPosX = pParticleInfo->vParticlePosition.x + pParticleInfo->vTrailPosition.x + fRadius * sin(fPhi) * cos(fTheta);
						fPosY = pParticleInfo->vParticlePosition.y + pParticleInfo->vTrailPosition.y + fRadius * sin(fPhi) * sin(fTheta);
						fPosZ = pParticleInfo->vParticlePosition.z + pParticleInfo->vTrailPosition.z + fRadius * cos(fPhi);

						fDirX = m_pGameInstance->Get_Float_Random(pParticleInfo->vDirectionMin.x, pParticleInfo->vDirectionMax.x);
						fDirY = m_pGameInstance->Get_Float_Random(pParticleInfo->vDirectionMin.y, pParticleInfo->vDirectionMax.y);
						fDirZ = m_pGameInstance->Get_Float_Random(pParticleInfo->vDirectionMin.z, pParticleInfo->vDirectionMax.z);
					}
					else if (pParticleInfo->eType == PARTICLE_INFO::PARTICLE_TYPE::PARTICLE_SPHERE_CONV)
					{
						_float fRadius = m_pGameInstance->Get_Float_Random(pParticleInfo->vParticlePosition.w * 0.6f, pParticleInfo->vParticlePosition.w);
						_float fPhi = m_pGameInstance->Get_Float_Random(0.f, 1.f) * XM_2PI;
						_float fTheta = XMScalarACos(m_pGameInstance->Get_Float_Random(0.f, 1.f) * 2.f - 1.f);

						fPosX = pParticleInfo->vParticlePosition.x + pParticleInfo->vTrailPosition.x + fRadius * sin(fPhi) * cos(fTheta);
						fPosY = pParticleInfo->vParticlePosition.y + pParticleInfo->vTrailPosition.y + fRadius * sin(fPhi) * sin(fTheta);
						fPosZ = pParticleInfo->vParticlePosition.z + pParticleInfo->vTrailPosition.z + fRadius * cos(fPhi);

						_float3 vCenDir = {};
						_vector vCenter = XMVectorSet(pParticleInfo->vDirectionMin.x, pParticleInfo->vDirectionMin.y, pParticleInfo->vDirectionMin.z, 0.f);
						_vector vFirstPos = XMVectorSet(fPosX, fPosY, fPosZ, 0.f);
						XMStoreFloat3(&vCenDir, vCenter - vFirstPos);

						fDirX = vCenDir.x;
						fDirY = vCenDir.y;
						fDirZ = vCenDir.z;
					}

					pVertices[iIndex].vRight = _float4(fScale, 0.f, 0.f, 0.f);
					pVertices[iIndex].vUp = _float4(0.f, fScale, 0.f, 0.f);
					pVertices[iIndex].vLook = _float4(0.f, 0.f, 1.f, 0.f);
					pVertices[iIndex].vPosition = _float4(fPosX, fPosY, fPosZ, 1.f);

					++iNumCreation;
					m_pValids[iIndex] = TRUE;
					m_pSpeeds[iIndex] = fSpeed;
					m_pLifeTimes[iIndex] = _float2(0.0f, fLifeTime);
					m_pDirections[iIndex] = _float3(fDirX, fDirY, fDirZ);
				}
				else
				{
					_float fDirX = { }, fDirY = { }, fDirZ = { };
					_float fSpeed = m_pGameInstance->Get_Float_Random(pParticleInfo->vParticleSpeed.x, pParticleInfo->vParticleSpeed.y);
					_float fLifeTime = m_pGameInstance->Get_Float_Random(pParticleInfo->vParticleLifeTime.x, pParticleInfo->vParticleLifeTime.y);

					fDirX = m_pGameInstance->Get_Float_Random(pParticleInfo->vDirectionMin.x, pParticleInfo->vDirectionMax.x);
					fDirY = m_pGameInstance->Get_Float_Random(pParticleInfo->vDirectionMin.y, pParticleInfo->vDirectionMax.y);
					fDirZ = m_pGameInstance->Get_Float_Random(pParticleInfo->vDirectionMin.z, pParticleInfo->vDirectionMax.z);

					pVertices[iIndex].vRight = { 0.f, 0.f, 0.f, 0.f };
					pVertices[iIndex].vUp = { 0.f, 0.f, 0.f, 0.f };
					pVertices[iIndex].vLook = { 0.f, 0.f, 0.f, 0.f };
					pVertices[iIndex].vPosition = { 0.f, 0.f, 0.f, 0.f };

					m_pValids[iIndex] = FALSE;
					m_pSpeeds[iIndex] = fSpeed;
					m_pLifeTimes[iIndex] = _float2(0.0f, fLifeTime);
					m_pDirections[iIndex] = _float3(fDirX, fDirY, fDirZ);
				}
			}
			else
			{
				pVertices[iIndex].vRight = { 0.f, 0.f, 0.f, 0.f };
				pVertices[iIndex].vUp = { 0.f, 0.f, 0.f, 0.f };
				pVertices[iIndex].vLook = { 0.f, 0.f, 0.f, 0.f };
				pVertices[iIndex].vPosition = { 0.f, 0.f, 0.f, 0.f };
			}
		}
	}

#pragma region INSTANCE_BUFFER
	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InstanceInitialData, &m_pVBInstance)))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

CVIBuffer_Particle_Instancing* CVIBuffer_Particle_Instancing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumInstance)
{
	CVIBuffer_Particle_Instancing* pInstance = new CVIBuffer_Particle_Instancing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iNumInstance)))
	{
		MSG_BOX(TEXT("Failed to Create : CVIBuffer_Point_Instancing"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Particle_Instancing::Clone(void* pArg)
{
	CVIBuffer_Particle_Instancing* pInstance = new CVIBuffer_Particle_Instancing(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CVIBuffer_Particle_Instancing"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Particle_Instancing::Free()
{
	__super::Free();

	if (!m_bCloned)
		Safe_Delete_Array(m_InstanceInitialData.pSysMem);
	else
	{
		Safe_Delete_Array(m_pValids);
		Safe_Delete_Array(m_pSpeeds);
		Safe_Delete_Array(m_pLifeTimes);
		Safe_Delete_Array(m_pDirections);

		Safe_Delete_Array(m_pGravityTime);
	}
}
