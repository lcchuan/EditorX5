#include "StdAfx.h"
#include "Lexer.h"
#include "ScintillaEdit.h"
#include "EditorXFrame.h"
#include "scintilla/SciLexer.h"
#include "scintilla/Scintilla.h"

const COLORREF Lexer::COLOR_LINE_BK = RGB(232,242,254);
const COLORREF Lexer::COLOR_STRING = RGB(128,128,128);
const COLORREF Lexer::COLOR_CHAR = RGB(128,128,128);
const COLORREF Lexer::COLOR_COMMENTS = RGB(0,128,0);
const COLORREF Lexer::COLOR_PRECOMPILED = RGB(149,0,85); //预编译
const COLORREF Lexer::COLOR_KEYWORD = RGB(58,163,255);   //关键字
const COLORREF Lexer::COLOR_CRIMSON = RGB(149,0,85);     //深红色
const COLORREF Lexer::COLOR_DARKGREEN =  RGB(0,128,0);   //深绿色
const COLORREF Lexer::COLOR_ORANGE =  RGB(214,150,80);   //橘黄色
const COLORREF Lexer::COLOR_LIGHTBROWN =  RGB(191,95,63);  //浅褐色

int Lexer::SetLanguage(ScintillaEdit* pEditor,const int& lang_type)
{
	int result(lang_type);

	//清除全局样式
	pEditor->ExecuteScintilla(SCI_STYLECLEARALL);
	switch (lang_type) {
	case LANG_C:
		pEditor->ExecuteScintilla(SCI_SETLEXER, SCLEX_CPP);
		pEditor->ExecuteScintilla(SCI_SETKEYWORDS, 0, (sptr_t)GetKeyword1_C());
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_C_WORD, COLOR_KEYWORD);
		//pEditor->ExecuteScintilla(SCI_STYLESETBOLD, SCE_C_WORD, TRUE);
		pEditor->ExecuteScintilla(SCI_SETKEYWORDS, 1, (sptr_t)GetKeyword2_C());
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_C_WORD2, COLOR_CRIMSON);

		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_C_STRING, COLOR_STRING); //字符串
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_C_CHARACTER, COLOR_STRING); //字符
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_C_PREPROCESSOR, COLOR_PRECOMPILED);//预编译开关
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_C_COMMENT, COLOR_COMMENTS);    //块注释
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_C_COMMENTLINE, COLOR_COMMENTS);//行注释
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_C_COMMENTDOC, COLOR_COMMENTS); //文档注释（/**开头）
		break;
	case LANG_JAVA:
		pEditor->ExecuteScintilla(SCI_SETLEXER, SCLEX_CPP);
		pEditor->ExecuteScintilla(SCI_SETKEYWORDS, 0, (sptr_t)GetKeyword1_JAVA());
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_C_WORD, COLOR_KEYWORD);
		pEditor->ExecuteScintilla(SCI_SETKEYWORDS, 1, (sptr_t)GetKeyword2_JAVA());
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_C_WORD2, COLOR_CRIMSON);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_C_STRING, COLOR_STRING); //字符串
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_C_CHARACTER, COLOR_STRING); //字符
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_C_COMMENT, COLOR_COMMENTS);    //块注释
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_C_COMMENTLINE, COLOR_COMMENTS);//行注释
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_C_COMMENTDOC, COLOR_COMMENTS); //文档注释（/**开头）
		break;
	case LANG_SQL:
		pEditor->ExecuteScintilla(SCI_SETLEXER, SCLEX_SQL);
		pEditor->ExecuteScintilla(SCI_SETKEYWORDS, 0, (sptr_t)GetKeyword1_SQL());
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_SQL_WORD, COLOR_KEYWORD);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_SQL_STRING, COLOR_STRING);    //字符串
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_SQL_CHARACTER, COLOR_STRING); //字符
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_SQL_COMMENT, COLOR_COMMENTS);    //块注释
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_SQL_COMMENTLINE, COLOR_COMMENTS);//行注释
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_SQL_COMMENTDOC, COLOR_COMMENTS); //文档注释（/**开头）
		break;
	case LANG_XML:
		pEditor->ExecuteScintilla(SCI_SETLEXER, SCLEX_XML);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_H_TAG, COLOR_CRIMSON);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_H_XMLSTART, COLOR_DARKGREEN); // <?
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_H_XMLEND, COLOR_DARKGREEN);   // ?>
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_H_COMMENT, COLOR_COMMENTS);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_H_ATTRIBUTE, COLOR_KEYWORD);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_H_DOUBLESTRING, COLOR_STRING);

		//如果不调用SCI_SETKEYWORDS，在切换一次其它语言后，在切换回XML，便不刷新样式！
		pEditor->ExecuteScintilla(SCI_SETKEYWORDS, 1, (sptr_t)"xml");
		break;
	case LANG_JSP:
		pEditor->ExecuteScintilla(SCI_SETLEXER, SCLEX_HTML);

		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_H_TAG, COLOR_CRIMSON);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_H_ASP, COLOR_LIGHTBROWN); // <% ... %>
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_H_COMMENT, COLOR_COMMENTS);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_H_ATTRIBUTE, COLOR_KEYWORD);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_H_DOUBLESTRING, COLOR_STRING);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_H_SINGLESTRING, COLOR_STRING);

		//for js
		pEditor->ExecuteScintilla(SCI_SETKEYWORDS, 1, (sptr_t)GetKeyword1_JS());
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_HJ_KEYWORD, COLOR_KEYWORD);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_HJ_DOUBLESTRING, COLOR_STRING);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_HJ_SINGLESTRING, COLOR_STRING);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_HJ_COMMENT, COLOR_COMMENTS);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_HJ_COMMENTLINE, COLOR_COMMENTS);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_HJ_COMMENTDOC, COLOR_COMMENTS);
		//pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_HJ_SYMBOLS, RGB(255,0,0)); //括号、中括号等

		//for java
		//pEditor->ExecuteScintilla(SCI_SETKEYWORDS, 3, (sptr_t)GetKeyword1_JAVA());
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_HJA_KEYWORD, COLOR_KEYWORD);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_HJA_COMMENT, COLOR_COMMENTS);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_HJA_COMMENTLINE, COLOR_COMMENTS);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_HJA_COMMENTDOC, COLOR_COMMENTS);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_HJA_DOUBLESTRING, COLOR_STRING);
		break;
	case LANG_HTML:
		pEditor->ExecuteScintilla(SCI_SETLEXER, SCLEX_HTML);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_H_TAG, COLOR_CRIMSON);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_H_COMMENT, COLOR_COMMENTS);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_H_ATTRIBUTE, COLOR_KEYWORD);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_H_DOUBLESTRING, COLOR_STRING);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_H_SINGLESTRING, COLOR_STRING);

		//for js
		pEditor->ExecuteScintilla(SCI_SETKEYWORDS, 1, (sptr_t)GetKeyword1_JS());
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_HJ_KEYWORD, COLOR_KEYWORD);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_HJ_DOUBLESTRING, COLOR_STRING);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_HJ_SINGLESTRING, COLOR_STRING);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_HJ_COMMENT, COLOR_COMMENTS);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_HJ_COMMENTLINE, COLOR_COMMENTS);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_HJ_COMMENTDOC, COLOR_COMMENTS);
		break;
	case LANG_JS:
		pEditor->ExecuteScintilla(SCI_SETLEXER, SCLEX_CPP);
		pEditor->ExecuteScintilla(SCI_SETKEYWORDS, 0, (sptr_t)GetKeyword1_JS());
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_C_WORD, COLOR_KEYWORD);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_C_WORD2, COLOR_CRIMSON);
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_C_STRING, COLOR_STRING); //字符串
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_C_CHARACTER, COLOR_STRING); //字符
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_C_COMMENT, COLOR_COMMENTS);    //块注释
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_C_COMMENTLINE, COLOR_COMMENTS);//行注释
		pEditor->ExecuteScintilla(SCI_STYLESETFORE, SCE_C_COMMENTDOC, COLOR_COMMENTS); //文档注释（/**开头）
		break;
	default:
		if (lang_type != LANG_NONE) {
			EditorXFrame::GetInstance().MessageBox(TEXT("暂不支持该语言"));
		}
		pEditor->ExecuteScintilla(SCI_SETLEXER, SCLEX_NULL);
		result = LANG_NONE;
	}

	//设置当前行背景色
	pEditor->ExecuteScintilla(SCI_SETCARETLINEVISIBLE, TRUE);
	pEditor->ExecuteScintilla(SCI_SETCARETLINEBACK, COLOR_LINE_BK);
	return result;
}

