#include "Convert_Manager.h"

#include <io.h>

#define F_OK 0

CConvert_Manager::CConvert_Manager()
{
}

HRESULT CConvert_Manager::Convert_Bones(aiNode* pNode, _int iParentBoneIndex)
{
	BONE_DESC tBone = { };

	/* Bone Name */
	strcpy_s(tBone.szName, pNode->mName.data);

	/* Parent Bone Idx */
	tBone.iParentBoneIndex = iParentBoneIndex;

	/* Transform Matrix */
	memcpy(&tBone.TransformationMatrix, &pNode->mTransformation, sizeof(_float4x4));
	XMStoreFloat4x4(&tBone.TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&tBone.TransformationMatrix)));

	m_Bones.push_back(tBone);

	_int iNextParentBoneIndex = static_cast<_int>(m_Bones.size()) - 1;

	for (size_t siBoneChild = 0; siBoneChild < pNode->mNumChildren; ++siBoneChild)
	{
		Convert_Bones(pNode->mChildren[siBoneChild], iNextParentBoneIndex);
	}

	return S_OK;
}

HRESULT CConvert_Manager::Convert_Anim(const wstring& strModelFolderPath)
{
	HANDLE hFind;
	WIN32_FIND_DATA tagFindData;

	_bool bResult = TRUE;

	_tchar	szDir[MAX_PATH] = TEXT("");
	_tchar	szEXT[MAX_PATH] = TEXT("");
	wstring strNewFilePath;
	wstring strFullPath;

	hFind = FindFirstFile(strModelFolderPath.c_str(), &tagFindData);

	if (hFind == INVALID_HANDLE_VALUE)
		return E_FAIL;

	_wsplitpath_s(strModelFolderPath.c_str(), NULL, NULL, szDir, MAX_PATH, NULL, NULL, NULL, NULL);

	while (bResult)
	{
		_wsplitpath_s(tagFindData.cFileName, NULL, NULL, NULL, NULL, NULL, NULL, szEXT, MAX_PATH);

		if (tagFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (lstrcmp(tagFindData.cFileName, L".") && lstrcmp(tagFindData.cFileName, L".."))
			{
				strNewFilePath = L"";
				strNewFilePath += szDir;
				strNewFilePath += tagFindData.cFileName;
				strNewFilePath += L"/*.*";

				wstring strFolderTag = tagFindData.cFileName;

				Convert_Anim(strNewFilePath);
			}
		}
		else if (!lstrcmp(szEXT, TEXT(".fbx")))
		{
			strFullPath = L"";
			strFullPath += szDir;
			strFullPath += tagFindData.cFileName;

			/* Create Pivot Matrix */
			_matrix PivotMatrix = XMMatrixIdentity();

			_vector vRotation = 
				XMQuaternionRotationRollPitchYaw(
					XMConvertToRadians(m_vPivotRotation.x),
					XMConvertToRadians(m_vPivotRotation.y),
					XMConvertToRadians(m_vPivotRotation.z));

			PivotMatrix = XMMatrixAffineTransformation(XMLoadFloat4(&m_vPivotScale), { 0.f, 0.f, 0.f, 1.f }, vRotation, XMLoadFloat4(&m_vPivotTranslation));

			/* Convert */
			if (FAILED(Convert_AnimModel(ConvertWCToC(strFullPath.c_str()), PivotMatrix)))
				return E_FAIL;
		}

		bResult = FindNextFile(hFind, &tagFindData);
	}

	FindClose(hFind);

	return S_OK;
}

HRESULT CConvert_Manager::Convert_NonAnim(const wstring& strModelFolderPath)
{
	HANDLE hFind;
	WIN32_FIND_DATA tagFindData;

	_bool bResult = TRUE;

	_tchar	szDir[MAX_PATH] = TEXT("");
	_tchar	szEXT[MAX_PATH] = TEXT("");
	wstring strNewFilePath;
	wstring strFullPath;

	hFind = FindFirstFile(strModelFolderPath.c_str(), &tagFindData);

	if (hFind == INVALID_HANDLE_VALUE)
		return E_FAIL;

	_wsplitpath_s(strModelFolderPath.c_str(), NULL, NULL, szDir, MAX_PATH, NULL, NULL, NULL, NULL);

	while (bResult)
	{
		_wsplitpath_s(tagFindData.cFileName, NULL, NULL, NULL, NULL, NULL, NULL, szEXT, MAX_PATH);

		if (tagFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (lstrcmp(tagFindData.cFileName, L".") && lstrcmp(tagFindData.cFileName, L".."))
			{
				strNewFilePath = L"";
				strNewFilePath += szDir;
				strNewFilePath += tagFindData.cFileName;
				strNewFilePath += L"/*.*";

				wstring strFolderTag = tagFindData.cFileName;

				Convert_NonAnim(strNewFilePath);
			}
		}
		else if (!lstrcmp(szEXT, TEXT(".fbx")))
		{
			strFullPath = L"";
			strFullPath += szDir;
			strFullPath += tagFindData.cFileName;

			/* Create Pivot Matrix */
			_matrix PivotMatrix = XMMatrixIdentity();

			_vector vRotation =
				XMQuaternionRotationRollPitchYaw(
					XMConvertToRadians(m_vPivotRotation.x),
					XMConvertToRadians(m_vPivotRotation.y),
					XMConvertToRadians(m_vPivotRotation.z));

			PivotMatrix = XMMatrixAffineTransformation(XMLoadFloat4(&m_vPivotScale), { 0.f, 0.f, 0.f, 1.f }, vRotation, XMLoadFloat4(&m_vPivotTranslation));

			/* Convert */
			if (FAILED(Convert_NonAnimModel(ConvertWCToC(strFullPath.c_str()), PivotMatrix)))
				return E_FAIL;
		}

		bResult = FindNextFile(hFind, &tagFindData);
	}

	FindClose(hFind);

	return S_OK;
}

