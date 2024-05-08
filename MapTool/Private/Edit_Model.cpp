#include "Edit_Model.h"
#include "Edit_Bone.h"
#include "Edit_Mesh.h"
#include "Edit_Animation.h"
#include "Edit_Channel.h"
#include "Transform.h"
#include "Texture.h"
#include "GameInstance.h"

CEdit_Model::CEdit_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CEdit_Model::CEdit_Model(const CEdit_Model& rhs)
	: CComponent(rhs)
	, m_eModelType(rhs.m_eModelType)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	, m_iNumAnimations(rhs.m_iNumAnimations)
{
	for (auto& pPrototypeAnimation : rhs.m_Animations)
		m_Animations.push_back(pPrototypeAnimation->Clone());

	for (auto& pPrototypeBone : rhs.m_Bones)
		m_Bones.push_back(pPrototypeBone->Clone());

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	for (auto& Material : m_Materials)
		for (size_t i = 0; i < ::AI_TEXTURE_TYPE_MAX; i++)
			Safe_AddRef(Material.pMaterials[i]);
}

_char* CEdit_Model::Get_MeshName(_uint iIndex)
{
	return m_Meshes[iIndex]->Get_Name();
}

CEdit_Bone* CEdit_Model::Get_Bone(string strBoneName)
{
	for (auto& Bone : m_Bones)
	{
		if (!strcmp(Bone->Get_BoneName(), strBoneName.c_str()))
			return Bone;
	}

	return nullptr;
}

const _float4x4* CEdit_Model::Get_CombinedMatrix(string strBoneName)
{
	for (auto& Bone : m_Bones)
	{
		if (!strcmp(Bone->Get_BoneName(), strBoneName.c_str()))
			return Bone->Get_CombinedMatrix();
	}

	return nullptr;
}

void CEdit_Model::Set_PartRootBone(string strRootNode, CEdit_Bone* pBone)
{
	for (auto& Bone : m_Bones)
	{
		if (!strcmp(Bone->Get_BoneName(), strRootNode.c_str()))
		{
			Bone->Set_BoneType(CEdit_Bone::BONE_PART_ROOT);
			Bone->Set_ParentBone(pBone);
			return;
		}
	}
}

vector<string> CEdit_Model::Get_BoneTags()
{
	vector<string> BoneTags;

	for (auto& iter : m_Bones)
		BoneTags.push_back(iter->Get_BoneName());

	return BoneTags;
}

vector<string> CEdit_Model::Get_AnimationTags()
{
	vector<string> AnimationTags;

	for (auto& iter : m_Animations)
		AnimationTags.push_back(iter->Get_AnimationTag());

	return AnimationTags;
}

void CEdit_Model::Set_Animation(_uint iAnimIndex)
{
	if (m_iCurrentAnimIndex != iAnimIndex)
	{
		m_fConvertRatio = 0.f;
		m_iPrevAnimIndex = m_iCurrentAnimIndex;
		m_iCurrentAnimIndex = iAnimIndex;

		m_Animations[m_iCurrentAnimIndex]->Reset_Anim();
		m_bChangeAnimation = TRUE;
	}
}

void CEdit_Model::Init_Animation(_uint iAnimIndex)
{
	m_iCurrentAnimIndex = iAnimIndex;
}

HRESULT CEdit_Model::Ready_Bones(ifstream& inFile)
{
	_int iNumBones = 0;

	inFile.read(reinterpret_cast<char*>(&iNumBones), sizeof _uint);
	m_Bones.reserve(iNumBones);

	for (size_t idx = 0; idx < iNumBones; ++idx)
	{
		BONE_DESC tBoneDesc = { };

		inFile.read(reinterpret_cast<char*>(&tBoneDesc.szName), sizeof(char[MAX_PATH]));
		inFile.read(reinterpret_cast<char*>(&tBoneDesc.iParentBoneIndex), sizeof _int);

		inFile.read(reinterpret_cast<char*>(&tBoneDesc.TransformationMatrix.m[0]), sizeof _float4);
		inFile.read(reinterpret_cast<char*>(&tBoneDesc.TransformationMatrix.m[1]), sizeof _float4);
		inFile.read(reinterpret_cast<char*>(&tBoneDesc.TransformationMatrix.m[2]), sizeof _float4);
		inFile.read(reinterpret_cast<char*>(&tBoneDesc.TransformationMatrix.m[3]), sizeof _float4);

		CEdit_Bone* pBone = CEdit_Bone::Create(&tBoneDesc);
		if (nullptr == pBone)
			return E_FAIL;

		m_Bones.push_back(pBone);
	}

	return S_OK;
}

