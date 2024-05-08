#pragma once

#include "Binary_Defines.h"
#include "Base.h"

BEGIN(Binary)

class CConvert_Manager final : public CBase
{
private:
	CConvert_Manager();
	virtual ~CConvert_Manager() = default;

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	Assimp::Importer		m_Importer;
	const aiScene*			m_pAIScene = { nullptr };

private:
	vector<BONE_DESC>		m_Bones;

private: /* For. Pivot */
	_float4	m_vPivotScale = { 1.f, 1.f, 1.f, 1.f };
	_float4	m_vPivotRotation = { 0.f, 0.f, 0.f, 0.f };
	_float4	m_vPivotTranslation = { 0.f, 0.f, 0.f, 1.f };

private:
	HRESULT Convert_Bones(aiNode* pNode, _int iParentBoneIndex);

public:
	HRESULT	Convert_Anim(const wstring& strModelFolderPath);
	HRESULT	Convert_NonAnim(const wstring& strModelFolderPath);
	HRESULT	Convert_Anim_SPToonShader(const wstring& strModelFolderPath);

	HRESULT Convert_AnimModel(const string& strModelFilePath, _fmatrix PivotMatrix = XMMatrixIdentity());
	HRESULT Convert_NonAnimModel(const string& strModelFilePath, _fmatrix PivotMatrix = XMMatrixIdentity());
	HRESULT Convert_AnimModel_SPToonShader(const string& strModelFilePath, _fmatrix PivotMatrix = XMMatrixIdentity());

public:
	HRESULT Initialize();

public:
	static CConvert_Manager*	Create();
	virtual void				Free();
};

END