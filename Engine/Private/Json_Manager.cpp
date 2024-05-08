#include "Json_Manager.h"

CJson_Manager::CJson_Manager()
{
}

HRESULT CJson_Manager::Load_JsonFile(const wstring& strJsonTag, const string& strJsonFilePath)
{
	Value root;
	ifstream inFile(strJsonFilePath, std::ifstream::binary);
	if (inFile.is_open() == FALSE) {
		return E_FAIL;
	}

	inFile >> root;
	m_Jsons.emplace(strJsonTag, root);

	inFile.close();

	return S_OK;
}

_int CJson_Manager::Find_JsonIntValue(const wstring& strJsonTag, vector<string> vFindTags)
{
	_int iReturn = { };

	auto& iterJson = m_Jsons.find(strJsonTag);
	if (iterJson == m_Jsons.end())
		return 0;

	reverse(vFindTags.begin(), vFindTags.end());

	Value findValue = iterJson->second[vFindTags.back().c_str()];
	vFindTags.pop_back();

	while (vFindTags.size() > 0)
	{
		findValue = findValue[vFindTags.back().c_str()];
		vFindTags.pop_back();
	}

	if (findValue.isInt())
		iReturn = findValue.asInt();
	else
		iReturn = INT_MIN;

	return iReturn;
}
_float CJson_Manager::Find_JsonFloatValue(const wstring& strJsonTag, vector<string> vFindTags)
{
	_float fReturn = { };

	auto& iterJson = m_Jsons.find(strJsonTag);
	if (iterJson == m_Jsons.end())
		return 0.f;

	reverse(vFindTags.begin(), vFindTags.end());

	Value findValue = iterJson->second[vFindTags.back().c_str()];
	vFindTags.pop_back();

	while (vFindTags.size() > 0)
	{
		findValue = findValue[vFindTags.back().c_str()];
		vFindTags.pop_back();
	}

	if (findValue.isDouble())
		fReturn = findValue.asFloat();
	else
		fReturn = INFINITY;

	return fReturn;
}
wstring CJson_Manager::Find_JsonStringValue(const wstring& strJsonTag, vector<string> vFindTags)
{
	auto& iterJson = m_Jsons.find(strJsonTag);
	if (iterJson == m_Jsons.end())
		return L"";

	reverse(vFindTags.begin(), vFindTags.end());

	Value findValue = iterJson->second[vFindTags.back().c_str()];
	vFindTags.pop_back();

	while (vFindTags.size() > 0)
	{
		findValue = findValue[vFindTags.back().c_str()];
		vFindTags.pop_back();
	}

	if (findValue.isNull())
		return L"";

	wstring_convert<codecvt_utf8<wchar_t>, wchar_t> convertString;

	string strJson = findValue.asCString();
	wstring strWideJson = convertString.from_bytes(strJson);

	return strWideJson;
}

list<_int> CJson_Manager::Find_JsonIntArrayValues(const wstring& strJsonTag, vector<string> vFindTags)
{
	list<_int> IntArrayValues;

	auto& iterJson = m_Jsons.find(strJsonTag);
	if (iterJson == m_Jsons.end())
		return list<_int>();

	reverse(vFindTags.begin(), vFindTags.end());

	Value findValue = iterJson->second[vFindTags.back().c_str()];
	vFindTags.pop_back();

	while (vFindTags.size() > 0)
	{
		findValue = findValue[vFindTags.back().c_str()];
		vFindTags.pop_back();
	}

	_int iSize = findValue.size();
	for (_int iIndex = 0; iIndex < iSize; ++iIndex)
	{
		if (findValue[iIndex].isInt())
			IntArrayValues.push_back(findValue[iIndex].asInt());
	}

	return IntArrayValues;
}

list<wstring> CJson_Manager::Find_JsonStringArrayValues(const wstring& strJsonTag, vector<string> vFindTags)
{
	list<wstring> StringArrayValues;

	auto& iterJson = m_Jsons.find(strJsonTag);
	if (iterJson == m_Jsons.end())
		return list<wstring>();

	reverse(vFindTags.begin(), vFindTags.end());

	Value findValue = iterJson->second[vFindTags.back().c_str()];
	vFindTags.pop_back();

	while (vFindTags.size() > 0)
	{
		findValue = findValue[vFindTags.back().c_str()];
		vFindTags.pop_back();
	}

	_int iSize = findValue.size();
	for (_int iIndex = 0; iIndex < iSize; ++iIndex)
	{
		wstring_convert<codecvt_utf8<wchar_t>, wchar_t> convertString;

		string strJson = findValue[iIndex].asCString();
		wstring strWideJson = convertString.from_bytes(strJson);

		StringArrayValues.push_back(strWideJson);
	}

	return StringArrayValues;
}

HRESULT CJson_Manager::Initialize()
{
	return S_OK;
}

CJson_Manager* CJson_Manager::Create()
{
	CJson_Manager* pInstance = new CJson_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CJson_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJson_Manager::Free()
{
	__super::Free();
}