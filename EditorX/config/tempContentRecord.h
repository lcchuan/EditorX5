#pragma once
#include "configBase.h"
#include<list>

class ScintillaEdit;
class TempContentRecord : public ConfigBase{
public:
	~TempContentRecord();
	static TempContentRecord& GetInstance() {return m_instance;}
	
	/**
	 * 编辑器打开后调用该函数已打开之前的临时文本记录
	 * @return 如果有临时记录且成功打开则返回true,否则返回false
	 */
	bool ShowRecords();

	void AddEditorForSave(ScintillaEdit* pEditor);
	void DeleteTempContent(ScintillaEdit* pEditor);

protected:
	//单例模式
	static TempContentRecord m_instance;
	TempContentRecord();

	//临时数据的最大个数
	static const int CONTENT_MAX_COUNT;
	//需要保存临时内容的编辑器
	ScintillaEdit** m_arrEditorForSave;
	//临界区，为了线程安全
	CRITICAL_SECTION m_criticalSection;

	//临时内容的保存线程
	HANDLE m_hSaveThread;
	static DWORD WINAPI SaveProcThread(LPVOID lpParam);
};
