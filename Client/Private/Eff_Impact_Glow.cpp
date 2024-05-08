#include "stdafx.h"
#include "Eff_Impact_Glow.h"

#include "GameInstance.h"
#include "Player.h"

CImpact_Glow::CImpact_Glow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CImpact_Glow::CImpact_Glow(const CImpact_Glow& rhs)
	: CEffect(rhs)
{

}

HRESULT CImpact_Glow::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Eff_Impact_Glow"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CImpact_Glow::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CImpact_Glow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CImpact_Glow::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		IMPACT_DESC* pDesc = static_cast<IMPACT_DESC*>(pArg);

		m_EffectDesc.eEffectID = pDesc->eEffectID;
		m_EffectDesc.vSpawnPos = pDesc->vSpawnPos;
		m_EffectDesc.vScale	= pDesc->vScale;
		m_EffectDesc.pParent = pDesc->pParent;
		m_eSequenceDir = pDesc->eSequenceDir;
		m_iFanAngle = pDesc->iFanSize;
		m_eElement = pDesc->eELEMENT;
		m_ePlaneDir = pDesc->ePlaneDir;
		m_iRotationY = pDesc->iRotAxisY;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vSpawnPos);
		m_pTransformCom->Set_Scale(m_EffectDesc.vScale);
		//m_pTransformCom->Set_Scale(XMVectorSet(1.f, 1.5f, 1.f, 1.f)); 기준

		_int iMedian = {};
		_int iQuater = {};
	switch (m_EffectDesc.eEffectID)
	{
	case Client::EFFECT_IMPACT_SINGLE:
		m_iNumGlow = 1;
		if(pDesc->iRotation == 0)
		m_vecRotation.push_back(m_pGameInstance->Get_Int_Random(0, 360));
		else
		m_vecRotation.push_back(pDesc->iRotation);

		break;
	case Client::EFFECT_IMPACT_RADIAL:
		if (pDesc->iNumSpike == 0)
		{
			m_iNumGlow = m_pGameInstance->Get_Int_Random(6,8);
		}
		else
		{
			m_iNumGlow = pDesc->iNumSpike;
		}
		for (_int i = 0; i < m_iNumGlow; i++)
		{
			m_vecRotation.push_back(m_pGameInstance->Get_Int_Random(0, 360));
		}
		break;
	case Client::EFFECT_IMPACT_PLANE:
		if (pDesc->fRadius == 0.f)
		{
			CTransform* pParentsTransform = (CTransform*)(m_EffectDesc.pParent->Find_Component(TEXT("Com_Transform")));
			m_fRadius = pParentsTransform->Get_Scale().m128_f32[0];
		}
		else
			m_fRadius = pDesc->fRadius;

		if (pDesc->iNumSpike == 0)
		{
			m_iNumGlow = static_cast<_int>(m_pGameInstance->Get_Int_Random(5, 6) * m_fRadius);
			if (m_iNumGlow < 2)
				m_iNumGlow = 2;
		}
		else
			m_iNumGlow = pDesc->iNumSpike;
		
		m_fSpacing = m_fRadius / static_cast<_float>(m_iNumGlow);

		iMedian = m_iNumGlow / 2;

		for (_int i = 0; i < m_iNumGlow; i++)
		{
			if (pDesc->iRotation == 0)
			{
				if(m_iNumGlow%2 == 0&&i>iMedian)
					m_vecRotation.push_back((m_iFanAngle *2 / m_iNumGlow * (i+1)) - m_iFanAngle);
				else
					m_vecRotation.push_back((m_iFanAngle*2 / m_iNumGlow * i)- m_iFanAngle);
			}
			else
			m_vecRotation.push_back(pDesc->iRotation);

		}
		break;

	case Client::EFFECT_IMPACT_PLANE_SEQUENCE:
		if (pDesc->fRadius == 0.f)
		{
			CTransform* pParentsTransform = (CTransform*)(m_EffectDesc.pParent->Find_Component(TEXT("Com_Transform")));
			m_fRadius = pParentsTransform->Get_Scale().m128_f32[0];
		}
		else
			m_fRadius = pDesc->fRadius;

		if (pDesc->iNumSpike == 0)
		{
			m_iNumGlow = static_cast<_int>(m_pGameInstance->Get_Int_Random(5, 6) * m_fRadius);
			if (m_iNumGlow <2)
				m_iNumGlow = 2;
		}
		else
		{
			m_iNumGlow = pDesc->iNumSpike;
		}
		m_fSpacing = m_fRadius / static_cast<_float>(m_iNumGlow);


		iMedian = m_iNumGlow / 2;

		for (_int i = 0; i < m_iNumGlow; i++)
		{
			if (pDesc->iRotation == 0)
			{
				if (m_iNumGlow % 2 == 0 && i > iMedian)
					m_vecRotation.push_back((m_iFanAngle*2 / m_iNumGlow * (i + 1)) - m_iFanAngle);
				else
					m_vecRotation.push_back((m_iFanAngle*2 / m_iNumGlow * i) - m_iFanAngle);
			}
			else
				m_vecRotation.push_back(pDesc->iRotation);
		}
		break;
	case Client::EFFECT_IMPACT_PLANE_CIRCULAR:
		if (pDesc->fRadius == 0.f)
		{
			CTransform* pParentsTransform = (CTransform*)(m_EffectDesc.pParent->Find_Component(TEXT("Com_Transform")));
			m_fRadius = pParentsTransform->Get_Scale().m128_f32[0];
		}
		else
			m_fRadius = pDesc->fRadius;

		if (pDesc->iNumSpike == 0)
		{
			m_iNumGlow = static_cast<_int>(m_pGameInstance->Get_Int_Random(8,10) * m_fRadius);
			if (m_iNumGlow < 4)
				m_iNumGlow = 4;
		}
		else
		{
			m_iNumGlow = pDesc->iNumSpike;
			if (m_iNumGlow < 4)
				m_iNumGlow = 4;
		}

		iMedian = m_iNumGlow / 2;
		iQuater = m_iNumGlow / 4;
		if(m_iNumGlow %4 == 0)
			m_fSpacing = (m_fRadius*2.f ) / iMedian;
		else
			m_fSpacing = (m_fRadius * 2.f) / (iMedian+1);

		for (_int i = 0; i < m_iNumGlow; i++)
		{
			if (i < iMedian)
			{
					//정면
				if (i < iQuater)
				{
					//좌측
					m_vecRotation.push_back((m_iFanAngle / iQuater * (i+1)));
				}
				else
				{
					//우측
					if (m_iNumGlow % 4 == 0)
						m_vecRotation.push_back((m_iFanAngle / iQuater * (i-iQuater + 1)));
					else
						m_vecRotation.push_back((m_iFanAngle / (iQuater+1) * (i - iQuater + 1)));
				}
			}
			else
			{
					//후면
				if ((i - iMedian) < iQuater)
				{
					//좌측
					if (m_iNumGlow % 4 == 0)
						m_vecRotation.push_back((m_iFanAngle / iQuater * ((i - iMedian)+1)));
					else
						m_vecRotation.push_back((m_iFanAngle / (iQuater + 1) * ((i - iMedian)+1)));

				}
				else
				{
					//우측
					if (m_iNumGlow % 4 == 0)
						m_vecRotation.push_back((m_iFanAngle / iQuater * (i - iMedian-iQuater + 1)));
					else
						m_vecRotation.push_back((m_iFanAngle / (iQuater + 1) * (i - iMedian - iQuater + 1)));

				}
			}
			//if (i <= iMedian)
			//{
			//	if (i < iQuater)
			//		m_vecRotation.push_back((m_iFanAngle*2 / iQuater * i) - m_iFanAngle);
			//	else
			//		m_vecRotation.push_back((m_iFanAngle * 2 / iQuater * (i - iQuater+1)) - m_iFanAngle);
			//}
			//else
			//{
			//	if((i-iMedian) < iQuater)
			//		m_vecRotation.push_back((m_iFanAngle * 2 / iQuater * ((i - iMedian) + 1)) - m_iFanAngle);
			//	else
			//		m_vecRotation.push_back((m_iFanAngle * 2 / iQuater * ((i - iMedian) - iQuater)) - m_iFanAngle);

			//}
			//cout << m_vecRotation[i] << endl;
		}
		break;
	case Client::EFFECT_END:
		break;
	default:
		break;
	}

	//위치 잡기
		_float fSpace = {};
		_float fHeight = {};
		_float fLength = {};
	if (m_EffectDesc.eEffectID == EFFECT_IMPACT_PLANE_CIRCULAR)
	{
		for (_int i = 0; i < m_iNumGlow; i++)
		{
			_int iHalf = m_iNumGlow / 2;
			_int iQuater = m_iNumGlow / 4;
			_vector vPos = m_EffectDesc.vSpawnPos;
			_vector vScale = m_EffectDesc.vScale;
			vScale.m128_f32[0] *= m_pGameInstance->Get_Float_Random(1.f, 1.5f);
			vScale.m128_f32[1] = vScale.m128_f32[0] * 1.5f;
			m_pTransformCom->Set_Scale(vScale);

			fSpace = m_fSpacing;

			_vector vUp = { 0.f,1.f,0.f,0.f };

			m_pTransformCom->Look_At_Position(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()));
			m_pTransformCom->RotationFromCurrentState(vUp, XMConvertToRadians((_float)m_iRotationY));

			_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			vLook.m128_f32[1] = 0.f;

			if (i < iMedian)
			{
				if (i < iQuater)
				{
					vPos += XMVector3Normalize(vRight) * fSpace * static_cast<_float>(i + 1);
					fLength = fSpace * (i + 1);
					fHeight = static_cast<_float>(sqrt(pow(m_fRadius, 2) - pow(fLength, 2)));
					vPos += XMVector3Normalize(vLook) * fHeight;
				}
				else
				{
					vPos -= XMVector3Normalize(vRight) * fSpace * static_cast<_float>(i - iQuater + 1);
					fLength = fSpace * (i - iQuater + 1);
					fHeight = static_cast<_float>(sqrt(pow(m_fRadius, 2) - pow(fLength, 2)));
					vPos += XMVector3Normalize(vLook) * fHeight;
				}
				//vPos += XMVector3Normalize(vRight) * fSpace * (iQuater - i);
				//fLength = fSpace * (iQuater - i);
				//fHeight = sqrt(pow(m_fRadius, 2) - pow(fLength, 2));
				//vPos += XMVector3Normalize(vLook) * fHeight;
			}
			else
			{

				if ((i-iMedian) < iQuater)
				{
					vPos += XMVector3Normalize(vRight) * fSpace * static_cast<_float>((i - iMedian) + 1);
					fLength = fSpace * ((i - iMedian));
					fHeight = static_cast<_float>(sqrt(pow(m_fRadius, 2) - pow(fLength, 2)));
					vPos -= XMVector3Normalize(vLook) * fHeight;
				}
				else
				{
					vPos -= XMVector3Normalize(vRight) * fSpace * static_cast<_float>((i - iMedian) - iQuater + 1);
					fLength = fSpace * ((i - iMedian) - iQuater);
					fHeight = static_cast<_float>(sqrt(pow(m_fRadius, 2) - pow(fLength, 2)));
					vPos -= XMVector3Normalize(vLook) * fHeight;
				}
				//vPos += XMVector3Normalize(vRight) * fSpace * (iQuater+1 - (i-(iMedian-1)));
				//fLength = fSpace * (iQuater+1 - (i-(iMedian-1)));
				//fHeight = sqrt(pow(m_fRadius, 2) - pow(fLength, 2));
				//vPos += XMVector3Normalize(vLook)* -fHeight;// m_fRadius* sin(m_vecRotation[i]);// *(iMedian - (i - iMedian));
			}
			//cout << vPos.m128_f32[0] << "," << vPos.m128_f32[1] << "," << vPos.m128_f32[2] << endl;

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

			if (m_iRotationY == 0)
			{
			m_pTransformCom->Look_At_Position(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()));
			m_pTransformCom->RotationFromCurrentState(vRight, XMConvertToRadians((_float)m_vecRotation[i]));
			}
			//m_pTransformCom->Go_Up(0.05f);

			m_vecGlowMat.push_back(m_pTransformCom->Get_WorldFloat4x4());

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vSpawnPos);
		}
	}
	else
	{
		for (_int i = 0; i<m_iNumGlow;i++)
		{
			_int iMedian = m_iNumGlow / 2;
			_vector vPos = m_EffectDesc.vSpawnPos;
			_vector vScale = m_EffectDesc.vScale;
			if (m_iNumGlow > 4 && m_EffectDesc.eEffectID == EFFECT_IMPACT_PLANE)
			{
				if (i == iMedian|| i ==(iMedian+1))
				{
					vScale.m128_f32[0] *= m_pGameInstance->Get_Float_Random(1.5f, 1.7f);
					vScale.m128_f32[1] = vScale.m128_f32[0] * 1.6f;
					m_pTransformCom->Set_Scale(vScale);
				}
				vScale.m128_f32[0] *= m_pGameInstance->Get_Float_Random(0.8f, 1.5f);
				vScale.m128_f32[1] = vScale.m128_f32[0] * 1.6f;
				m_pTransformCom->Set_Scale(vScale);
			}
			else
			{
			vScale.m128_f32[0] *= m_pGameInstance->Get_Float_Random(0.8f, 1.5f);
			vScale.m128_f32[1] = vScale.m128_f32[0] * 1.6f;
			m_pTransformCom->Set_Scale(vScale);
			}


			fSpace = m_fSpacing;



			_vector vUp = { 0.f,1.f,0.f,0.f };
			_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			switch (m_ePlaneDir)
			{
			case Client::CImpact_Glow::EFF_PLANE_RIGHT:
				m_pTransformCom->Look_At_Position(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()));
				m_pTransformCom->RotationFromCurrentState(vUp, XMConvertToRadians((_float)m_iRotationY));
				vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
				m_pTransformCom->RotationFromCurrentState(vLook, XMConvertToRadians(90));
				break;
			case Client::CImpact_Glow::EFF_PLANE_LEFT:
				m_pTransformCom->Look_At_Position(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()));
				m_pTransformCom->RotationFromCurrentState(vUp, XMConvertToRadians((_float)m_iRotationY));
				vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
				m_pTransformCom->RotationFromCurrentState(vLook, XMConvertToRadians(270));
				break;
			case Client::CImpact_Glow::EFF_PLANE_REVERSED:
				m_pTransformCom->Look_At_Position(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()));
				m_pTransformCom->RotationFromCurrentState(vUp, XMConvertToRadians((_float)m_iRotationY));
				vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
				m_pTransformCom->RotationFromCurrentState(vLook, XMConvertToRadians(180));
				break;
			case Client::CImpact_Glow::EFF_PLANE_END:
				m_pTransformCom->Look_At_Position(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()));
				m_pTransformCom->RotationFromCurrentState(vUp, XMConvertToRadians((_float)m_iRotationY));
				vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
				break;
			default:
				break;
			}

			_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vPos += XMVector3Normalize(vRight) * fSpace * static_cast<_float>(iMedian - i);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION,vPos);

			//m_pTransformCom->Look_At_Position(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()));
			//_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			m_pTransformCom->RotationFromCurrentState(vLook, XMConvertToRadians((_float)m_vecRotation[i]));
			m_pTransformCom->Go_Up(0.05f);
		
			m_vecGlowMat.push_back(m_pTransformCom->Get_WorldFloat4x4());

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectDesc.vSpawnPos);
		}
	}

	}
	else
		return E_FAIL;

	m_EffectDesc.fLifeTime = 2.f;
	for (_int i = 0; i < m_iNumGlow; i++)
	{

		m_vecTimeAcc.push_back(0.f);
		_int iRandom = m_pGameInstance->Get_Int_Random(0, 1);
		_float2 vUVPos = { 0.5f*iRandom,0.f };
		m_vecUVPos.push_back(vUVPos);
	}
	return S_OK;
}

