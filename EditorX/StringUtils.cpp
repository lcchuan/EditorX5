#include "StdAfx.h"
#include "StringUtils.h"

int StringUtils::split(const T_STRING str,const T_STRING separator,std::vector<T_STRING>& arr_str)
{
	arr_str.clear();

	const int str_len = (int)str.size();
	const int separator_len = (int)separator.size();
	
	int first(0);
	int extract_count = (int)str.find(separator);
	while (extract_count > -1)
	{
		arr_str.push_back(str.substr(first,extract_count));	

		first += extract_count+separator_len;
		if (first >= str_len) {
			break;
		}
		extract_count = (int)str.find(separator,first);
		extract_count -= first;
	}
	arr_str.push_back(str.substr(first,extract_count));
	return (int)arr_str.size();
}

StringUtils::T_STRING StringUtils::format(const StringUtils::T_CHAR* format,...)
{
	T_STRING str;
  
    va_list marker = NULL;  
    va_start(marker, format);
#ifdef _UNICODE
	size_t num_of_chars = _vscwprintf(format, marker); 
#else
	size_t num_of_chars = _vscprintf(format, marker); 
#endif 
	str.resize(num_of_chars);
#ifdef _UNICODE
	vswprintf_s(const_cast<wchar_t*>(str.data()), num_of_chars+1, format, marker); 
#else
	vsprintf_s(const_cast<char*>(str.data()), num_of_chars+1, format, marker); 
#endif  
    va_end(marker);

    return str;  
}

StringUtils::T_STRING StringUtils::rtrim(const StringUtils::T_STRING& str,const StringUtils::T_STRING& sub)
{
	T_STRING dest = str;
	if (sub.length() > 0) {
		dest.erase(dest.find_last_not_of(sub.c_str()) + 1);  
	}
	return dest;
}

StringUtils::T_STRING StringUtils::rtrim(const StringUtils::T_STRING& str,const StringUtils::T_CHAR& sub/*=0*/)
{
	if (str.length() < 1) {
		return str;
	}
	T_STRING dest = str;
	if (sub != 0) {
		dest.erase(dest.find_last_not_of(sub) + 1);  
	} else {
		bool contain_whitespace = false;
		int i = (int)(dest.length()-1);
		bool b = is_whitespace(dest[i]);
		while (i>-1 && is_whitespace(dest[i])) {
			contain_whitespace = true;
			i--;
		}
		if (contain_whitespace) {
			dest.erase(i+1);
		}
	}
	return dest;
}

StringUtils::T_STRING StringUtils::ltrim(const StringUtils::T_STRING& str,const StringUtils::T_STRING& sub)
{
	T_STRING dest = str;
	if (sub.length() > 0) {
		dest.erase(0,dest.find_first_not_of(sub.c_str()));  
	}
	return dest;
}

StringUtils::T_STRING StringUtils::ltrim(const StringUtils::T_STRING& str,const StringUtils::T_CHAR& sub/*=0*/)
{
	if (str.length() < 1) {
		return str;
	}

	T_STRING dest = str;
	if (sub != 0) {
		dest.erase(0,dest.find_first_not_of(sub));  
	} else {
		bool contain_whitespace = false;
		size_t i = 0;
		while (i<dest.length() && is_whitespace(dest[i])) {
			contain_whitespace = true;
			i++;
		}
		if (contain_whitespace) {
			dest.erase(0,i);
		}
	}
	return dest;
}

StringUtils::T_STRING StringUtils::to_lowercase(StringUtils::T_STRING& str)
{
	T_CHAR* pStr = const_cast<T_CHAR*>(str.data());
	for (int i=0; i<str.length(); i++) {
		if ('A' <= pStr[i] && pStr[i] <= 'Z') {
			pStr[i] += 32;
		}
	}
	return str;
}

StringUtils::T_STRING StringUtils::to_uppercase(StringUtils::T_STRING& str)
{
	T_CHAR* pStr = const_cast<T_CHAR*>(str.data());
	for (int i=0; i<str.length(); i++) {
		if ('a' <= pStr[i] && pStr[i] <= 'z') {
			pStr[i] -= 32;
		}
	}
	return str;
}

int StringUtils::replace(T_STRING& strSrc,const T_CHAR& oldC,const T_CHAR& newC)
{
	int replace_count(0);
	size_t pos = (0);
	while ((pos=strSrc.find(oldC,pos)) != std::string::npos) {
		strSrc.replace(pos++,1,1,newC);
		replace_count++;
	}
	return replace_count;
}

