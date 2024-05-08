#pragma once

#define PURE = 0
#define EPSILON 0.01f
#define EPSILON_VECTOR XMVECTOR{ EPSILON, EPSILON, EPSILON, EPSILON }

#define MAX_PARTICLE 4096

#define AI_TEXTURE_TYPE_MAX  aiTextureType_UNKNOWN

#define CLAMP(VALUE, MIN, MAX) min(max(VALUE, MIN), MAX)

#define BEGIN(NAME) namespace NAME {
#define END			}

#ifdef ENGINE_EXPORTS
#define ENGINE_DLL _declspec(dllexport)
#else
#define ENGINE_DLL _declspec(dllimport)
#endif

#define MSG_BOX(message)								\
::MessageBox(nullptr, message, L"error", MB_OK)

#define NO_COPY(ClassName)								\
ClassName(const ClassName&) = delete;					\
ClassName& operator=(const ClassName&) = delete;

#define DECLARE_SINGLETON(ClassName)					\
		NO_COPY(ClassName)								\
public :												\
	static ClassName* Get_Instance();					\
	static unsigned long Destroy_Instance();			\
private:												\
	static ClassName* m_pInstance;

#define IMPLEMENT_SINGLETON(ClassName)					\
ClassName* ClassName::m_pInstance = nullptr;			\
ClassName* ClassName::Get_Instance()					\
{														\
	if (nullptr == m_pInstance)							\
		m_pInstance = new ClassName;					\
	return m_pInstance;									\
}														\
unsigned long ClassName::Destroy_Instance()				\
{														\
	unsigned long	dwRefCnt = 0;						\
	if(nullptr != m_pInstance)							\
	{													\
		dwRefCnt = m_pInstance->Release();				\
		if(0 == dwRefCnt)								\
			m_pInstance = nullptr;						\
	}													\
	return dwRefCnt;									\
}

#define IMPLEMENT_CREATE_SINGLETON(ClassName)			\
ClassName* ClassName::m_pInstance = nullptr;			\
ClassName* ClassName::Get_Instance()					\
{														\
	if (nullptr == m_pInstance)							\
	{													\
		m_pInstance = ClassName::Create();				\
	}													\
	return m_pInstance;									\
}														\
unsigned long ClassName::Destroy_Instance()				\
{														\
	unsigned long	dwRefCnt = 0;						\
	if(nullptr != m_pInstance)							\
	{													\
		dwRefCnt = m_pInstance->Release();				\
		if(0 == dwRefCnt)								\
			m_pInstance = nullptr;						\
	}													\
	return dwRefCnt;									\
}

#define GET_INSTANCE(CLASSNAME) []() {					\
	CLASSNAME* pInstance = CLASSNAME::Get_Instance();	\
	pInstance->AddRef();								\
	return pInstance;									\
}()														\

#define RELEASE_INSTANCE(CLASSNAME) []() {				\
	CLASSNAME* pInstance = CLASSNAME::Get_Instance();	\
	pInstance->Release();								\
}()														\