HRESULT CEdit_Model::Ready_Meshes(ifstream& inFile)
{
	inFile.read(reinterpret_cast<char*>(&m_iNumMeshes), sizeof _uint);
	m_Meshes.reserve(m_iNumMeshes);

	for (size_t idx = 0; idx < m_iNumMeshes; ++idx)
	{
		MESH_DESC tMeshDesc = { };

		inFile.read(reinterpret_cast<char*>(&tMeshDesc.szName), sizeof(char[MAX_PATH]));
		inFile.read(reinterpret_cast<char*>(&tMeshDesc.iMaterialIndex), sizeof _uint);
		inFile.read(reinterpret_cast<char*>(&tMeshDesc.iNumVertices), sizeof _uint);
		inFile.read(reinterpret_cast<char*>(&tMeshDesc.iNumIndices), sizeof _uint);

		tMeshDesc.vPositions = new XMFLOAT3[tMeshDesc.iNumVertices];
		tMeshDesc.vNormals = new XMFLOAT3[tMeshDesc.iNumVertices];
		tMeshDesc.vTextureCoords = new XMFLOAT3[tMeshDesc.iNumVertices];
		tMeshDesc.vTangents = new XMFLOAT3[tMeshDesc.iNumVertices];

		inFile.read(reinterpret_cast<char*>(tMeshDesc.vPositions), sizeof _float3 * tMeshDesc.iNumVertices);
		inFile.read(reinterpret_cast<char*>(tMeshDesc.vNormals), sizeof _float3 * tMeshDesc.iNumVertices);
		inFile.read(reinterpret_cast<char*>(tMeshDesc.vTextureCoords), sizeof _float3 * tMeshDesc.iNumVertices);
		inFile.read(reinterpret_cast<char*>(tMeshDesc.vTangents), sizeof _float3 * tMeshDesc.iNumVertices);

		if (m_eModelType == MTYPE_ANIM)
		{
			inFile.read(reinterpret_cast<char*>(&tMeshDesc.iNumBones), sizeof _uint);

			tMeshDesc.OffsetMatices = new _float4x4[tMeshDesc.iNumBones];
			tMeshDesc.BoneIndices = new _uint[tMeshDesc.iNumBones];

			inFile.read(reinterpret_cast<char*>(tMeshDesc.OffsetMatices), sizeof _float4x4 * tMeshDesc.iNumBones);
			inFile.read(reinterpret_cast<char*>(tMeshDesc.BoneIndices), sizeof _uint * tMeshDesc.iNumBones);

			tMeshDesc.vBlendIndices = new XMUINT4[tMeshDesc.iNumVertices];
			tMeshDesc.vBlendWeights = new _float4[tMeshDesc.iNumVertices];

			inFile.read(reinterpret_cast<char*>(tMeshDesc.vBlendIndices), sizeof XMUINT4 * tMeshDesc.iNumVertices);
			inFile.read(reinterpret_cast<char*>(tMeshDesc.vBlendWeights), sizeof _float4 * tMeshDesc.iNumVertices);
		}

		tMeshDesc.pIndices = new _ulong[tMeshDesc.iNumIndices];

		inFile.read(reinterpret_cast<char*>(tMeshDesc.pIndices), sizeof _ulong * tMeshDesc.iNumIndices);

		CEdit_Mesh* pMesh = CEdit_Mesh::Create(m_pDevice, m_pContext, m_eModelType, &tMeshDesc, m_Bones, XMLoadFloat4x4(&m_PivotMatrix));
		
		if (pMesh == nullptr)
			return E_FAIL;

		Safe_Delete_Array(tMeshDesc.vPositions);
		Safe_Delete_Array(tMeshDesc.vNormals);
		Safe_Delete_Array(tMeshDesc.vTextureCoords);
		Safe_Delete_Array(tMeshDesc.vTangents);

		if (m_eModelType == MTYPE_ANIM)
		{
			Safe_Delete_Array(tMeshDesc.OffsetMatices);
			Safe_Delete_Array(tMeshDesc.BoneIndices);

			Safe_Delete_Array(tMeshDesc.vBlendIndices);
			Safe_Delete_Array(tMeshDesc.vBlendWeights);
		}

		Safe_Delete_Array(tMeshDesc.pIndices);

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CEdit_Model::Ready_Materials(ifstream& inFile, const string& strModelFilePath)
{
	inFile.read(reinterpret_cast<char*>(&m_iNumMaterials), sizeof _uint);
	m_Meshes.reserve(m_iNumMeshes);

	_char		szDrive[MAX_PATH] = "";
	_char		szDirectory[MAX_PATH] = "";
	_char		szFileName[MAX_PATH] = "";
	_char		szExt[MAX_PATH] = "";

	_char		szTextureFilePath[MAX_PATH] = "";

	_splitpath_s(strModelFilePath.c_str(), szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		MODEL_MATERIAL			ModelMaterials = {};

		_uint iNumTexture = 0;
		inFile.read(reinterpret_cast<char*>(&iNumTexture), sizeof _uint);

		for (size_t j = 0; j < iNumTexture; j++)
		{
			_uint	iMaterialIdx = 0;
			char	szTexturePath[MAX_PATH];

			inFile.read(reinterpret_cast<char*>(&iMaterialIdx), sizeof _uint);
			inFile.read(reinterpret_cast<char*>(&szTexturePath), sizeof(char[MAX_PATH]));

			_splitpath_s(szTexturePath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			strcpy_s(szTextureFilePath, szDrive);
			strcat_s(szTextureFilePath, szDirectory);
			strcat_s(szTextureFilePath, szFileName);
			strcat_s(szTextureFilePath, szExt);

			wstring strPrototypeTag = L"Prototype_Component_Texture_";
			strPrototypeTag += ConvertCToWC(szFileName);

			/* Texture Find or Create */
			CTexture* pTexture = static_cast<CTexture*>(
				m_pGameInstance->Find_Component_Prototype(0, strPrototypeTag));

			if (pTexture == nullptr)
			{
				_tchar			szFullPath[MAX_PATH] = L"";
				MultiByteToWideChar(CP_ACP, 0, szTextureFilePath, static_cast<_int>(strlen(szTextureFilePath)), szFullPath, MAX_PATH);

				if (FAILED(m_pGameInstance->Add_Component_Prototype(0, strPrototypeTag,
					pTexture = CTexture::Create(m_pDevice, m_pContext, szFullPath)))) 
					return E_FAIL;
			}

			if (pTexture == nullptr)
				return E_FAIL;

			ModelMaterials.pMaterials[iMaterialIdx] = pTexture;
			Safe_AddRef(pTexture);

			if (nullptr == ModelMaterials.pMaterials[iMaterialIdx])
				return E_FAIL;
		}

		m_Materials.push_back(ModelMaterials);
	}

	return S_OK;
}

HRESULT CEdit_Model::Ready_Animations(ifstream& inFile)
{
	inFile.read(reinterpret_cast<char*>(&m_iNumAnimations), sizeof _uint);
	m_Animations.reserve(m_iNumAnimations);

	for (size_t idx = 0; idx < m_iNumAnimations; ++idx)
	{
		ANIM_DESC tAnimDesc = { };

		inFile.read(reinterpret_cast<char*>(&tAnimDesc.szName), sizeof(char[MAX_PATH]));
		inFile.read(reinterpret_cast<char*>(&tAnimDesc.dDuration), sizeof _double);
		inFile.read(reinterpret_cast<char*>(&tAnimDesc.dTicksPerSecond), sizeof _double);
		inFile.read(reinterpret_cast<char*>(&tAnimDesc.iNumChannels), sizeof _uint);

		tAnimDesc.pChannelDesc = new CH_DESC[tAnimDesc.iNumChannels];
		for (size_t chIdx = 0; chIdx < tAnimDesc.iNumChannels; ++chIdx)
		{
			inFile.read(reinterpret_cast<char*>(&tAnimDesc.pChannelDesc[chIdx].szName), sizeof(char[MAX_PATH]));
			inFile.read(reinterpret_cast<char*>(&tAnimDesc.pChannelDesc[chIdx].iBoneIndex), sizeof _uint);
			inFile.read(reinterpret_cast<char*>(&tAnimDesc.pChannelDesc[chIdx].iNumKeyFrames), sizeof _uint);

			tAnimDesc.pChannelDesc[chIdx].pKeyFrames = new KEYFRAME[tAnimDesc.pChannelDesc[chIdx].iNumKeyFrames];
			for (size_t frameIdx = 0; frameIdx < tAnimDesc.pChannelDesc[chIdx].iNumKeyFrames; ++frameIdx)
			{
				inFile.read(reinterpret_cast<char*>(&tAnimDesc.pChannelDesc[chIdx].pKeyFrames[frameIdx].fTime), sizeof _float);
				inFile.read(reinterpret_cast<char*>(&tAnimDesc.pChannelDesc[chIdx].pKeyFrames[frameIdx].vScale), sizeof XMFLOAT4);
				inFile.read(reinterpret_cast<char*>(&tAnimDesc.pChannelDesc[chIdx].pKeyFrames[frameIdx].vRotation), sizeof XMFLOAT4);
				inFile.read(reinterpret_cast<char*>(&tAnimDesc.pChannelDesc[chIdx].pKeyFrames[frameIdx].vTranslation), sizeof XMFLOAT4);
			}
		}

		CEdit_Animation* pAnimation = CEdit_Animation::Create(&tAnimDesc, m_Bones);

		if (nullptr == pAnimation)
			return E_FAIL;

		for (size_t chIdx = 0; chIdx < tAnimDesc.iNumChannels; ++chIdx)
		{
			Safe_Delete_Array(tAnimDesc.pChannelDesc[chIdx].pKeyFrames);
		}

		Safe_Delete_Array(tAnimDesc.pChannelDesc);

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

HRESULT CEdit_Model::Bind_BoneMatrices(_uint iMeshIndex, CShader* pShader, const _char* pConstantName)
{
	return m_Meshes[iMeshIndex]->Bind_BoneMatrices(pShader, m_Bones, pConstantName, XMLoadFloat4x4(&m_PivotMatrix));
}

HRESULT CEdit_Model::Bind_Material(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, TEXTURETYPE eMaterialType)
{
	_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	if (m_Materials[iMaterialIndex].pMaterials[eMaterialType] == nullptr)
		return E_NOINTERFACE;

	return m_Materials[iMaterialIndex].pMaterials[eMaterialType]->Bind_ShaderResource(pShader, pConstantName, 0);
}

void CEdit_Model::Invalidate_CombinedTransformationMatrix()
{
	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
	}
}

_float CEdit_Model::Play_Animation(_bool isLoop, _float fTimeDelta)
{
	_float fCurrentAnimPosition = 0.f;

	if (m_bChangeAnimation)
	{
		m_fConvertRatio += fTimeDelta * 5.f;

		CEdit_Model::Change_Animation();

		if (m_fConvertRatio > 1.f)
		{
			m_bChangeAnimation = FALSE;
		}
	}
	else
		fCurrentAnimPosition = m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix(isLoop, m_Bones, fTimeDelta);

	/* 모든 뼈들의 CombinedTransformationMatrix를 갱신한다. */
	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
	}

	return fCurrentAnimPosition;
}

_float CEdit_Model::Play_Animation_Half(_bool isLoop, _float fTimeDelta)
{
	_float fCurrentAnimPosition = 0.f;

	if (m_bChangeAnimation)
	{
		m_fConvertRatio += fTimeDelta * 5.f;

		CEdit_Model::Change_Animation();

		if (m_fConvertRatio > 1.f)
		{
			m_bChangeAnimation = FALSE;
		}
	}
	else
		fCurrentAnimPosition = m_Animations[m_iCurrentAnimIndex]->Invalidate_TransformationMatrix_Half(isLoop, m_Bones, fTimeDelta);

	/* 모든 뼈들의 CombinedTransformationMatrix를 갱신한다. */
	for (auto& pBone : m_Bones)
	{
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones);
	}

	return fCurrentAnimPosition;
}

void CEdit_Model::Change_Animation()
{
	_vector			vScale;
	_vector			vRotation;
	_vector			vTranslation;

	_vector			vSourScale, vDestScale;
	_vector			vSourRotation, vDestRotation;
	_vector			vSourTranslation, vDestTranslation;

	for (_uint iBoneIdx = 0; iBoneIdx < m_Bones.size(); ++iBoneIdx)
	{
		CEdit_Channel* pPrevBoneCH = m_Animations[m_iPrevAnimIndex]->Find_Channel(iBoneIdx);
		CEdit_Channel* pNextBoneCH = m_Animations[m_iCurrentAnimIndex]->Find_Channel(iBoneIdx);

		_float4 vPrevBoneScale = pPrevBoneCH->Get_PrevFrame().vScale;
		_float4 vPrevBoneRotation = pPrevBoneCH->Get_PrevFrame().vRotation;
		_float4 vPrevBoneTranslation = pPrevBoneCH->Get_PrevFrame().vTranslation;

		_float4 vNextBoneScale = pNextBoneCH->Get_FirstFrame().vScale;
		_float4 vNextBoneRotation = pNextBoneCH->Get_FirstFrame().vRotation;
		_float4 vNextBoneTranslation = pNextBoneCH->Get_FirstFrame().vTranslation;

		_float4x4 vTransformationMatrix = m_Bones[iBoneIdx]->Get_TransformationMatrix();

		if (pPrevBoneCH == nullptr && pNextBoneCH == nullptr)
			continue;

		else if (pPrevBoneCH == nullptr)
		{
			XMMatrixDecompose(&vSourScale, &vSourRotation, &vSourTranslation, XMLoadFloat4x4(&vTransformationMatrix));

			vDestScale = XMLoadFloat4(&vNextBoneScale);
			vScale = XMVectorLerp(vSourScale, vDestScale, m_fConvertRatio);

			vDestRotation = XMLoadFloat4(&vNextBoneRotation);
			vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, m_fConvertRatio);

			vDestTranslation = XMLoadFloat4(&vNextBoneTranslation);
			vTranslation = XMVectorLerp(vSourTranslation, vDestTranslation, m_fConvertRatio);

			m_Bones[iBoneIdx]->Set_TransformationMatrix(XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation));
		}

		else if (pNextBoneCH == nullptr)
		{
			if (XMVector4Equal(XMLoadFloat4(&vPrevBoneScale), { 0.f,0.f,0.f,0.f }) ||
				XMVector4Equal(XMLoadFloat4(&vPrevBoneRotation), { 0.f,0.f,0.f,0.f }) ||
				XMVector4Equal(XMLoadFloat4(&vPrevBoneTranslation), { 0.f,0.f,0.f,0.f }))
			{
				XMMatrixDecompose(&vScale, &vRotation, &vTranslation, XMLoadFloat4x4(&vTransformationMatrix));
			}
			else
			{
				vScale = XMLoadFloat4(&vPrevBoneScale);
				vRotation = XMLoadFloat4(&vPrevBoneRotation);
				vTranslation = XMLoadFloat4(&vPrevBoneTranslation);
			}

			m_Bones[iBoneIdx]->Set_TransformationMatrix(XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation));
		}

		else
		{
			if (XMVector4Equal(XMLoadFloat4(&vPrevBoneScale), { 0.f,0.f,0.f,0.f }) ||
				XMVector4Equal(XMLoadFloat4(&vPrevBoneRotation), { 0.f,0.f,0.f,0.f }) ||
				XMVector4Equal(XMLoadFloat4(&vPrevBoneTranslation), { 0.f,0.f,0.f,0.f }))
			{
				XMMatrixDecompose(&vSourScale, &vSourRotation, &vSourTranslation, XMLoadFloat4x4(&vTransformationMatrix));
			}
			else
			{
				vSourScale = XMLoadFloat4(&vPrevBoneScale);
				vSourRotation = XMLoadFloat4(&vPrevBoneRotation);
				vSourTranslation = XMLoadFloat4(&vPrevBoneTranslation);
			}

			vDestScale = XMLoadFloat4(&vNextBoneScale);
			vScale = XMVectorLerp(vSourScale, vDestScale, m_fConvertRatio);

			vDestRotation = XMLoadFloat4(&vNextBoneRotation);
			vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, m_fConvertRatio);

			vDestTranslation = XMLoadFloat4(&vNextBoneTranslation);
			vTranslation = XMVectorLerp(vSourTranslation, vDestTranslation, m_fConvertRatio);

			m_Bones[iBoneIdx]->Set_TransformationMatrix(XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation));
		}
	}
}

