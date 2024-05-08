#include "Particle_Manager.h"

#include "GameInstance.h"

CParticle_Manager::CParticle_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

vector<wstring> CParticle_Manager::Get_ParticleTags()
{
	vector<wstring> strParticleTags;

	for (auto& Particle : m_Prototypes)
		strParticleTags.push_back(Particle.first);

	return strParticleTags;
}

PARTICLE_INFO CParticle_Manager::Get_ParticleInfo(const wstring& strPrototypeTag)
{
	auto& Particle = m_Prototypes.find(strPrototypeTag);
	if (Particle == m_Prototypes.end())
		return PARTICLE_INFO();

	return Particle->second->Get_ParticleInfo();
}

HRESULT CParticle_Manager::Prepare_ParticleManager(CTexture* pTexture, CRenderer* pRenderer)
{
	if (pTexture == nullptr || pRenderer == nullptr)
		return E_FAIL;

	if (m_pTextureCom == nullptr && m_pRendererCom == nullptr)
	{
		m_pTextureCom = pTexture;
		m_pRendererCom = pRenderer;
	}
	else
	{
		Safe_Release(pTexture);
		Safe_Release(pRenderer);
	}

	return S_OK;
}

HRESULT CParticle_Manager::Play_Particle(const wstring& strPrototypeTag, const wstring& strParticleTag, _int iShaderPass, _int iParticleIdx, const _float4x4* pParentBone, CTransform* pParentMatrix, _float fDuration ,_bool bFade)
{
	if (m_Prototypes.find(strPrototypeTag) == m_Prototypes.end())
		return E_FAIL;

	if (m_Particles.find(strParticleTag) != m_Particles.end())
		return E_FAIL;

	CParticle::PARTICLE_DESC tParticleDesc = { };
	tParticleDesc.bFade = bFade;
	tParticleDesc.iShaderPass = iShaderPass;
	tParticleDesc.iParticleIdx = iParticleIdx;
	tParticleDesc.fDuration = fDuration;
	tParticleDesc.pParentBoneMatrix = pParentBone;
	tParticleDesc.pParentWorldMatrix = pParentMatrix;
	tParticleDesc.pShaderCom = m_pShaderCom;
	tParticleDesc.pTextureCom = m_pTextureCom;
	tParticleDesc.pVIBufferCom = m_pVIBufferCom;

	CParticle* pParticle = m_Prototypes.find(strPrototypeTag)->second->Clone(&tParticleDesc);
	if (pParticle == nullptr)
		return E_FAIL;

	m_Particles.insert({ strParticleTag, pParticle });

	return S_OK;
}

HRESULT CParticle_Manager::Pause_Particle(const wstring& strParticleTag, _bool bPause)
{
	auto& Particle = m_Particles.find(strParticleTag);
	if (Particle == m_Particles.end())
		return E_FAIL;

	Particle->second->Set_Pause(bPause);

	return S_OK;
}

HRESULT CParticle_Manager::Stop_Particle(const wstring& strParticleTag, _float fDelay)
{
	auto& Particle = m_Particles.find(strParticleTag);
	if (Particle == m_Particles.end())
		return E_FAIL;

	if (fDelay == -1.f)
	{
		Safe_Release(Particle->second);
		m_Particles.erase(Particle);
	}
	else
	{
		Particle->second->Get_ParticleInfo().iParticleEmitRate = -1;
		Particle->second->Get_ParticleInfo().fParticleCreateTime = 99.f;
		Particle->second->Set_Delay(fDelay);
		Particle->second->Set_DelayStop(TRUE);
	}

	return S_OK;
}

HRESULT CParticle_Manager::Create_ParticlePrototype(const wstring& strParticleTag, PARTICLE_INFO tValue)
{
	if (m_Prototypes.find(strParticleTag) != m_Prototypes.end())
		return E_FAIL;

	CParticle* pParticle = CParticle::Create(tValue);
	if (pParticle == nullptr)
		return E_FAIL;

	m_Prototypes.insert({ strParticleTag, pParticle });

	return S_OK;
}

HRESULT CParticle_Manager::Delete_ParticlePrototype(const wstring& strParticleTag)
{
	auto& Pair = m_Prototypes.find(strParticleTag);
	if (Pair == m_Prototypes.end())
		return E_FAIL;

	Safe_Release(Pair->second);
	m_Prototypes.erase(Pair);

	return S_OK;
}

