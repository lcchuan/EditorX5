#pragma once
#include "BaseWnd.h"
#include <string>
#include <map>
#include <vector>
#include "StringUtils.h"
#include "scintilla/Scintilla.h"
#include "scintilla/SciLexer.h"

/**
 * 使用该类之前需要在程序初始化时调用::LoadLibrary(_T(".\\bin\\SciLexer.dll"))
 */
class ScintillaEdit : public BaseWnd
{
public:
	enum CHARSET{
		CHARSET_ANSI = 0
		, CHARSET_UTF8
		, CHARSET_UTF16LE
		, CHARSET_UTF16BE
	};

public:
	ScintillaEdit(void);
	virtual ~ScintillaEdit(void);

	virtual BOOL Create(HINSTANCE hInstance,HWND hParent,const RECT *p_rect=NULL);
	virtual BOOL SetFont(HFONT hFont);

	/**
	 * 打开文件
	 * @param file 文件的绝对路径,如果为NULL,则将弹出文件保存对话框以选择文件保存路径
	 * @param charset 表示以何种字符编码打开，如果为-1，则表示程序自动判断，否则请输入ScintillaEdit::CHARSET
	 * @return 如果成功则返回0，否则返回GetLastError()的值
	 */
	DWORD OpenFile(LPCTSTR file,int charset=-1);
	DWORD SaveFile(BOOL saveAs=FALSE);
	BOOL IsModified();

	virtual std::wstring GetToolTip() const {return m_strFilePath.length()<1 ? TEXT("") : m_strFilePath;}
	inline std::wstring GetFilePath() const { return m_strFilePath; }
	std::wstring GetFileName() const;
	std::wstring GetFileExt() const; //获取文件的后缀名

	/**
	 * 设置文本内容
	 * @param clearModified 是否清除修改标记，如果设置为true,则表示该编辑器的内容未被修改过
	 */
	void SetText(LPCTSTR text,bool clearModified=FALSE);
	//获取UTF8编码的文本内容
	std::string GetText_UTF8();
	std::string GetSeledText_UTF8();
	StringUtils::T_STRING GetSelectedText();
	void ReplaceSel(const std::wstring& text);

	inline void MakeUpperCase() { ExecuteScintilla(SCI_UPPERCASE); }
	inline void MakeLowerCase() { ExecuteScintilla(SCI_LOWERCASE); }

	//清除空行
	void ClearNullLine();
	//清除行号（从网站上拷贝代码时，经常会在每行的开头携带行号）
	void ClearLineNumber();

	//以当前光标位置为起点(如果选中了文字，则以选择的结束点为起点)，开始循环搜索，返回搜索到的位置，否则返回-1
	int FindByUTF8(const char* pUTF8Text,bool matchcase=true,bool wholeword=true,bool next=true);
	int Find(const StringUtils::T_CHAR *pText,bool matchcase=true,bool wholeword=true,bool next=true);
	/**
	 * 搜索并替换文本
	 * @param pText 欲被替换的文本
	 * @param pReplaceText 替换的文本
	 * @type 0-替换所有，1-替换并搜索下一个，2-替换并搜索上一个
	 * @return 返回替换的文本数量
	 */
	int Replace(const StringUtils::T_CHAR *pText,const StringUtils::T_CHAR *pReplaceText,bool matchcase,bool wholeword,int type);

	/*
	 * 字数统计
	 * @param result[out] 0-字符编码(ScintillaEdit::CHARSET_...)； 1-字符数； 2-字节数
	 */
	void WordStat(std::vector<intptr_t> &result);

	CHARSET GetCharset() const {return m_charset;}
	CHARSET SetCharset(const int& charset);
	bool IsContainBOM() const {return m_containBOM;}
	void SetContainBOM(bool containBOM) {m_containBOM=containBOM;}

	//设置语法着色，返回成功设置的语言类型，如果无该语言类型，返回Lexer::LANG_NONE
	int SetLanguage(const int& lang_type);
	int GetLanguage() const {return m_langType;}

	//设置自动换行
	void SetWordWrap(bool worderap) {ExecuteScintilla(SCI_SETWRAPMODE,worderap ? SC_WRAP_WORD : SC_WRAP_NONE);}
	bool GetWordWrap() {return (ExecuteScintilla(SCI_GETWRAPMODE) == SC_WRAP_WORD);}

	inline sptr_t ExecuteScintilla(UINT msg, uptr_t wParam=0, sptr_t lParam=0) {
		return m_pScintillaFunc(m_pScintillaObj, msg, wParam, lParam);
	}

protected:
	//用于执行scintilla的方法
	typedef sptr_t (SCINTILLA_FUNC) (void*, UINT, uptr_t, sptr_t);
	typedef void* SCINTILLA_PTR;
	SCINTILLA_FUNC *m_pScintillaFunc;
	SCINTILLA_PTR m_pScintillaObj;

	//文件内容相关属性
	std::wstring m_strFilePath;
	CHARSET m_charset;      //当前选择的字符编码
	CHARSET m_file_charset; //文件的原始字符编码
	bool m_containBOM; //文件是否携带BOM信息
	//标记语言类型，默认值Lexer::LANG_NODE
	int m_langType;

	//实例计数
	static int m_ref_count;

	/////////////////////////////////////////////////
	//右键菜单相关处理
	enum {
		IDM_EDITOR_UNDO = WM_USER+1
		, IDM_EDITOR_REDO
		, IDM_EDITOR_SELALL
		, IDM_EDITOR_COPY
		, IDM_EDITOR_CUT
		, IDM_EDITOR_PASTE
		, IDM_EDITOR_DELETE
		, IDM_EDITOR_CLEARNULLROW
		, IDM_EDITOR_CLEARROWNUM
		, IDM_EDITOR_CASEUPPER
		, IDM_EDITOR_CASELOWER
	};
	static std::vector<HMENU> m_arrHPopupMenu; //第一个元素为右键菜单的句柄，详情请参看GetContextMenu()
	static HMENU GetContextMenu();
	BOOL OnContentMenuClick(WPARAM wParam);

	//控件默认的消息处理函数的地址
	WNDPROC m_pDefaultWndProc;
	//自定义的消息处理函数
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	BOOL OnKeyDown(WPARAM wParam, LPARAM lParam);
};

