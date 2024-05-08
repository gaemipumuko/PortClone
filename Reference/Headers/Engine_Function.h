#pragma once

namespace Engine 
{
	template<typename T>
	void Safe_Delete(T& Temp)
	{
		if (Temp)
		{
			delete Temp;
			Temp = nullptr;
		}
	}

	template<typename T>
	void Safe_Delete_Array(T& Temp)
	{
		if (Temp)
		{
			delete[] Temp;
			Temp = nullptr;
		}
	}

	template<typename T>
	unsigned long Safe_AddRef(T& Temp)
	{
		unsigned long dwRefCnt = 0;

		if (nullptr != Temp)
		{
			dwRefCnt = Temp->AddRef();			
		}

		return dwRefCnt;
	}

	template<typename T>
	unsigned long Safe_Release(T& Temp)
	{
		unsigned long dwRefCnt = 0;

		if (Temp)
		{
			dwRefCnt = Temp->Release();
			if(0 == dwRefCnt)
				Temp = nullptr;
		}

		return dwRefCnt;
	}

	class CTag_Finder
	{
	private:
		const wchar_t*	m_pStr;

	public:
		CTag_Finder(const wchar_t* pStr)
			: m_pStr(pStr)
		{

		}

	public:
		template<typename T>
		bool	operator()(T& Pair)
		{
			if (!lstrcmp(m_pStr, Pair.first))
				return true;

			return false;
		}
	};

	inline RECT Get_Rect(float fX, float fY, float fSizeX, float fSizeY, float fWinCX = 0.f, float fWinCY = 0.f)
	{
		RECT tRect = 
		{ 
			(LONG)(fX - fSizeX * 0.5f + (fWinCX / 2)),
			(LONG)(fY - fSizeY * 0.5f + (fWinCY / 2)),
			(LONG)(fX + fSizeX * 0.5f + (fWinCX / 2)),
			(LONG)(fY + fSizeY * 0.5f + (fWinCY / 2))
		};

		return tRect;
	}

	/* wchar to char */
	inline std::string ConvertWCToC(std::wstring strSrc)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::string narrowStr = converter.to_bytes(strSrc);

		return narrowStr;
	}

	/* char to wchar */
	inline std::wstring ConvertCToWC(std::string strSrc)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring wideStrConverted = converter.from_bytes(strSrc);

		return wideStrConverted;
	}

	/* split wstirng */
	inline std::vector<std::wstring> SplitBySeparator(const std::wstring& strInput, wchar_t* chSeparator)
	{
		std::wistringstream	wiss(strInput);

		std::wstring				strToken;
		std::vector<std::wstring>	vecTokens;

		while (getline(wiss, strToken, *chSeparator))
			vecTokens.push_back(strToken);

		return vecTokens;
	}

	/* split stirng */
	inline std::vector<std::string> SplitBySeparator(const std::string& strInput, char chSeparator)
	{
		std::istringstream	wiss(strInput);

		std::string					strToken;
		std::vector<std::string>	vecTokens;

		while (getline(wiss, strToken, chSeparator))
			vecTokens.push_back(strToken);

		return vecTokens;
	}

	/* calculate slope */
	inline float CalculateSlope(const XMFLOAT2& SrcPoint, const XMFLOAT2& DstPoint) 
	{
		if (SrcPoint.x == DstPoint.x)
			return INFINITY;

		return (DstPoint.y - SrcPoint.y) / (DstPoint.x - SrcPoint.x);
	}

	/* calculate quaternion rotation angle */
	inline float CalculateQuaternionRotationAngle(const XMVECTOR& vQuaternion1, const XMVECTOR& vQuaternion2)
	{
		float fDot = XMVectorGetX(XMVector4Dot(vQuaternion1, vQuaternion2));
		float fTheta = 2.f * std::acosf(std::abs(fDot));

		return fTheta;
	}

	/* diff two vector */
	inline float Get_Distance(FXMVECTOR vSrcPos, FXMVECTOR vDstPos) 
	{
		return XMVectorGetX(XMVector3Length(XMVectorSubtract(vSrcPos, vDstPos)));
	}

	/* float lerp */
	inline float Get_fLerp(float fA, float fB, float fRatio)
	{
		return fA * (1.f - fRatio) + (fB * fRatio);
	}
}