HRESULT CConvert_Manager::Convert_Anim_SPToonShader(const wstring& strModelFolderPath)
{
	HANDLE hFind;
	WIN32_FIND_DATA tagFindData;

	_bool bResult = TRUE;

	_tchar	szDir[MAX_PATH] = TEXT("");
	_tchar	szEXT[MAX_PATH] = TEXT("");
	wstring strNewFilePath;
	wstring strFullPath;

	hFind = FindFirstFile(strModelFolderPath.c_str(), &tagFindData);

	if (hFind == INVALID_HANDLE_VALUE)
		return E_FAIL;

	_wsplitpath_s(strModelFolderPath.c_str(), NULL, NULL, szDir, MAX_PATH, NULL, NULL, NULL, NULL);

	while (bResult)
	{
		_wsplitpath_s(tagFindData.cFileName, NULL, NULL, NULL, NULL, NULL, NULL, szEXT, MAX_PATH);

		if (tagFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (lstrcmp(tagFindData.cFileName, L".") && lstrcmp(tagFindData.cFileName, L".."))
			{
				strNewFilePath = L"";
				strNewFilePath += szDir;
				strNewFilePath += tagFindData.cFileName;
				strNewFilePath += L"/*.*";

				wstring strFolderTag = tagFindData.cFileName;

				Convert_Anim_SPToonShader(strNewFilePath);
			}
		}
		else if (!lstrcmp(szEXT, TEXT(".fbx")))
		{
			strFullPath = L"";
			strFullPath += szDir;
			strFullPath += tagFindData.cFileName;

			/* Create Pivot Matrix */
			_matrix PivotMatrix = XMMatrixIdentity();

			_vector vRotation =
				XMQuaternionRotationRollPitchYaw(
					XMConvertToRadians(m_vPivotRotation.x),
					XMConvertToRadians(m_vPivotRotation.y),
					XMConvertToRadians(m_vPivotRotation.z));

			PivotMatrix = XMMatrixAffineTransformation(XMLoadFloat4(&m_vPivotScale), { 0.f, 0.f, 0.f, 1.f }, vRotation, XMLoadFloat4(&m_vPivotTranslation));

			/* Convert */
			if (FAILED(Convert_AnimModel_SPToonShader(ConvertWCToC(strFullPath.c_str()), PivotMatrix)))
				return E_FAIL;
		}

		bResult = FindNextFile(hFind, &tagFindData);
	}

	FindClose(hFind);

	return S_OK;
}