HRESULT CParticle_Manager::Save_Particles(const wstring& strFilePath)
{
	ofstream outFile(strFilePath.c_str(), ios::binary | ios::out);
	if (outFile.is_open() == FALSE) {
		return E_FAIL;
	}

	_int iStore = static_cast<_int>(m_Prototypes.size());
	outFile.write(reinterpret_cast<char*>(&iStore), sizeof _int);
	for (auto& Particle : m_Prototypes)
	{
		char	szName[MAX_PATH];	strcpy_s(szName, ConvertWCToC(Particle.first).c_str());
		outFile.write(reinterpret_cast<char*>(&szName), sizeof(char[MAX_PATH]));

		PARTICLE_INFO tParticleInfo = Particle.second->Get_ParticleInfo();

		outFile.write(reinterpret_cast<char*>(&tParticleInfo.bFade), sizeof _bool);
		outFile.write(reinterpret_cast<char*>(&tParticleInfo.iMaxParticle), sizeof _int);

		outFile.write(reinterpret_cast<char*>(&tParticleInfo.bFrameAnimation), sizeof _bool);
		outFile.write(reinterpret_cast<char*>(&tParticleInfo.fFrameAnimCycle), sizeof _float);
		outFile.write(reinterpret_cast<char*>(&tParticleInfo.fAngle), sizeof _float);

		outFile.write(reinterpret_cast<char*>(&tParticleInfo.iNumParticleRowCol), sizeof XMINT2);
		outFile.write(reinterpret_cast<char*>(&tParticleInfo.iCurrParticleRowCol), sizeof XMINT2);

		outFile.write(reinterpret_cast<char*>(&tParticleInfo.iParticleEmitRate), sizeof _int);
		outFile.write(reinterpret_cast<char*>(&tParticleInfo.fParticleCreateTime), sizeof _float);

		outFile.write(reinterpret_cast<char*>(&tParticleInfo.vParticleColor), sizeof _vector);

		outFile.write(reinterpret_cast<char*>(&tParticleInfo.bTrail), sizeof _bool);
		outFile.write(reinterpret_cast<char*>(&tParticleInfo.eType), sizeof PARTICLE_INFO::PARTICLE_TYPE);
#pragma region Add Option
		outFile.write(reinterpret_cast<char*>(&tParticleInfo.iOption), sizeof _uint);

		outFile.write(reinterpret_cast<char*>(&tParticleInfo.fGravAcc), sizeof _float);
		outFile.write(reinterpret_cast<char*>(&tParticleInfo.vRotationXYZ), sizeof XMFLOAT3);
		outFile.write(reinterpret_cast<char*>(&tParticleInfo.fAccel), sizeof _float);
#pragma endregion

		outFile.write(reinterpret_cast<char*>(&tParticleInfo.vParticlePosition), sizeof XMFLOAT4);
		outFile.write(reinterpret_cast<char*>(&tParticleInfo.vCorrectionVector), sizeof XMFLOAT4);

		outFile.write(reinterpret_cast<char*>(&tParticleInfo.vTrailPosition), sizeof XMFLOAT3);
		outFile.write(reinterpret_cast<char*>(&tParticleInfo.vDirectionMin), sizeof XMFLOAT3);
		outFile.write(reinterpret_cast<char*>(&tParticleInfo.vDirectionMax), sizeof XMFLOAT3);

		outFile.write(reinterpret_cast<char*>(&tParticleInfo.vParticleSize), sizeof XMFLOAT2);
		outFile.write(reinterpret_cast<char*>(&tParticleInfo.vParticleSpeed), sizeof XMFLOAT2);
		outFile.write(reinterpret_cast<char*>(&tParticleInfo.vParticleLifeTime), sizeof XMFLOAT2);
	}

	outFile.close();

	return S_OK;
}

