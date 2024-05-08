#include "Edit_Channel.h"
#include "Edit_Bone.h"

CEdit_Channel::CEdit_Channel()
{
}

void CEdit_Channel::Invalidate_TransformationMatrix(const vector<class CEdit_Bone*>& Bones, _uint* pCurrentKeyFrameIndex, _float fCurrentAnimPosition)
{
	if (0.0f == fCurrentAnimPosition)
		*pCurrentKeyFrameIndex = 0;

	KEYFRAME		LastKeyFrame = m_KeyFrames.back();

	_vector			vScale;
	_vector			vRotation;
	_vector			vTranslation;

	if (fCurrentAnimPosition >= LastKeyFrame.fTime)
	{
		vScale = XMLoadFloat4(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vTranslation = XMLoadFloat4(&LastKeyFrame.vTranslation);
	}
	else
	{
		while (fCurrentAnimPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTime)
			++*pCurrentKeyFrameIndex;

		_float		fRatio = (fCurrentAnimPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTime) /
			(m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTime - m_KeyFrames[*pCurrentKeyFrameIndex].fTime);

		_vector		vSourScale, vDestScale;
		_vector		vSourRotation, vDestRotation;
		_vector		vSourTranslation, vDestTranslation;

		vSourScale = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vScale);
		vDestScale = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vScale);
		vScale = XMVectorLerp(vSourScale, vDestScale, fRatio);

		vSourRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vRotation);
		vDestRotation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, fRatio);

		vSourTranslation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex].vTranslation);
		vDestTranslation = XMLoadFloat4(&m_KeyFrames[*pCurrentKeyFrameIndex + 1].vTranslation);
		vTranslation = XMVectorLerp(vSourTranslation, vDestTranslation, fRatio);
	}
	_matrix			TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);

	XMStoreFloat4(&m_ePrevFrame.vScale, vScale);
	XMStoreFloat4(&m_ePrevFrame.vRotation, vRotation);
	XMStoreFloat4(&m_ePrevFrame.vTranslation, vTranslation);
}

HRESULT CEdit_Channel::Initialize(const CH_DESC* pChDesc, const vector<class CEdit_Bone*>& Bones)
{
	/* Channel Name */
	strcpy_s(m_szName, pChDesc->szName);

	m_iBoneIndex = pChDesc->iBoneIndex;
	m_iNumKeyFrames = pChDesc->iNumKeyFrames;

	for (size_t siKeyFrame = 0; siKeyFrame < m_iNumKeyFrames; ++siKeyFrame)
	{
		m_KeyFrames.push_back(pChDesc->pKeyFrames[siKeyFrame]);
	}

	return S_OK;
}

CEdit_Channel* CEdit_Channel::Create(const CH_DESC* pChDesc, const vector<class CEdit_Bone*>& Bones)
{
	CEdit_Channel* pInstance = new CEdit_Channel();

	if (FAILED(pInstance->Initialize(pChDesc, Bones)))
	{
		MSG_BOX(TEXT("Failed to Create : CEdit_Channel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEdit_Channel::Free()
{
	__super::Free();

	m_KeyFrames.clear();
}