HRESULT CImpact_Glow::PreviousTick(_float fTimeDelta)
{

	return S_OK;
}

HRESULT CImpact_Glow::Tick(_float fTimeDelta)
{
	m_EffectDesc.fLifeTime -= fTimeDelta;
	if (m_EffectDesc.fLifeTime <= 0.f)
	{
		Set_Destroy(TRUE);
	}


		if (m_EffectDesc.eEffectID == EFFECT_IMPACT_PLANE_SEQUENCE)
		{
				if (m_eSequenceDir == DIR_FROM_LEFT)
				{

					for (_int i = 0; i < (m_iSequeceIdx + 1); i++)
					{
						m_vecTimeAcc[i] += fTimeDelta * 2.f;
						_matrix	TransformMatrix = XMLoadFloat4x4(&m_vecGlowMat[i]);
						_vector vUp = TransformMatrix.r[CTransform::STATE_UP];
						_vector vRight = TransformMatrix.r[CTransform::STATE_RIGHT];
						_vector vPos = TransformMatrix.r[CTransform::STATE_POSITION];
						if (m_vecTimeAcc[i] < 0.1f)
						{
						vPos += XMVector3Normalize(vUp) * m_pTransformCom->Get_SpeedPerSec() * fTimeDelta;
						TransformMatrix.r[CTransform::STATE_POSITION] = vPos;
						}
						_float fRight = XMVectorGetX(XMVector3Length(vRight));
						_float fUp = XMVectorGetX(XMVector3Length(vUp));

						if (fRight >= 0.3f)
						{
							fRight -= fTimeDelta * 8.f;
							fUp -= fTimeDelta * 8.f;

							TransformMatrix.r[CTransform::STATE_RIGHT] = XMVector3Normalize(vRight) * fRight;
							TransformMatrix.r[CTransform::STATE_UP] = XMVector3Normalize(vUp) * fUp;

						}

						XMStoreFloat4x4(&m_vecGlowMat[i], TransformMatrix);
					}
				}
				else if (m_eSequenceDir == DIR_FROM_RIGHT)
				{
					for (_int i = 0; i < (m_iSequeceIdx + 1); i++)
					{
						m_vecTimeAcc[i] += fTimeDelta * 2.f;
						_matrix	TransformMatrix = XMLoadFloat4x4(&m_vecGlowMat[(m_iNumGlow - 1) - i]);
						_vector vUp = TransformMatrix.r[CTransform::STATE_UP];
						_vector vRight = TransformMatrix.r[CTransform::STATE_RIGHT];
						_vector vPos = TransformMatrix.r[CTransform::STATE_POSITION];
						if (m_vecTimeAcc[i] < 0.1f)
						{
							vPos += XMVector3Normalize(vUp) * m_pTransformCom->Get_SpeedPerSec() * fTimeDelta;
							TransformMatrix.r[CTransform::STATE_POSITION] = vPos;
						}

						_float fRight = XMVectorGetX(XMVector3Length(vRight));
						_float fUp = XMVectorGetX(XMVector3Length(vUp));

						if (fRight >= 0.3f)
						{
							fRight -= fTimeDelta * 8.f;
							fUp -= fTimeDelta * 8.f;

							TransformMatrix.r[CTransform::STATE_RIGHT] = XMVector3Normalize(vRight) * fRight;
							TransformMatrix.r[CTransform::STATE_UP] = XMVector3Normalize(vUp) * fUp;

						}

						XMStoreFloat4x4(&m_vecGlowMat[(m_iNumGlow - 1) - i], TransformMatrix);
					}
				}

			
		}
		else if (m_EffectDesc.eEffectID == EFFECT_IMPACT_PLANE_CIRCULAR)
		{
			m_vecTimeAcc[0] += fTimeDelta * 2.f;

			if (m_vecTimeAcc[0] < .5f)
			{

				for (_int i = 0; i < m_iNumGlow; i++)
				{
					_matrix	TransformMatrix = XMLoadFloat4x4(&m_vecGlowMat[i]);

					////쳐다보기 LookAt_Direction
					_vector vRight = TransformMatrix.r[CTransform::STATE_RIGHT];
					_vector vUp = TransformMatrix.r[CTransform::STATE_UP];
					_vector vLook = TransformMatrix.r[CTransform::STATE_LOOK];
					_vector vPos = TransformMatrix.r[CTransform::STATE_POSITION];

					////Get Scale
					_float fRight = XMVectorGetX(XMVector3Length(vRight));
					_float fUp = XMVectorGetX(XMVector3Length(vUp));
					_float fLook = XMVectorGetX(XMVector3Length(vLook));

					//_vector		vNewLook = XMLoadFloat4(&m_pGameInstance->Get_CamPosition()) - vPos;
					//_vector		vNewRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
					//_vector		vNewUp = XMVector3Cross(vLook, vRight);

					//TransformMatrix.r[CTransform::STATE_RIGHT] = XMVector3Normalize(vNewRight) * fRight;
					//TransformMatrix.r[CTransform::STATE_UP] = XMVector3Normalize(vNewUp) * fUp;
					//TransformMatrix.r[CTransform::STATE_LOOK] = XMVector3Normalize(vNewLook) * fLook;

					//XMStoreFloat4x4(&m_vecGlowMat[i], TransformMatrix);

					//쳐다보는중 내적 통해 방향 설정
					//_vector vFromCentre = m_EffectDesc.vSpawnPos - vPos;
					//vFromCentre.m128_f32[1] = 0.f;
					//_vector vFlatLook = TransformMatrix.r[CTransform::STATE_LOOK];
					//vFlatLook.m128_f32[1] = 0.f;
					//_float fDotProduct = XMVectorGetX(XMVector4Dot(XMVector4Normalize(vFlatLook), XMVector4Normalize(vFromCentre)));
		
					//if (fDotProduct > 0.f)
					//{
					//	vRight = XMVector4Normalize(TransformMatrix.r[CTransform::STATE_RIGHT]);
					//	vUp = XMVector4Normalize(TransformMatrix.r[CTransform::STATE_UP]);
					//	vLook = XMVector4Normalize(TransformMatrix.r[CTransform::STATE_LOOK]);

					//	// 회전 행렬을 생성한다.
					//	_matrix		RotationMatrix = XMMatrixRotationAxis(vRight, XMConvertToRadians(330));

					//	// 회전 행렬을 적용한다.
					//	TransformMatrix.r[CTransform::STATE_RIGHT]= XMVector4Transform(vRight, RotationMatrix) * fRight;
					//	TransformMatrix.r[CTransform::STATE_UP]   = XMVector4Transform(vUp, RotationMatrix) * fUp;
					//	TransformMatrix.r[CTransform::STATE_LOOK] = XMVector4Transform(vLook, RotationMatrix) * fLook;
					//}
					//else
					//{
						//vRight = XMVector4Normalize(TransformMatrix.r[CTransform::STATE_RIGHT]);
						//vUp = XMVector4Normalize(TransformMatrix.r[CTransform::STATE_UP]);
						//vLook = XMVector4Normalize(TransformMatrix.r[CTransform::STATE_LOOK]);

						//// 회전 행렬을 생성한다.
						//_matrix		RotationMatrix = XMMatrixRotationAxis(vLook, XMConvertToRadians(m_vecRotation[i]));

						//// 회전 행렬을 적용한다.
						//TransformMatrix.r[CTransform::STATE_RIGHT] = XMVector4Transform(vRight, RotationMatrix) * fRight;
						//TransformMatrix.r[CTransform::STATE_UP] = XMVector4Transform(vUp, RotationMatrix) * fUp;
						//TransformMatrix.r[CTransform::STATE_LOOK] = XMVector4Transform(vLook, RotationMatrix) * fLook;
					//}

					vPos += XMVector3Normalize(TransformMatrix.r[CTransform::STATE_UP]) * m_pTransformCom->Get_SpeedPerSec() * fTimeDelta;
					TransformMatrix.r[CTransform::STATE_POSITION] = vPos;

					fRight = XMVectorGetX(XMVector3Length(TransformMatrix.r[CTransform::STATE_RIGHT]));
					fUp = XMVectorGetX(XMVector3Length(TransformMatrix.r[CTransform::STATE_UP]));
					if (fRight >= 0.3f)
					{
						fRight -= fTimeDelta * 8.f;
						fUp -= fTimeDelta * 8.f;

						TransformMatrix.r[CTransform::STATE_RIGHT] = XMVector3Normalize(TransformMatrix.r[CTransform::STATE_RIGHT]) * fRight;
						TransformMatrix.r[CTransform::STATE_UP] = XMVector3Normalize(TransformMatrix.r[CTransform::STATE_UP]) * fUp;

					}

					XMStoreFloat4x4(&m_vecGlowMat[i], TransformMatrix);

				}
			}
		}
		else
		{

			m_vecTimeAcc[0] += fTimeDelta * 2.f;

			if (m_vecTimeAcc[0] < 0.3f)
			{

				for (_int i = 0; i < m_iNumGlow; i++)
				{
					_matrix	TransformMatrix = XMLoadFloat4x4(&m_vecGlowMat[i]);
					_vector vUp = TransformMatrix.r[CTransform::STATE_UP];
					_vector vRight = TransformMatrix.r[CTransform::STATE_RIGHT];
					_vector vPos = TransformMatrix.r[CTransform::STATE_POSITION];
					vPos += XMVector3Normalize(vUp) * m_pTransformCom->Get_SpeedPerSec() * fTimeDelta;
					TransformMatrix.r[CTransform::STATE_POSITION] = vPos;

					_float fRight = XMVectorGetX(XMVector3Length(vRight));
					_float fUp = XMVectorGetX(XMVector3Length(vUp));

					if (fRight >= 0.3f)
					{
						fRight -= fTimeDelta * 8.f;
						fUp -= fTimeDelta * 8.f;

						TransformMatrix.r[CTransform::STATE_RIGHT] = XMVector3Normalize(vRight) * fRight;
						TransformMatrix.r[CTransform::STATE_UP] = XMVector3Normalize(vUp) * fUp;

					}

					XMStoreFloat4x4(&m_vecGlowMat[i], TransformMatrix);

				}
			}
		}



	
	
	if (m_EffectDesc.eEffectID == EFFECT_IMPACT_PLANE_SEQUENCE && m_vecTimeAcc[m_iSequeceIdx] >= 0.1f)
	{
		if (m_iSequeceIdx < m_iNumGlow-1)
		{

			m_iSequeceIdx++;
		}
	}

	//	m_pTransformCom->Go_Up(fTimeDelta);

	//_vector vSelfScale = m_pTransformCom->Get_Scale();

	//if (vSelfScale.m128_f32[0] >= 0.3f)		// 일정 수치까지 크기를 줄이기
	//{
	//	vSelfScale.m128_f32[0] -= fTimeDelta* 8.f;
	//	vSelfScale.m128_f32[1] -= fTimeDelta* 8.f;
	//	m_pTransformCom->Set_Scale(vSelfScale);
	//}


	//m_fTimeAcc += fTimeDelta;

	//크기 커지게
	//if (m_fTimeAcc <= 1.f)
	//{
	//	_float fSize = sin(XMConvertToRadians(m_fTimeAcc * 90.f)) * m_EffectDesc.vScale.m128_f32[0];
	//	m_pTransformCom->Set_Scale(XMVectorSet(fSize, fSize, fSize, 1.f));
	//}
	ELEMENT eElement;
	if (m_eElement == ELEMENT_END)
	{
		if (nullptr == m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")))
			return S_OK;

		eElement = static_cast<CPlayer*>(m_pGameInstance->Find_Object(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_Player")))->Get_Element();
	}
	else
		eElement = m_eElement;

	switch (eElement)
	{
	case Client::ELEMENT_FIRE:
		m_vElementColor = { 255.f / 255.f, 210.f / 255.f, 165.f / 255.f, 1.f };
		break;
	case Client::ELEMENT_WATER:
		m_vElementColor = { 0.75f, 1.f, 1.f, 1.f };
		break;
	case Client::ELEMENT_WIND:
		m_vElementColor = { 116.f / 255.f,230.f / 255.f,236.f / 255.f ,1.f };
		break;
	case Client::ELEMENT_ELECTRIC:
		m_vElementColor = { 231.f / 255.f,207.f / 254.f,1.f ,1.f };
		break;
	case Client::ELEMENT_FROST:
		m_vElementColor = { 0.75f, 1.f, 1.f, 1.f };
		break;
	case Client::ELEMENT_EARTH:
		m_vElementColor = { 1.f, 1.0f, .75f, 1.f };
		break;
	case Client::ELEMENT_END:
		m_vElementColor = { 1.f, 1.0f, .75f, 1.f };
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CImpact_Glow::LateTick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_WorldMatrix());

	m_pRendererCom->Add_RenderGroup(CRenderer::RG_BLEND, this);

	return S_OK;
}

HRESULT CImpact_Glow::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vElementColor, sizeof(_vector))))
		return E_FAIL;

	if (m_eSequenceDir == DIR_FROM_LEFT)
	{
		for (_int i = 0; i < (m_iSequeceIdx + 1); i++)
		{
			if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_vecTimeAcc[i], sizeof(_float))))
				return E_FAIL;

			_float4x4 GlowMat = m_vecGlowMat[i];

			if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", GlowMat)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVPos", &m_vecUVPos[i], sizeof(_float2))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(VTXTEX_EFF_IMPACT_GLOW)))
				return E_FAIL;

			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}
	}
	else if(m_eSequenceDir == DIR_FROM_RIGHT)
	{
		for (_int i = 0; i < m_iSequeceIdx+1; i++)
		{
			if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_vecTimeAcc[i], sizeof(_float))))
				return E_FAIL;
			_float4x4 GlowMat = m_vecGlowMat[(m_iNumGlow - 1) - i];

			if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", GlowMat)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVPos", &m_vecUVPos[i], sizeof(_float2))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(VTXTEX_EFF_IMPACT_GLOW)))
				return E_FAIL;

			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}
	}
	else
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeAcc", &m_vecTimeAcc[0], sizeof(_float))))
			return E_FAIL;

		for (_int i = 0; i < m_iNumGlow; i++)
		{
			_float4x4 GlowMat = m_vecGlowMat[i];
	
			if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", GlowMat)))
				return E_FAIL;
	
			if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVPos", &m_vecUVPos[i], sizeof(_float2))))
				return E_FAIL;
	
			if (FAILED(m_pShaderCom->Begin(VTXTEX_EFF_IMPACT_GLOW)))
				return E_FAIL;
	
			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}
	}

	return S_OK;
}

CImpact_Glow* CImpact_Glow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CImpact_Glow* pInstance = new CImpact_Glow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Create : CImpact_Glow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CImpact_Glow::Clone(void* pArg)
{
	CImpact_Glow* pInstance = new CImpact_Glow(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CImpact_Glow"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CImpact_Glow::Free()
{
	__super::Free();

	if (m_bCloned == true)
	{

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);

	}
}