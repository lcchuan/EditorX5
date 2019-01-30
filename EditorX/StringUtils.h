#pragma once

/**
 * 字符串帮助类 StringUtils
 * 字符串拼接类 StringBuilder
 * 20180105 lcc 创建
 */
#include<string>
#include<vector>
#include<list>

/**
 * 字符串帮助类
 */
class StringUtils
{
public:
#ifdef _UNICODE
	typedef wchar_t T_CHAR;
	typedef std::wstring T_STRING;
#else
	typedef char T_CHAR;
	typedef std::string T_STRING;
#endif

public:
	/**
	 * 将一个字符串拆分为多个子字符串，然后将结果作为字符串数组输出
	 * param : str[in] - 待分割的字符串
			   seprator  [in] - 分隔符
			   arr_str   [out]- 分割后的子字符串数组
	 * return : 返回arrString的元素个数，至少为1
	 */
	static int split(const T_STRING str,const T_STRING separator,std::vector<T_STRING>& arr_str);

	/**
	 * 字符串中的内容替换
	 * @param strSrc[in][out] 原字符串,函数执行之后输出替换后的新字符串
	 * @param oldC[in] 欲被替换的子字符串
	 * @param newC[in] 欲替换的新的子字符串
	 * @return 返回替换的数量
	 */
	static int replace(T_STRING& strSrc,const T_STRING& oldC,const T_STRING& newC);
	static int replace(T_STRING& strSrc,const T_CHAR& oldC,const T_CHAR& newC);

	/**
	 * 格式化字符串，转移字符的定义与printf相同
	 */
	static T_STRING format(const T_CHAR* format,...);

	/*
	 * @param sub 如果为空字符串，则表示删除空白字符（空格、TAB等）
	 */
	static T_STRING rtrim(const T_STRING& str,const T_STRING& sub);
	static T_STRING rtrim(const T_STRING& str,const T_CHAR& sub=0);
	static T_STRING ltrim(const T_STRING& str,const T_STRING& sub);
	static T_STRING ltrim(const T_STRING& str,const T_CHAR& sub=0);

	static T_STRING to_lowercase(T_STRING& str);
	static T_STRING to_uppercase(T_STRING& str);

	//判断是否为空白符
	static inline bool is_whitespace(const T_CHAR& c) {
		//160-&nbsp; 0x3000-unicode的空格编码; 0xC2A0-utf8的空格编码
		return (' ' == c || '\t' == c || 160 == c || 0x3000 == c);
	}

	/**
	 * 获得字符集对应BOM字节
	 * @param charset[in]     字符集名称
	 * @param pBomBytes[out]  用于输出该字符集对应的bom,建议数组的大小大于或等于4
	 * @return bom的字节数，如果为0，则表示未找到该字符集对应的bom
	 */
	static int GetBOMBytes(LPCTSTR charset,unsigned char* pBomBytes);

	/**
	 * 判断字符串的编码
	 * @param str 欲判断的字符串
	 * @param size 该字符串的大小，以字节为单位，不包括终止符
	 * @return 0-无法正确判断字符编码；
	 *         2-UTF8; 3-UTF16LE; 4-UTF16BE；
	 *         如果小于0，则表示携带BOM头，abs()之后取得的正整数的含义参考上面的注释
	 */
	static int JudgeCharset(const char* str,const DWORD& size);

	//字符编码转换
	static std::string UnicodeToUTF8(const wchar_t* pText);
	static std::string UnicodeToANSI(const wchar_t* pText);
	static std::wstring UTF8ToUnicode(const char* pText);
	static std::string UTF8ToANSI(const char* pText);
	static std::wstring ANSIToUnicode(const char* pText);
	static std::string ANSIToUTF8(const char* pText);
};

/**
 * 字符串拼接,类似于java的StringBuilder
 * T 只能为std::string 或 std::wstring
 * !!!模板类不能把函数定义放到cpp中，否则外部使用该函数时，会出现链接错误
 */
template<class T> class StringBuilder {
public:
	void reset(void) {
		m_lst_str.clear();
	}
	void append(const T& str) {
		m_lst_str.push_back(str);
	}
	size_t length() {
		const int count = (int)m_lst_str.size();
		size_t total_size = 0;
		std::list<T>::iterator iter = m_lst_str.begin();
		for (; iter!=m_lst_str.end(); iter++) {
			total_size += (*iter).length();
		}
		return total_size;
	}
	T toString() {
		T str;
		const size_t total_size = length();
		if (total_size < 1) {
			return str;
		}		
		std::list<T>::iterator iter;
	
		str.resize(total_size);
		if (sizeof(str[0]) == 2) {//std::wstring
			wchar_t *p_temp = (wchar_t*)(str.data());
			for (iter = m_lst_str.begin(); iter!=m_lst_str.end(); iter++) {
				std::memcpy(p_temp,(*iter).c_str(),(*iter).length()*2);
				p_temp += (*iter).length();
			}
		} else { //std::string
			char *p_temp = (char*)(str.data());
			for (iter = m_lst_str.begin(); iter!=m_lst_str.end(); iter++) {
				std::memcpy(p_temp,(*iter).c_str(),(*iter).length());
				p_temp += (*iter).length();
			}
		}

		return str;
	}

private:
	std::list<T> m_lst_str;
};