HRESULT CEdit_Model::Initialize_Prototype(const string& strModelFilePath)
{
	ifstream inFile(ConvertCToWC(strModelFilePath.c_str()), ios::binary | ios::in);

	if (inFile.is_open() == FALSE)
		return E_FAIL;

	/* Model Type */
	inFile.read(reinterpret_cast<char*>(&m_eModelType), sizeof _int);

	/* Pivot */
	inFile.read(reinterpret_cast<char*>(&m_PivotMatrix.m[0]), sizeof _float4);
	inFile.read(reinterpret_cast<char*>(&m_PivotMatrix.m[1]), sizeof _float4);
	inFile.read(reinterpret_cast<char*>(&m_PivotMatrix.m[2]), sizeof _float4);
	inFile.read(reinterpret_cast<char*>(&m_PivotMatrix.m[3]), sizeof _float4);

	if (m_eModelType == MTYPE_ANIM)
	{
		if (FAILED(Ready_Bones(inFile)))
		{
			inFile.close();
			return E_FAIL;
		}
	}

	if (FAILED(Ready_Meshes(inFile)))
	{
		inFile.close();
		return E_FAIL;
	}

	if (FAILED(Ready_Materials(inFile, strModelFilePath)))
	{
		inFile.close();
		return E_FAIL;
	}

	if (m_eModelType == MTYPE_ANIM)
	{
		if (FAILED(Ready_Animations(inFile)))
		{
			inFile.close();
			return E_FAIL;
		}
	}

	inFile.close();

	return S_OK;
}

HRESULT CEdit_Model::Initialize_Clone(void* pArg)
{
	return S_OK;
}

HRESULT CEdit_Model::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

CEdit_Model* CEdit_Model::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const string& strModelFilePath)
{
	CEdit_Model* pInstance = new CEdit_Model(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strModelFilePath)))
	{
		MSG_BOX(TEXT("Failed to Create : CEdit_Model"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CEdit_Model::Clone(void* pArg)
{
	CEdit_Model* pInstance = new CEdit_Model(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CEdit_Model"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEdit_Model::Free()
{
	__super::Free();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	m_Bones.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);

	m_Meshes.clear();

	for (auto& Material : m_Materials)
	{
		for (size_t i = 0; i < ::AI_TEXTURE_TYPE_MAX; i++)
		{
			Safe_Release(Material.pMaterials[i]);
		}
	}

	m_Materials.clear();

	for (auto& Anim : m_Animations)
		Safe_Release(Anim);

	m_Animations.clear();
}