HRESULT CConvert_Manager::Convert_AnimModel(const string& strModelFilePath, _fmatrix PivotMatrix)
{
	_uint iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	m_pAIScene = m_Importer.ReadFile(strModelFilePath, iFlag);
	if (m_pAIScene == nullptr)
		return E_FAIL;

	/* Save File */
	_char	szDrive[MAX_PATH] = ("");
	_char	szDir[MAX_PATH] = ("");
	_char	szFileName[MAX_PATH] = ("");
	wstring strNewFilePath;

	_splitpath_s(strModelFilePath.c_str(), szDrive, MAX_PATH, szDir, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);
	string strConvertPath = szDrive; strConvertPath += szDir; strConvertPath += szFileName; strConvertPath += ".dat";

	ofstream outFile(ConvertCToWC(strConvertPath.c_str()), ios::binary | ios::out);
	if (outFile.is_open() == FALSE) {
		return E_FAIL;
	}

	_uint	iStore = { };
	_float4 vStore = { };

	/* Model Type */
	iStore = 1;
	outFile.write(reinterpret_cast<char*>(&iStore), sizeof _int);

	/* Pivot Matrix */
	XMStoreFloat4(&vStore, PivotMatrix.r[0]);
	outFile.write(reinterpret_cast<char*>(&vStore), sizeof _float4);

	XMStoreFloat4(&vStore, PivotMatrix.r[1]);
	outFile.write(reinterpret_cast<char*>(&vStore), sizeof _float4);

	XMStoreFloat4(&vStore, PivotMatrix.r[2]);
	outFile.write(reinterpret_cast<char*>(&vStore), sizeof _float4);

	XMStoreFloat4(&vStore, PivotMatrix.r[3]);
	outFile.write(reinterpret_cast<char*>(&vStore), sizeof _float4);

	/* 전체 Bones */
	if (FAILED(Convert_Bones(m_pAIScene->mRootNode, -1)))
		return E_FAIL;

	iStore = static_cast<_uint>(m_Bones.size());
	outFile.write(reinterpret_cast<char*>(&iStore), sizeof _int);

	for (auto& iter : m_Bones)
	{
		/* Bone Name */
		outFile.write(reinterpret_cast<char*>(&iter.szName), sizeof(char[MAX_PATH]));

		/* Parent Bone Idx */
		outFile.write(reinterpret_cast<char*>(&iter.iParentBoneIndex), sizeof _int);

		/* Transform Matrix */
		outFile.write(reinterpret_cast<char*>(&iter.TransformationMatrix.m[0]), sizeof _float4);
		outFile.write(reinterpret_cast<char*>(&iter.TransformationMatrix.m[1]), sizeof _float4);
		outFile.write(reinterpret_cast<char*>(&iter.TransformationMatrix.m[2]), sizeof _float4);
		outFile.write(reinterpret_cast<char*>(&iter.TransformationMatrix.m[3]), sizeof _float4);
	}

	/* MESH */
	iStore = m_pAIScene->mNumMeshes;
	for (_uint iIndex = 0; iIndex < m_pAIScene->mNumMeshes; iIndex++)
	{
		const	aiMesh* pAIMesh = m_pAIScene->mMeshes[iIndex];
		if (pAIMesh->mTextureCoords[0] == nullptr)
			--iStore;
	}

	outFile.write(reinterpret_cast<char*>(&iStore), sizeof _uint);

	for (_uint iIndex = 0; iIndex < m_pAIScene->mNumMeshes; iIndex++)
	{
		const	aiMesh* pAIMesh = m_pAIScene->mMeshes[iIndex];
		if (pAIMesh->mTextureCoords[0] == nullptr)
			continue;

		char	szName[MAX_PATH];	strcpy_s(szName, pAIMesh->mName.data);
		outFile.write(reinterpret_cast<char*>(&szName), sizeof(char[MAX_PATH]));

		iStore = pAIMesh->mMaterialIndex;
		outFile.write(reinterpret_cast<char*>(&iStore), sizeof _uint);

		iStore = pAIMesh->mNumVertices;
		outFile.write(reinterpret_cast<char*>(&iStore), sizeof _uint);

		iStore = pAIMesh->mNumFaces * 3;
		outFile.write(reinterpret_cast<char*>(&iStore), sizeof _uint);

		aiVector3D* aiVertices = pAIMesh->mVertices;
		outFile.write(reinterpret_cast<char*>(aiVertices), sizeof _float3 * pAIMesh->mNumVertices);

		aiVector3D* aiNormals = pAIMesh->mNormals;
		outFile.write(reinterpret_cast<char*>(aiNormals), sizeof _float3 * pAIMesh->mNumVertices);

		aiVector3D* aiTextureCoords = pAIMesh->mTextureCoords[0];
		outFile.write(reinterpret_cast<char*>(aiTextureCoords), sizeof _float3 * pAIMesh->mNumVertices);

		aiVector3D* aiTangents = pAIMesh->mTangents;
		outFile.write(reinterpret_cast<char*>(aiTangents), sizeof _float3 * pAIMesh->mNumVertices);

		iStore = pAIMesh->mNumBones;	/* Mesh에 영향을 주는 뼈 개수 */
		outFile.write(reinterpret_cast<char*>(&iStore), sizeof _uint);

		/* Offset Matices, Bone Indices */
		_float4x4* OffsetMatices = new _float4x4[pAIMesh->mNumBones];
		_uint* BoneIndices = new _uint[pAIMesh->mNumBones];

		/* Blend Index, Blend Weight */
		XMUINT4* vBlendIndices = new XMUINT4[pAIMesh->mNumVertices];
		ZeroMemory(vBlendIndices, sizeof XMUINT4 * pAIMesh->mNumVertices);

		XMFLOAT4* vBlendWeights = new XMFLOAT4[pAIMesh->mNumVertices];
		ZeroMemory(vBlendWeights, sizeof XMFLOAT4 * pAIMesh->mNumVertices);

		/* Mesh에 영향을 주는 뼈를 순회 */
		for (size_t siBoneIdx = 0; siBoneIdx < pAIMesh->mNumBones; ++siBoneIdx)
		{
			/* Idx 번째의 뼈를 선택 */
			aiBone* pAIBone = pAIMesh->mBones[siBoneIdx];

			/* Idx 번째 뼈의 Offset Matrix를 저장 */
			memcpy(&OffsetMatices[siBoneIdx], &pAIBone->mOffsetMatrix, sizeof(_float4x4));
			XMStoreFloat4x4(&OffsetMatices[siBoneIdx], XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatices[siBoneIdx])));

			/* Mesh에 영향을 주는 Idx 번째 뼈의, 전체 뼈 중에서의 Idx를 저장 */
			_uint	iBoneIndex = 0;

			auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](BONE_DESC pBone)->_bool {
				if (0 == strcmp(pBone.szName, pAIBone->mName.data))
					return true;
				++iBoneIndex;
				return false;
				});

			BoneIndices[siBoneIdx] = iBoneIndex;

			/* 이 뼈가 영향을 미치는 여러 개의 정점을 순회 */
			for (size_t siVertex = 0; siVertex < pAIBone->mNumWeights; ++siVertex)
			{
				/* 한 정점에는 최대 4개 까지의 뼈가 영향을 미칠 수 있음 */
				if (0.0f == vBlendWeights[pAIBone->mWeights[siVertex].mVertexId].x)
				{
					vBlendIndices[pAIBone->mWeights[siVertex].mVertexId].x = static_cast<uint32_t>(siBoneIdx);
					vBlendWeights[pAIBone->mWeights[siVertex].mVertexId].x = pAIBone->mWeights[siVertex].mWeight;
				}
				else if (0.0f == vBlendWeights[pAIBone->mWeights[siVertex].mVertexId].y)
				{
					vBlendIndices[pAIBone->mWeights[siVertex].mVertexId].y = static_cast<uint32_t>(siBoneIdx);
					vBlendWeights[pAIBone->mWeights[siVertex].mVertexId].y = pAIBone->mWeights[siVertex].mWeight;
				}
				else if (0.0f == vBlendWeights[pAIBone->mWeights[siVertex].mVertexId].z)
				{
					vBlendIndices[pAIBone->mWeights[siVertex].mVertexId].z = static_cast<uint32_t>(siBoneIdx);
					vBlendWeights[pAIBone->mWeights[siVertex].mVertexId].z = pAIBone->mWeights[siVertex].mWeight;
				}
				else if (0.0f == vBlendWeights[pAIBone->mWeights[siVertex].mVertexId].w)
				{
					vBlendIndices[pAIBone->mWeights[siVertex].mVertexId].w = static_cast<uint32_t>(siBoneIdx);
					vBlendWeights[pAIBone->mWeights[siVertex].mVertexId].w = pAIBone->mWeights[siVertex].mWeight;
				}
			}
		}

		outFile.write(reinterpret_cast<char*>(OffsetMatices), sizeof _float4x4 * pAIMesh->mNumBones);
		outFile.write(reinterpret_cast<char*>(BoneIndices), sizeof _uint * pAIMesh->mNumBones);

		outFile.write(reinterpret_cast<char*>(vBlendIndices), sizeof XMUINT4 * pAIMesh->mNumVertices);
		outFile.write(reinterpret_cast<char*>(vBlendWeights), sizeof XMFLOAT4 * pAIMesh->mNumVertices);

		Safe_Delete_Array(OffsetMatices);
		Safe_Delete_Array(BoneIndices);

		Safe_Delete_Array(vBlendIndices);
		Safe_Delete_Array(vBlendWeights);

		for (size_t siIndex = 0; siIndex < pAIMesh->mNumFaces; ++siIndex)
		{
			aiFace	AIFace = pAIMesh->mFaces[siIndex];

			_uint* iIndices = AIFace.mIndices;
			outFile.write(reinterpret_cast<char*>(iIndices), sizeof _uint * 3);
		}
	}

	/* MATERIAL */
	iStore = m_pAIScene->mNumMaterials;
	outFile.write(reinterpret_cast<char*>(&iStore), sizeof _uint);

	for (size_t iIndex = 0; iIndex < m_pAIScene->mNumMaterials; iIndex++)
	{
		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[iIndex];

		map<_uint, aiString>	TexturePath;
		for (_uint iMaterial = 0; iMaterial < ::AI_TEXTURE_TYPE_MAX; iMaterial++)
		{
			aiString		strTexturePath;

			if (FAILED(pAIMaterial->GetTexture(aiTextureType(iMaterial), 0, &strTexturePath)))
				continue;

			TexturePath.emplace(iMaterial, strTexturePath);
		}

		_uint iNumTexture = static_cast<_uint>(TexturePath.size());
		outFile.write(reinterpret_cast<char*>(&iNumTexture), sizeof _uint);

		for (auto& Pair : TexturePath)
		{
			outFile.write(reinterpret_cast<const char*>(&Pair.first), sizeof _uint);

			char	szPath[MAX_PATH];	strcpy_s(szPath, Pair.second.C_Str());
			outFile.write(reinterpret_cast<char*>(&szPath), sizeof(char[MAX_PATH]));
		}

		TexturePath.clear();
	}

	/* Animation */
	iStore = m_pAIScene->mNumAnimations;
	outFile.write(reinterpret_cast<char*>(&iStore), sizeof _uint);

	for (size_t iAnimation = 0; iAnimation < m_pAIScene->mNumAnimations; ++iAnimation)
	{
		aiAnimation* pAIAnimation = m_pAIScene->mAnimations[iAnimation];

		char	szName[MAX_PATH];	strcpy_s(szName, pAIAnimation->mName.data);
		outFile.write(reinterpret_cast<char*>(&szName), sizeof(char[MAX_PATH]));

		outFile.write(reinterpret_cast<char*>(&pAIAnimation->mDuration), sizeof _double);
		outFile.write(reinterpret_cast<char*>(&pAIAnimation->mTicksPerSecond), sizeof _double);
		outFile.write(reinterpret_cast<char*>(&pAIAnimation->mNumChannels), sizeof _uint);

		/* Channel */
		for (size_t iChannel = 0; iChannel < pAIAnimation->mNumChannels; ++iChannel)
		{
			char	szName[MAX_PATH];	strcpy_s(szName, pAIAnimation->mChannels[iChannel]->mNodeName.data);
			outFile.write(reinterpret_cast<char*>(&szName), sizeof(char[MAX_PATH]));

			_uint		iChannelBoneIndex = 0;

			auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](BONE_DESC pBone)->_bool {
				if (0 == strcmp(pBone.szName, pAIAnimation->mChannels[iChannel]->mNodeName.data))
					return true;
				++iChannelBoneIndex;
				return false;
				});

			outFile.write(reinterpret_cast<char*>(&iChannelBoneIndex), sizeof _uint);

			_uint iNumKeyFrames = 0;

			iNumKeyFrames = max(pAIAnimation->mChannels[iChannel]->mNumScalingKeys, pAIAnimation->mChannels[iChannel]->mNumRotationKeys);
			iNumKeyFrames = max(iNumKeyFrames, pAIAnimation->mChannels[iChannel]->mNumPositionKeys);
			outFile.write(reinterpret_cast<char*>(&iNumKeyFrames), sizeof _uint);

			KEYFRAME			KeyFrame = {};
			_float4				vScale = {}, vRotation = {}, vTranslation = { 0.0f, 0.f, 0.f, 1.f };

			for (size_t iKeyFrame = 0; iKeyFrame < iNumKeyFrames; ++iKeyFrame)
			{
				if (iKeyFrame < pAIAnimation->mChannels[iChannel]->mNumScalingKeys)
				{
					memcpy(&vScale, &pAIAnimation->mChannels[iChannel]->mScalingKeys[iKeyFrame].mValue, sizeof(_float3));

					KeyFrame.fTime = static_cast<_float>(pAIAnimation->mChannels[iChannel]->mScalingKeys[iKeyFrame].mTime);
				}

				if (iKeyFrame < pAIAnimation->mChannels[iChannel]->mNumRotationKeys)
				{
					vRotation.x = pAIAnimation->mChannels[iChannel]->mRotationKeys[iKeyFrame].mValue.x;
					vRotation.y = pAIAnimation->mChannels[iChannel]->mRotationKeys[iKeyFrame].mValue.y;
					vRotation.z = pAIAnimation->mChannels[iChannel]->mRotationKeys[iKeyFrame].mValue.z;
					vRotation.w = pAIAnimation->mChannels[iChannel]->mRotationKeys[iKeyFrame].mValue.w;

					KeyFrame.fTime = static_cast<_float>(pAIAnimation->mChannels[iChannel]->mRotationKeys[iKeyFrame].mTime);
				}

				if (iKeyFrame < pAIAnimation->mChannels[iChannel]->mNumPositionKeys)
				{
					memcpy(&vTranslation, &pAIAnimation->mChannels[iChannel]->mPositionKeys[iKeyFrame].mValue, sizeof(_float3));

					KeyFrame.fTime = static_cast<_float>(pAIAnimation->mChannels[iChannel]->mPositionKeys[iKeyFrame].mTime);
				}

				outFile.write(reinterpret_cast<char*>(&KeyFrame.fTime), sizeof _float);
				outFile.write(reinterpret_cast<char*>(&vScale), sizeof XMFLOAT4);
				outFile.write(reinterpret_cast<char*>(&vRotation), sizeof XMFLOAT4);
				outFile.write(reinterpret_cast<char*>(&vTranslation), sizeof XMFLOAT4);
			}
		}
	}

	m_Bones.clear();

	outFile.close();

	m_Importer.FreeScene();

	return S_OK;
}

