#pragma once

/**
 * 用于语法着色
 */
class ScintillaEdit;
class Lexer
{
public:
	enum {
		LANG_NONE = 0
		,LANG_C
		,LANG_JAVA
		,LANG_SQL
		,LANG_XML
		,LANG_JSP
		,LANG_HTML
		,LANG_JS
		,_LANG_END //为了方便遍历语言类型
	};

	//颜色定义
	static const COLORREF COLOR_LINE_BK;     //行背景色
	static const COLORREF COLOR_STRING;      //字符串
	static const COLORREF COLOR_CHAR;        //字符
	static const COLORREF COLOR_COMMENTS;    //注释
	static const COLORREF COLOR_PRECOMPILED; //预编译
	static const COLORREF COLOR_KEYWORD;     //关键字
	static const COLORREF COLOR_CRIMSON;     //深红色
	static const COLORREF COLOR_DARKGREEN;   //深绿色
	static const COLORREF COLOR_ORANGE;      //橘黄色
	static const COLORREF COLOR_LIGHTBROWN;  //浅褐色

public:

	//通过文件后缀名判断语言类型
	static int GetLangByExt(LPCTSTR ext);
	//设置编辑器的语法着色，返回成功设置的语言类型，如果无该语言类型，返回Lexer::LANG_NONE
	static int SetLanguage(ScintillaEdit* pEditor,const int& lang_type);

protected:
	///////////////////////////////////////////////////////////////
	//获取相关语言的关键字
	static char* GetKeyword1_C();
	static char* GetKeyword2_C();

	static char* GetKeyword1_JAVA();
	static char* GetKeyword2_JAVA();

	static char* GetKeyword1_SQL();
	static char* GetKeyword1_JS();
};

