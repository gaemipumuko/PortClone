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
/// ���� ��ǥ��, �������� �־��ָ�, ���� ��ǥ�� �������� ���� ���� �ȿ��� ������ ��ǥ�� ��ȯ
/// </summary>
/// <param name="vCenterPos"></param>
/// <param name="fRadius"></param>
/// <returns></returns>
_vector CRandom_Manager::Get_RandomPos_InCircle(_vector vCenterPos, _float fRadius, _vector vSelfPos)
{
	_float fRandomAngle = Get_Float_Random(0.f, XM_2PI);
	_float fRandomRadius = Get_Float_Random(0.f, fRadius);

	// ������ ��ġ�� ���
	_vector vRandomPosInCircle = XMVectorSet
	(
		fRandomRadius * cos(fRandomAngle),	// X ��
		0.f,								// Y ��
		fRandomRadius * sin(fRandomAngle),  // Z ��
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