int StringUtils::replace(T_STRING& strSrc,const T_STRING& oldC,const T_STRING& newC)
{
	const size_t oldC_len = oldC.length();
	const size_t newC_len = newC.length();
	size_t pos = 0;
	int replace_count(0);
	while ((pos=strSrc.find(oldC,pos)) != std::string::npos) {
		strSrc.replace(pos,oldC_len,newC);
		pos += newC_len;
		replace_count++;
	}
	return replace_count;
}

int StringUtils::GetBOMBytes(LPCTSTR charset,unsigned char* pBomBytes)
{
#ifdef _UNICODE
#define t_stricmp _wcsicmp
#else
#define t_stricmp _stricmp
#endif  
	if (t_stricmp(charset,TEXT("utf8")) == 0 || t_stricmp(charset,TEXT("utf-8")) == 0) {
		pBomBytes[0] = 0XEF;
		pBomBytes[1] = 0XBB;
		pBomBytes[2] = 0XBF;
		return 3;
	} else if (t_stricmp(charset,TEXT("utf16be")) == 0 || t_stricmp(charset,TEXT("utf-16be")) == 0) {
		pBomBytes[0] = 0XFE;
		pBomBytes[1] = 0XFF;
		return 2;
	} else if (t_stricmp(charset,TEXT("utf16le")) == 0 || t_stricmp(charset,TEXT("utf-16le")) == 0) {
		pBomBytes[0] = 0XFF;
		pBomBytes[1] = 0XFE;
		return 2;
	} else {
		return 0;
	}
}

//@return 0-无法正确判断字符编码；2-UTF8; 3-UTF16LE; 4-UTF16BE
//        如果小于0，则表示携带BOM头，abs()之后取得的正整数的含义参考上面的注释
int StringUtils::JudgeCharset(const char* str,const DWORD& size) {
	unsigned char p_bombytes[4];
	int result(0);

	//先通过bom判断
	if (size > 3) {
		result = GetBOMBytes(TEXT("utf-8"),p_bombytes);
		if (result > 0 && ::memcmp(str,p_bombytes,result) == 0) {
			return -2;
		}
	}
	if (size > 2) {
		result = GetBOMBytes(TEXT("utf-16le"),p_bombytes);
		if (result > 0 && ::memcmp(str,p_bombytes,result) == 0) {
			return -3;
		}
		result = GetBOMBytes(TEXT("utf-16be"),p_bombytes);
		if (result > 0 && ::memcmp(str,p_bombytes,result) == 0) {
			return -4;
		}
	}

	/* 判断是否为utf-8
	 * UTF8 coding:
     * 1 bytes: 0xxxxxxx
     * 2 bytes: 110xxxxx 10xxxxxx
     * 3 bytes: 1110xxxx 10xxxxxx 10xxxxxx
     * 4 bytes: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
	 */
	bool match = false;
	unsigned char* pTemp = (unsigned char*)str;
	const unsigned char* pEnd = (unsigned char*)str+size;
	while (pTemp < pEnd) {
		if (*pTemp < 0x80) {
			++pTemp;
		} else if (*pTemp < 0xC0) {
			match = false;
			break;
		} else if (*pTemp < 0xE0) {
			if ((pTemp >= pEnd - 1) || ((pTemp[1] & 0xC0) != 0x80)) {
				match = false;
				break;
			}
			match = true;
			pTemp += 2;
		} else if (*pTemp < 0xF0) {
			if ((pTemp >= pEnd - 2) || ((pTemp[1] & 0xC0) != 0x80) || ((pTemp[2] & 0xC0) != 0x80)) {
				match = false;
				break;
			}
			match = true;
			pTemp += 3;
		} else {
			match = false;
			break;
		}
	}
	if (match) {
		return 2;
	}

	//判断是否为utf16
	DWORD nNullCount(0),nNullCount_BIGENDIAN(0),err_count(0);
	pTemp = (unsigned char*)str;
	for (DWORD i=0; i<size-1; i+=2) {
		if ((pTemp[i] != 0) && (pTemp[i+1] == 0))
			nNullCount++;
		else if ((pTemp[i] == 0) && (pTemp[i+1] != 0))
			nNullCount_BIGENDIAN++;
		else
			err_count++;

		if (nNullCount > 2)
			return 3;
		else if (nNullCount_BIGENDIAN > 2)
			return 4;
		else if (err_count > 2)
			break;
	}

	return 0;
}