int Lexer::GetLangByExt(LPCTSTR ext)
{
	if (_wcsicmp(ext,TEXT("h")) == 0 
		|| _wcsicmp(ext,TEXT("cpp")) == 0 
		|| _wcsicmp(ext,TEXT("cxx")) == 0 
		|| _wcsicmp(ext,TEXT("c")) == 0) {
		return LANG_C;
	} else if (_wcsicmp(ext,TEXT("java")) == 0) {
		return LANG_JAVA;
	} else if (_wcsicmp(ext,TEXT("sql")) == 0) {
		return LANG_SQL;
	} else if (_wcsicmp(ext,TEXT("xml")) == 0) {
		return LANG_XML;
	} else if (_wcsicmp(ext,TEXT("jsp")) == 0) {
		return LANG_JSP;
	} else if (_wcsicmp(ext,TEXT("html")) == 0) {
		return LANG_HTML;
	} else if (_wcsicmp(ext,TEXT("js")) == 0) {
		return LANG_JS;
	}
	return LANG_NONE;
}

char* Lexer::GetKeyword1_C()
{
	return "asm auto"
		" bool break"
		" case catch char class const const_cast continue"
        " default delete do double dynamic_cast"
        " else enum explicit extern"
		" false float for friend"
		" goto"
		" if inline int"
		" long"
		" mutable"
		" namespace new"
		" operator"
		" private protected public "
		" register reinterpret_cast return"
		" short signed size_t sizeof static static_cast struct switch"
		" template this throw true try typedef typeid typename "
		" union unsigned using"
		" virtual void volatile"
		" wchar_t while";
}

