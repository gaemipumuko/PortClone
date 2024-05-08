#include "Random_Manager.h"

CRandom_Manager::CRandom_Manager()
{
}

_int CRandom_Manager::Get_Int_Random(_int iStart, _int iEnd)
{
	uniform_int_distribution<_int> iDistribution(iStart, iEnd);

	return iDistribution(m_Generator);
}

_float CRandom_Manager::Get_Float_Random(_float fStart, _float fEnd)
{
	uniform_real_distribution<_float> fDistribution(fStart, fEnd);

	return fDistribution(m_Generator);
}

/// <summary>
/// 기준 좌표와, 반지름을 넣어주면, 기준 좌표를 기준으로 원의 범위 안에서 랜덤한 좌표를 반환
/// </summary>
/// <param name="vCenterPos"></param>
/// <param name="fRadius"></param>
/// <returns></returns>
_vector CRandom_Manager::Get_RandomPos_InCircle(_vector vCenterPos, _float fRadius, _vector vSelfPos)
{
	_float fRandomAngle = Get_Float_Random(0.f, XM_2PI);
	_float fRandomRadius = Get_Float_Random(0.f, fRadius);

	// 랜덤한 위치를 계산
	_vector vRandomPosInCircle = XMVectorSet
	(
		fRandomRadius * cos(fRandomAngle),	// X 축
		0.f,								// Y 축
		fRandomRadius * sin(fRandomAngle),  // Z 축
		0.f							        // W
	);

	vRandomPosInCircle = XMVectorAdd(vCenterPos, vRandomPosInCircle);

	if (XMVectorGetX(XMVector3Length(vSelfPos - vRandomPosInCircle)) < 5.f)
		return Get_RandomPos_InCircle(vCenterPos, fRadius);

	return vRandomPosInCircle;
}

HRESULT CRandom_Manager::Initialize()
{
	m_Generator.seed(static_cast<_uint>(chrono::system_clock::now().time_since_epoch().count()));

	return S_OK;
}

CRandom_Manager* CRandom_Manager::Create()
{
	CRandom_Manager* pInstance = new CRandom_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CRandom_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRandom_Manager::Free()
{
	__super::Free();
}
