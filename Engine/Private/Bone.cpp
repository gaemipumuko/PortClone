#include "Bone.h"

CBone::CBone()
{
}

void CBone::Invalidate_CombinedTransformationMatrix(const vector<CBone*>& Bones)
{
	if (m_eBoneType == BONE_PART_ROOT && m_iParentBoneIndex == -1)
	{
		XMStoreFloat4x4(&m_CombindTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) *
			XMLoadFloat4x4(&m_pParentBone->m_CombindTransformationMatrix));
	}
	else if (m_iParentBoneIndex == -1)
	{
		m_CombindTransformationMatrix = m_TransformationMatrix;
	}
	else
	{
		XMStoreFloat4x4(&m_CombindTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) *
			XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombindTransformationMatrix));
	}
}

void CBone::Invalidate_CombinedTransformationMatrix(const vector<CBone*>& Bones, _float fRate, _float fMargin)
{
	if (m_eBoneType == BONE_PART_ROOT && m_iParentBoneIndex == -1)
	{
		XMStoreFloat4x4(&m_CombindTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) *
			XMLoadFloat4x4(&m_pParentBone->m_CombindTransformationMatrix));

		m_CombindTransformationMatrix._41 *= fRate;
		m_CombindTransformationMatrix._42 *= fRate;
		m_CombindTransformationMatrix._43 *= fRate;

		m_CombindTransformationMatrix._42 -= fMargin;
	}
	else if (m_iParentBoneIndex == -1)
	{
		m_CombindTransformationMatrix = m_TransformationMatrix;
	}
	else
	{
		XMStoreFloat4x4(&m_CombindTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) *
			XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombindTransformationMatrix));
	}
}

HRESULT CBone::Initialize(const BONE_DESC* pBoneDesc, BONE_TYPE eType)
{
	/* Bone Type */
	m_eBoneType = eType;

	/* Bone Name */
	strcpy_s(m_szName, pBoneDesc->szName);

	/* Parent Bone Idx */
	m_iParentBoneIndex = pBoneDesc->iParentBoneIndex;

	/* Transform Matrix */
	m_TransformationMatrix = pBoneDesc->TransformationMatrix;
	XMStoreFloat4x4(&m_CombindTransformationMatrix, XMMatrixIdentity());

	return S_OK;
}

CBone* CBone::Create(const BONE_DESC* pBoneDesc)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pBoneDesc)))
	{
		MSG_BOX(TEXT("Failed to Create : CBone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Clone()
{
	return new CBone(*this);
}

void CBone::Free()
{
	__super::Free();

	Safe_Release(m_pParentBone);
}