char* Lexer::GetKeyword2_C()
{
	return "BOOL UINT"
		" DWORD"
		" FALSE"
		" HWND"
		" LPARAM LRESULT"
		" RECT RGB"
		" _T TEXT TRUE"
		" UINT"
		" WPARAM";
}

char* Lexer::GetKeyword1_JAVA()
{
	return "abstract ArrayList assert"
		" boolean break byte"
		" case catch char class continue const"
		" default do double Double"
		" else enum Exception extends"
		" false final finally float for"
		" goto"
		" HashMap HttpServletRequest HttpServletResponse"
		" if implements instanceof int Integer interface"
		" List long"
		" Map"
		" native new null"
		" private protected public"
		" return"
		" short static strictfp String StringBuilder StringBuffer switch super synchronized"
		" this throw throws transient true try"
		" void volatile"
		" while";
}

char* Lexer::GetKeyword2_JAVA()
{
	return "import package";
}

char* Lexer::GetKeyword1_SQL()
{//关键字必须全部为小写（Scintilla只认小写……）
	return "all alter and as"
		" begin between break by"
		" case close column comment constraint commit continue create cursor"
		" declare default delete desc distinct drop"
		" else end exec execute exit exists"
		" fetch from for foreign function"
		" go grant group"
		" having"
		" if immediate in inner insert integer into  is"
		" join"
		" key"
		" left like loop"
		" not null"
		" on open or order"
		" references return revoke right rollback rownum primary proc procedure"
		" select set"
		" table tablespace then timestamp top truncate"
		" union update"
		" values varchar varchar2 view"
		" when where while with";
}

char* Lexer::GetKeyword1_JS()
{
	return "alert Array"
		   " break"
		   " case catch char confirm const continue"
		   " Date debugger default delete do"
		   " else evel"
		   " false finally for function"
		   " if in instanceof"
		   " Math"
		   " new null Number"
		   " parseInt"
		   " return"
		   " switch String"
		   " this throw true try typeof"
		   " var void"
		   " while with";
}