HRESULT CConvert_Manager::Convert_NonAnimModel(const string& strModelFilePath, _fmatrix PivotMatrix)
{
	_uint iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	m_pAIScene = m_Importer.ReadFile(strModelFilePath, iFlag);
	if (m_pAIScene == nullptr)
		return E_FAIL;

	/* Save File */
	_char	szDrive[MAX_PATH] = ("");
	_char	szDir[MAX_PATH] = ("");
	_char	szFileName[MAX_PATH] = ("");
	wstring strNewFilePath;

	_splitpath_s(strModelFilePath.c_str(), szDrive, MAX_PATH, szDir, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);
	string strConvertPath = szDrive; strConvertPath += szDir; strConvertPath += szFileName; strConvertPath += ".dat";

	ofstream outFile(ConvertCToWC(strConvertPath.c_str()), ios::binary | ios::out);
	if (outFile.is_open() == FALSE) {
		return E_FAIL;
	}

	_uint	iStore = { };
	_float4 vStore = { };

	/* Model Type */
	iStore = 0;
	outFile.write(reinterpret_cast<char*>(&iStore), sizeof _int);

	/* Pivot Matrix */
	XMStoreFloat4(&vStore, PivotMatrix.r[0]);
	outFile.write(reinterpret_cast<char*>(&vStore), sizeof _float4);

	XMStoreFloat4(&vStore, PivotMatrix.r[1]);
	outFile.write(reinterpret_cast<char*>(&vStore), sizeof _float4);

	XMStoreFloat4(&vStore, PivotMatrix.r[2]);
	outFile.write(reinterpret_cast<char*>(&vStore), sizeof _float4);

	XMStoreFloat4(&vStore, PivotMatrix.r[3]);
	outFile.write(reinterpret_cast<char*>(&vStore), sizeof _float4);

	/* MESH */
	iStore = m_pAIScene->mNumMeshes;
	for (_uint iIndex = 0; iIndex < m_pAIScene->mNumMeshes; iIndex++)
	{
		const	aiMesh* pAIMesh = m_pAIScene->mMeshes[iIndex];
		if (pAIMesh->mTextureCoords[0] == nullptr)
			--iStore;
	}

	outFile.write(reinterpret_cast<char*>(&iStore), sizeof _uint);

	for (_uint iIndex = 0; iIndex < m_pAIScene->mNumMeshes; iIndex++)
	{
		const	aiMesh* pAIMesh = m_pAIScene->mMeshes[iIndex];
		if (pAIMesh->mTextureCoords[0] == nullptr)
			continue;

		char	szName[MAX_PATH];	strcpy_s(szName, pAIMesh->mName.data);
		outFile.write(reinterpret_cast<char*>(&szName), sizeof(char[MAX_PATH]));

		iStore = pAIMesh->mMaterialIndex;	/* Mesh에 Bind되는 Material Idx */
		outFile.write(reinterpret_cast<char*>(&iStore), sizeof _uint);

		iStore = pAIMesh->mNumVertices;		/* Mesh의 정점 개수 */
		outFile.write(reinterpret_cast<char*>(&iStore), sizeof _uint);

		iStore = pAIMesh->mNumFaces * 3;	/* Mesh의 삼각형(Face) 개수*/
		outFile.write(reinterpret_cast<char*>(&iStore), sizeof _uint);

		/* 정점 정보 */
		aiVector3D* aiVertices = pAIMesh->mVertices;
		outFile.write(reinterpret_cast<char*>(aiVertices), sizeof _float3 * pAIMesh->mNumVertices);

		aiVector3D* aiNormals = pAIMesh->mNormals;
		outFile.write(reinterpret_cast<char*>(aiNormals), sizeof _float3 * pAIMesh->mNumVertices);

		aiVector3D* aiTextureCoords = pAIMesh->mTextureCoords[0];
		outFile.write(reinterpret_cast<char*>(aiTextureCoords), sizeof _float3 * pAIMesh->mNumVertices);

		aiVector3D* aiTangents = pAIMesh->mTangents;
		if (aiTangents == nullptr)
			outFile.write(reinterpret_cast<char*>(aiTangents), sizeof _float3 * pAIMesh->mNumVertices);
		else
			outFile.write(reinterpret_cast<char*>(aiTangents), sizeof _float3 * pAIMesh->mNumVertices);

		for (size_t siIndex = 0; siIndex < pAIMesh->mNumFaces; ++siIndex)
		{
			aiFace	AIFace = pAIMesh->mFaces[siIndex];

			_uint* iIndices = AIFace.mIndices;
			outFile.write(reinterpret_cast<char*>(iIndices), sizeof _uint * 3);
		}
	}

	/* MATERIAL */
	iStore = m_pAIScene->mNumMaterials;
	outFile.write(reinterpret_cast<char*>(&iStore), sizeof _uint);

	for (_uint iIndex = 0; iIndex < m_pAIScene->mNumMaterials; iIndex++)
	{
		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[iIndex];

		map<_uint, aiString>	TexturePath;
		for (_uint iMaterial = 0; iMaterial < ::AI_TEXTURE_TYPE_MAX; iMaterial++)
		{
			aiString		strTexturePath;

			if (FAILED(pAIMaterial->GetTexture(aiTextureType(iMaterial), 0, &strTexturePath)))
				continue;

			TexturePath.emplace(iMaterial, strTexturePath);
		}

		_uint iNumTexture = static_cast<_uint>(TexturePath.size());
		outFile.write(reinterpret_cast<char*>(&iNumTexture), sizeof _uint);

		for (auto& Pair : TexturePath)
		{
			outFile.write(reinterpret_cast<const char*>(&Pair.first), sizeof _uint);

			char	szPath[MAX_PATH];	strcpy_s(szPath, Pair.second.C_Str());
			outFile.write(reinterpret_cast<char*>(&szPath), sizeof(char[MAX_PATH]));
		}

		TexturePath.clear();
	}

	outFile.close();

	m_Importer.FreeScene();

	return S_OK;
}