std::string StringUtils::UnicodeToUTF8(const wchar_t* pText)
{
	std::string dest;
	if (*pText != 0) {
		const int size = WideCharToMultiByte(CP_UTF8, 0, pText, -1, 0, 0, 0, 0);
		dest.resize(size-1);
		WideCharToMultiByte(CP_UTF8, 0, pText, -1, const_cast<LPSTR>(dest.data()), size, 0, 0);
	}
	return dest;
}

std::wstring StringUtils::UTF8ToUnicode(const char *pText) {
	std::wstring dest;
	if (*pText != 0) {
		int size = MultiByteToWideChar(CP_UTF8, 0, pText, -1, 0, 0);
		dest.resize(size-1);
		MultiByteToWideChar(CP_UTF8, 0, pText, -1, const_cast<LPWSTR>(dest.data()), size);
	}
	return dest;
}

/* Scintilla 代码 C:\D\code\VS2010\EditorX5\scintilla373\src\UniConversion.cxx
size_t UTF16FromUTF8(const char *s, size_t len, wchar_t *tbuf, size_t tlen) {
	size_t ui = 0;
	const unsigned char *us = reinterpret_cast<const unsigned char *>(s);
	size_t i = 0;
	while ((i<len) && (ui<tlen)) {
		unsigned char ch = us[i++];
		if (ch < 0x80) {
			tbuf[ui] = ch;
		} else if (ch < 0x80 + 0x40 + 0x20) {
			tbuf[ui] = static_cast<wchar_t>((ch & 0x1F) << 6);
			ch = us[i++];
			tbuf[ui] = static_cast<wchar_t>(tbuf[ui] + (ch & 0x7F));
		} else if (ch < 0x80 + 0x40 + 0x20 + 0x10) {
			tbuf[ui] = static_cast<wchar_t>((ch & 0xF) << 12);
			ch = us[i++];
			tbuf[ui] = static_cast<wchar_t>(tbuf[ui] + ((ch & 0x7F) << 6));
			ch = us[i++];
			tbuf[ui] = static_cast<wchar_t>(tbuf[ui] + (ch & 0x7F));
		} else {
			// Outside the BMP so need two surrogates
			int val = (ch & 0x7) << 18;
			ch = us[i++];
			val += (ch & 0x3F) << 12;
			ch = us[i++];
			val += (ch & 0x3F) << 6;
			ch = us[i++];
			val += (ch & 0x3F);
			tbuf[ui] = static_cast<wchar_t>(((val - 0x10000) >> 10) + SURROGATE_LEAD_FIRST);
			ui++;
			tbuf[ui] = static_cast<wchar_t>((val & 0x3ff) + SURROGATE_TRAIL_FIRST);
		}
		ui++;
	}
	return ui;
}
*/

std::wstring StringUtils::ANSIToUnicode(const char* pText)
{
	std::wstring dest;
	if (*pText != 0) {
		int size = MultiByteToWideChar(CP_ACP,0,pText,-1,0,0);
		dest.resize(size-1);
		MultiByteToWideChar(CP_ACP,0,pText,-1,const_cast<LPWSTR>(dest.data()),size);
	}
	return dest;
}

std::string StringUtils::UnicodeToANSI(const wchar_t* pText)
{
	std::string dest;
	if (*pText != 0) {
		int size = WideCharToMultiByte(CP_ACP, 0, pText, -1, 0, 0, 0, 0);
		dest.resize(size-1);
		WideCharToMultiByte(CP_ACP, 0, pText, -1, const_cast<LPSTR>(dest.data()), size, 0, 0);
	}
	return dest;
}

std::string StringUtils::ANSIToUTF8(const char* pText)
{
	std::string dest;
	if (*pText != 0) {
		std::wstring text_u = ANSIToUnicode(pText);
		dest = UnicodeToUTF8(text_u.c_str());
	}
	return dest;
}

std::string StringUtils::UTF8ToANSI(const char* pText)
{
	std::string dest;
	if (*pText != 0) {
		std::wstring text_u = UTF8ToUnicode(pText);
		dest = UnicodeToANSI(text_u.c_str());
	}
	return dest;
}