HRESULT CParticle_Manager::Load_Particles(const wstring& strFilePath)
{
	for (auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);
	m_Prototypes.clear();

	ifstream inFile(strFilePath.c_str(), ios::binary | ios::in);
	if (inFile.is_open() == FALSE) {
		return E_FAIL;
	}

	/* AreaÀÇ °³¼ö */
	_int iNumParticle = { };
	inFile.read(reinterpret_cast<char*>(&iNumParticle), sizeof _int);

	for (_int iParticleIndex = 0; iParticleIndex < iNumParticle; ++iParticleIndex)
	{
		char	szName[MAX_PATH];
		inFile.read(reinterpret_cast<char*>(&szName), sizeof(char[MAX_PATH]));

		PARTICLE_INFO tParticleInfo = { };

		inFile.read(reinterpret_cast<char*>(&tParticleInfo.bFade), sizeof _bool);
		inFile.read(reinterpret_cast<char*>(&tParticleInfo.iMaxParticle), sizeof _int);

		inFile.read(reinterpret_cast<char*>(&tParticleInfo.bFrameAnimation), sizeof _bool);
		inFile.read(reinterpret_cast<char*>(&tParticleInfo.fFrameAnimCycle), sizeof _float);
		inFile.read(reinterpret_cast<char*>(&tParticleInfo.fAngle), sizeof _float);

		inFile.read(reinterpret_cast<char*>(&tParticleInfo.iNumParticleRowCol), sizeof XMINT2);
		inFile.read(reinterpret_cast<char*>(&tParticleInfo.iCurrParticleRowCol), sizeof XMINT2);

		inFile.read(reinterpret_cast<char*>(&tParticleInfo.iParticleEmitRate), sizeof _int);
		inFile.read(reinterpret_cast<char*>(&tParticleInfo.fParticleCreateTime), sizeof _float);

		inFile.read(reinterpret_cast<char*>(&tParticleInfo.vParticleColor), sizeof _vector);

		inFile.read(reinterpret_cast<char*>(&tParticleInfo.bTrail), sizeof _bool);
		inFile.read(reinterpret_cast<char*>(&tParticleInfo.eType), sizeof PARTICLE_INFO::PARTICLE_TYPE);
#pragma region Add Option
		inFile.read(reinterpret_cast<char*>(&tParticleInfo.iOption), sizeof _uint);

		inFile.read(reinterpret_cast<char*>(&tParticleInfo.fGravAcc), sizeof _float);
		inFile.read(reinterpret_cast<char*>(&tParticleInfo.vRotationXYZ), sizeof XMFLOAT3);
		inFile.read(reinterpret_cast<char*>(&tParticleInfo.fAccel), sizeof _float);

#pragma endregion

		inFile.read(reinterpret_cast<char*>(&tParticleInfo.vParticlePosition), sizeof XMFLOAT4);
		inFile.read(reinterpret_cast<char*>(&tParticleInfo.vCorrectionVector), sizeof XMFLOAT4);

		inFile.read(reinterpret_cast<char*>(&tParticleInfo.vTrailPosition), sizeof XMFLOAT3);
		inFile.read(reinterpret_cast<char*>(&tParticleInfo.vDirectionMin), sizeof XMFLOAT3);
		inFile.read(reinterpret_cast<char*>(&tParticleInfo.vDirectionMax), sizeof XMFLOAT3);

		inFile.read(reinterpret_cast<char*>(&tParticleInfo.vParticleSize), sizeof XMFLOAT2);
		inFile.read(reinterpret_cast<char*>(&tParticleInfo.vParticleSpeed), sizeof XMFLOAT2);
		inFile.read(reinterpret_cast<char*>(&tParticleInfo.vParticleLifeTime), sizeof XMFLOAT2);

		Create_ParticlePrototype(ConvertCToWC(szName), tParticleInfo);
	}

	inFile.close();

	return S_OK;
}

HRESULT CParticle_Manager::Initialize()
{
	/* Shader Prototype */
	m_pShaderCom = CShader::Create(m_pDevice, m_pContext, VTXPOINT_INSTANCE::VertexElements, VTXPOINT_INSTANCE::iNumElements,
		TEXT("../../Reference/ShaderFiles/Shader_Particle.hlsl"));
	if (m_pShaderCom == nullptr)
		return E_FAIL;

	/* VIBuffer Prototype */
	m_pVIBufferCom = CVIBuffer_Particle_Instancing::Create(m_pDevice, m_pContext, MAX_PARTICLE);
	if (m_pVIBufferCom == nullptr)
		return E_FAIL;

	return S_OK;
}

HRESULT CParticle_Manager::Tick(_float fTimeDelta)
{
	for (auto& Pair = m_Particles.begin(); Pair != m_Particles.end();)
	{
		if (((Pair->second->Get_DelayStop() == TRUE) && (Pair->second->Get_Delay() < 0.f)) || (Pair->second->Get_Duration() == 0.f))
		{
			Safe_Release(Pair->second);
			Pair = m_Particles.erase(Pair);
		}
		else
			++Pair;
	}

	for (auto& Pair : m_Particles)
		Pair.second->Invalidate_Instance(fTimeDelta);

	return S_OK;
}

HRESULT CParticle_Manager::LateTick(_float fTimeDelta)
{
	for (auto& Particle : m_Particles)
	{
		if (Particle.second->Get_Pause() == TRUE)
			continue;

		m_pRendererCom->Add_RenderParticle(Particle.second);
	}

	return S_OK;
}

CParticle_Manager* CParticle_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticle_Manager* pInstance = new CParticle_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CParticle_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticle_Manager::Clear()
{
	for (auto& Pair : m_Particles)
		Safe_Release(Pair.second);
	m_Particles.clear();
}

void CParticle_Manager::Free()
{
	__super::Free();

	for (auto& Pair : m_Particles)
		Safe_Release(Pair.second);
	m_Particles.clear();

	for (auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);
	m_Prototypes.clear();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