HRESULT CConvert_Manager::Convert_AnimModel_SPToonShader(const string& strModelFilePath, _fmatrix PivotMatrix)
{
	_uint iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	m_pAIScene = m_Importer.ReadFile(strModelFilePath, iFlag);
	if (m_pAIScene == nullptr)
		return E_FAIL;

	/* Save File */
	_char	szDrive[MAX_PATH] = ("");
	_char	szDir[MAX_PATH] = ("");
	_char	szFileName[MAX_PATH] = ("");
	wstring strNewFilePath;

	_splitpath_s(strModelFilePath.c_str(), szDrive, MAX_PATH, szDir, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);
	string strConvertPath = szDrive; strConvertPath += szDir; strConvertPath += szFileName; strConvertPath += ".dat";

	ofstream outFile(ConvertCToWC(strConvertPath.c_str()), ios::binary | ios::out);
	if (outFile.is_open() == FALSE) {
		return E_FAIL;
	}

	_uint	iStore = { };
	_float4 vStore = { };

	/* Model Type */
	iStore = 1;
	outFile.write(reinterpret_cast<char*>(&iStore), sizeof _int);

	/* Pivot Matrix */
	XMStoreFloat4(&vStore, PivotMatrix.r[0]);
	outFile.write(reinterpret_cast<char*>(&vStore), sizeof _float4);

	XMStoreFloat4(&vStore, PivotMatrix.r[1]);
	outFile.write(reinterpret_cast<char*>(&vStore), sizeof _float4);

	XMStoreFloat4(&vStore, PivotMatrix.r[2]);
	outFile.write(reinterpret_cast<char*>(&vStore), sizeof _float4);

	XMStoreFloat4(&vStore, PivotMatrix.r[3]);
	outFile.write(reinterpret_cast<char*>(&vStore), sizeof _float4);

	/* 전체 Bones */
	if (FAILED(Convert_Bones(m_pAIScene->mRootNode, -1)))
		return E_FAIL;

	iStore = static_cast<_uint>(m_Bones.size());
	outFile.write(reinterpret_cast<char*>(&iStore), sizeof _int);

	for (auto& iter : m_Bones)
	{
		/* Bone Name */
		outFile.write(reinterpret_cast<char*>(&iter.szName), sizeof(char[MAX_PATH]));

		/* Parent Bone Idx */
		outFile.write(reinterpret_cast<char*>(&iter.iParentBoneIndex), sizeof _int);

		/* Transform Matrix */
		outFile.write(reinterpret_cast<char*>(&iter.TransformationMatrix.m[0]), sizeof _float4);
		outFile.write(reinterpret_cast<char*>(&iter.TransformationMatrix.m[1]), sizeof _float4);
		outFile.write(reinterpret_cast<char*>(&iter.TransformationMatrix.m[2]), sizeof _float4);
		outFile.write(reinterpret_cast<char*>(&iter.TransformationMatrix.m[3]), sizeof _float4);
	}

	/* MESH */
	iStore = m_pAIScene->mNumMeshes;
	for (_uint iIndex = 0; iIndex < m_pAIScene->mNumMeshes; iIndex++)
	{
		const	aiMesh* pAIMesh = m_pAIScene->mMeshes[iIndex];
		if (pAIMesh->mTextureCoords[0] == nullptr)
			--iStore;
	}

	outFile.write(reinterpret_cast<char*>(&iStore), sizeof _uint);

	for (_uint iIndex = 0; iIndex < m_pAIScene->mNumMeshes; iIndex++)
	{
		const	aiMesh* pAIMesh = m_pAIScene->mMeshes[iIndex];
		if (pAIMesh->mTextureCoords[0] == nullptr)
			continue;

		char	szName[MAX_PATH];	strcpy_s(szName, pAIMesh->mName.data);
		outFile.write(reinterpret_cast<char*>(&szName), sizeof(char[MAX_PATH]));

		iStore = pAIMesh->mMaterialIndex;
		outFile.write(reinterpret_cast<char*>(&iStore), sizeof _uint);

		iStore = pAIMesh->mNumVertices;
		outFile.write(reinterpret_cast<char*>(&iStore), sizeof _uint);

		iStore = pAIMesh->mNumFaces * 3;
		outFile.write(reinterpret_cast<char*>(&iStore), sizeof _uint);

		aiVector3D* aiVertices = pAIMesh->mVertices;
		outFile.write(reinterpret_cast<char*>(aiVertices), sizeof _float3 * pAIMesh->mNumVertices);

		aiVector3D* aiNormals = pAIMesh->mNormals;
		outFile.write(reinterpret_cast<char*>(aiNormals), sizeof _float3 * pAIMesh->mNumVertices);

		aiVector3D* aiTextureCoords = pAIMesh->mTextureCoords[0];
		outFile.write(reinterpret_cast<char*>(aiTextureCoords), sizeof _float3 * pAIMesh->mNumVertices);

		aiVector3D* aiTangents = pAIMesh->mTangents;
		outFile.write(reinterpret_cast<char*>(aiTangents), sizeof _float3 * pAIMesh->mNumVertices);

		iStore = pAIMesh->mNumBones;	/* Mesh에 영향을 주는 뼈 개수 */
		outFile.write(reinterpret_cast<char*>(&iStore), sizeof _uint);

		/* Offset Matices, Bone Indices */
		_float4x4* OffsetMatices = new _float4x4[pAIMesh->mNumBones];
		_uint* BoneIndices = new _uint[pAIMesh->mNumBones];

		/* Blend Index, Blend Weight */
		XMUINT4* vBlendIndices = new XMUINT4[pAIMesh->mNumVertices];
		ZeroMemory(vBlendIndices, sizeof XMUINT4 * pAIMesh->mNumVertices);

		XMFLOAT4* vBlendWeights = new XMFLOAT4[pAIMesh->mNumVertices];
		ZeroMemory(vBlendWeights, sizeof XMFLOAT4 * pAIMesh->mNumVertices);

		/* Mesh에 영향을 주는 뼈를 순회 */
		for (size_t siBoneIdx = 0; siBoneIdx < pAIMesh->mNumBones; ++siBoneIdx)
		{
			/* Idx 번째의 뼈를 선택 */
			aiBone* pAIBone = pAIMesh->mBones[siBoneIdx];

			/* Idx 번째 뼈의 Offset Matrix를 저장 */
			memcpy(&OffsetMatices[siBoneIdx], &pAIBone->mOffsetMatrix, sizeof(_float4x4));
			XMStoreFloat4x4(&OffsetMatices[siBoneIdx], XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatices[siBoneIdx])));

			/* Mesh에 영향을 주는 Idx 번째 뼈의, 전체 뼈 중에서의 Idx를 저장 */
			_uint	iBoneIndex = 0;

			auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](BONE_DESC pBone)->_bool {
				if (0 == strcmp(pBone.szName, pAIBone->mName.data))
					return true;
				++iBoneIndex;
				return false;
				});

			BoneIndices[siBoneIdx] = iBoneIndex;

			/* 이 뼈가 영향을 미치는 여러 개의 정점을 순회 */
			for (size_t siVertex = 0; siVertex < pAIBone->mNumWeights; ++siVertex)
			{
				/* 한 정점에는 최대 4개 까지의 뼈가 영향을 미칠 수 있음 */
				if (0.0f == vBlendWeights[pAIBone->mWeights[siVertex].mVertexId].x)
				{
					vBlendIndices[pAIBone->mWeights[siVertex].mVertexId].x = static_cast<uint32_t>(siBoneIdx);
					vBlendWeights[pAIBone->mWeights[siVertex].mVertexId].x = pAIBone->mWeights[siVertex].mWeight;
				}
				else if (0.0f == vBlendWeights[pAIBone->mWeights[siVertex].mVertexId].y)
				{
					vBlendIndices[pAIBone->mWeights[siVertex].mVertexId].y = static_cast<uint32_t>(siBoneIdx);
					vBlendWeights[pAIBone->mWeights[siVertex].mVertexId].y = pAIBone->mWeights[siVertex].mWeight;
				}
				else if (0.0f == vBlendWeights[pAIBone->mWeights[siVertex].mVertexId].z)
				{
					vBlendIndices[pAIBone->mWeights[siVertex].mVertexId].z = static_cast<uint32_t>(siBoneIdx);
					vBlendWeights[pAIBone->mWeights[siVertex].mVertexId].z = pAIBone->mWeights[siVertex].mWeight;
				}
				else if (0.0f == vBlendWeights[pAIBone->mWeights[siVertex].mVertexId].w)
				{
					vBlendIndices[pAIBone->mWeights[siVertex].mVertexId].w = static_cast<uint32_t>(siBoneIdx);
					vBlendWeights[pAIBone->mWeights[siVertex].mVertexId].w = pAIBone->mWeights[siVertex].mWeight;
				}
			}
		}

		outFile.write(reinterpret_cast<char*>(OffsetMatices), sizeof _float4x4 * pAIMesh->mNumBones);
		outFile.write(reinterpret_cast<char*>(BoneIndices), sizeof _uint * pAIMesh->mNumBones);

		outFile.write(reinterpret_cast<char*>(vBlendIndices), sizeof XMUINT4 * pAIMesh->mNumVertices);
		outFile.write(reinterpret_cast<char*>(vBlendWeights), sizeof XMFLOAT4 * pAIMesh->mNumVertices);

		Safe_Delete_Array(OffsetMatices);
		Safe_Delete_Array(BoneIndices);

		Safe_Delete_Array(vBlendIndices);
		Safe_Delete_Array(vBlendWeights);

		for (size_t siIndex = 0; siIndex < pAIMesh->mNumFaces; ++siIndex)
		{
			aiFace	AIFace = pAIMesh->mFaces[siIndex];

			_uint* iIndices = AIFace.mIndices;
			outFile.write(reinterpret_cast<char*>(iIndices), sizeof _uint * 3);
		}
	}

	/* MATERIAL */
	iStore = m_pAIScene->mNumMaterials;
	outFile.write(reinterpret_cast<char*>(&iStore), sizeof _uint);

	for (size_t iIndex = 0; iIndex < m_pAIScene->mNumMaterials; iIndex++)
	{
		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[iIndex];

		map<_uint, aiString>	TexturePath;
		for (_uint iMaterial = 0; iMaterial < ::AI_TEXTURE_TYPE_MAX; iMaterial++)
		{
			aiString		strTexturePath;

			if (iMaterial == aiTextureType::aiTextureType_LIGHTMAP) /* 라이트 맵 */
			{
				auto iter = TexturePath.find(aiTextureType::aiTextureType_DIFFUSE);
				if (iter == TexturePath.end())
					continue;

				aiString strDiffuse = iter->second;
				vector<string> vecDiffuseSplit = SplitBySeparator(strDiffuse.C_Str(), '_');

				string strLightmapPath = { };
				for (_int iIndex = 0; iIndex < vecDiffuseSplit.size() - 1; ++iIndex)
				{
					strLightmapPath += vecDiffuseSplit[iIndex];
					strLightmapPath += "_";
				}

				strLightmapPath += "Lightmap.png";

				string	strVerifyTag = { };
				_char	szVerifyTag[MAX_PATH] = ("");
				_splitpath_s(strLightmapPath.c_str(), nullptr, 0, nullptr, 0, szVerifyTag, MAX_PATH, nullptr, 0);

				strVerifyTag = szDir;
				strVerifyTag += szVerifyTag;
				strVerifyTag += ".png";

				if (_access(strVerifyTag.c_str(), F_OK) == 0) {
					TexturePath.emplace(iMaterial, strLightmapPath);
				}
			}
			else if (iMaterial == aiTextureType::aiTextureType_EMISSION_COLOR) /* 램프 맵*/
			{
				auto iter = TexturePath.find(aiTextureType::aiTextureType_DIFFUSE);
				if (iter == TexturePath.end())
					continue;

				aiString strDiffuse = iter->second;
				vector<string> vecDiffuseSplit = SplitBySeparator(strDiffuse.C_Str(), '_');

				string strRampmapPath = { };
				for (_int iIndex = 0; iIndex < vecDiffuseSplit.size() - 1; ++iIndex)
				{
					strRampmapPath += vecDiffuseSplit[iIndex];
					strRampmapPath += "_";
				}

				strRampmapPath += "Shadow_Ramp.png";

				string	strVerifyTag = { };
				_char	szVerifyTag[MAX_PATH] = ("");
				_splitpath_s(strRampmapPath.c_str(), nullptr, 0, nullptr, 0, szVerifyTag, MAX_PATH, nullptr, 0);

				strVerifyTag = szDir;
				strVerifyTag += szVerifyTag;
				strVerifyTag += ".png";

				if (_access(strVerifyTag.c_str(), F_OK) == 0) {
					TexturePath.emplace(iMaterial, strRampmapPath);
				}
			}
			else
			{
				if (FAILED(pAIMaterial->GetTexture(aiTextureType(iMaterial), 0, &strTexturePath)))
					continue;

				TexturePath.emplace(iMaterial, strTexturePath);
			}
		}

		_uint iNumTexture = static_cast<_uint>(TexturePath.size());
		outFile.write(reinterpret_cast<char*>(&iNumTexture), sizeof _uint);

		for (auto& Pair : TexturePath)
		{
			outFile.write(reinterpret_cast<const char*>(&Pair.first), sizeof _uint);

			char	szPath[MAX_PATH];	strcpy_s(szPath, Pair.second.C_Str());
			outFile.write(reinterpret_cast<char*>(&szPath), sizeof(char[MAX_PATH]));
		}

		TexturePath.clear();
	}

	/* Animation */
	iStore = m_pAIScene->mNumAnimations;
	outFile.write(reinterpret_cast<char*>(&iStore), sizeof _uint);

	for (size_t iAnimation = 0; iAnimation < m_pAIScene->mNumAnimations; ++iAnimation)
	{
		aiAnimation* pAIAnimation = m_pAIScene->mAnimations[iAnimation];

		char	szName[MAX_PATH];	strcpy_s(szName, pAIAnimation->mName.data);
		outFile.write(reinterpret_cast<char*>(&szName), sizeof(char[MAX_PATH]));

		outFile.write(reinterpret_cast<char*>(&pAIAnimation->mDuration), sizeof _double);
		outFile.write(reinterpret_cast<char*>(&pAIAnimation->mTicksPerSecond), sizeof _double);
		outFile.write(reinterpret_cast<char*>(&pAIAnimation->mNumChannels), sizeof _uint);

		/* Channel */
		for (size_t iChannel = 0; iChannel < pAIAnimation->mNumChannels; ++iChannel)
		{
			char	szName[MAX_PATH];	strcpy_s(szName, pAIAnimation->mChannels[iChannel]->mNodeName.data);
			outFile.write(reinterpret_cast<char*>(&szName), sizeof(char[MAX_PATH]));

			_uint		iChannelBoneIndex = 0;

			auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](BONE_DESC pBone)->_bool {
				if (0 == strcmp(pBone.szName, pAIAnimation->mChannels[iChannel]->mNodeName.data))
					return true;
				++iChannelBoneIndex;
				return false;
				});

			outFile.write(reinterpret_cast<char*>(&iChannelBoneIndex), sizeof _uint);

			_uint iNumKeyFrames = 0;

			iNumKeyFrames = max(pAIAnimation->mChannels[iChannel]->mNumScalingKeys, pAIAnimation->mChannels[iChannel]->mNumRotationKeys);
			iNumKeyFrames = max(iNumKeyFrames, pAIAnimation->mChannels[iChannel]->mNumPositionKeys);
			outFile.write(reinterpret_cast<char*>(&iNumKeyFrames), sizeof _uint);

			KEYFRAME			KeyFrame = {};
			_float4				vScale = {}, vRotation = {}, vTranslation = { 0.0f, 0.f, 0.f, 1.f };

			for (size_t iKeyFrame = 0; iKeyFrame < iNumKeyFrames; ++iKeyFrame)
			{
				if (iKeyFrame < pAIAnimation->mChannels[iChannel]->mNumScalingKeys)
				{
					memcpy(&vScale, &pAIAnimation->mChannels[iChannel]->mScalingKeys[iKeyFrame].mValue, sizeof(_float3));

					KeyFrame.fTime = static_cast<_float>(pAIAnimation->mChannels[iChannel]->mScalingKeys[iKeyFrame].mTime);
				}

				if (iKeyFrame < pAIAnimation->mChannels[iChannel]->mNumRotationKeys)
				{
					vRotation.x = pAIAnimation->mChannels[iChannel]->mRotationKeys[iKeyFrame].mValue.x;
					vRotation.y = pAIAnimation->mChannels[iChannel]->mRotationKeys[iKeyFrame].mValue.y;
					vRotation.z = pAIAnimation->mChannels[iChannel]->mRotationKeys[iKeyFrame].mValue.z;
					vRotation.w = pAIAnimation->mChannels[iChannel]->mRotationKeys[iKeyFrame].mValue.w;

					KeyFrame.fTime = static_cast<_float>(pAIAnimation->mChannels[iChannel]->mRotationKeys[iKeyFrame].mTime);
				}

				if (iKeyFrame < pAIAnimation->mChannels[iChannel]->mNumPositionKeys)
				{
					memcpy(&vTranslation, &pAIAnimation->mChannels[iChannel]->mPositionKeys[iKeyFrame].mValue, sizeof(_float3));

					KeyFrame.fTime = static_cast<_float>(pAIAnimation->mChannels[iChannel]->mPositionKeys[iKeyFrame].mTime);
				}

				outFile.write(reinterpret_cast<char*>(&KeyFrame.fTime), sizeof _float);
				outFile.write(reinterpret_cast<char*>(&vScale), sizeof XMFLOAT4);
				outFile.write(reinterpret_cast<char*>(&vRotation), sizeof XMFLOAT4);
				outFile.write(reinterpret_cast<char*>(&vTranslation), sizeof XMFLOAT4);
			}
		}
	}

	m_Bones.clear();

	outFile.close();

	m_Importer.FreeScene();

	return S_OK;
}

HRESULT CConvert_Manager::Initialize()
{
	return S_OK;
}

CConvert_Manager* CConvert_Manager::Create()
{
	CConvert_Manager* pInstance = new CConvert_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CConvert_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CConvert_Manager::Free()
{
	__super::Free();

	m_Importer.FreeScene();
}
