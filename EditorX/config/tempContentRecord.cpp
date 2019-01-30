#include "stdAfx.h"
#include "tempContentRecord.h"
#include "../ScintillaEdit.h"
#include "../EditorXFrame.h"

//不可能在3秒内同时修改6个以上的编辑器
const int TempContentRecord::CONTENT_MAX_COUNT = 6;
TempContentRecord TempContentRecord::m_instance;

TempContentRecord::TempContentRecord() {
	m_hSaveThread = NULL;
	m_arrEditorForSave = nullptr;
	::InitializeCriticalSection(&m_criticalSection);
}
TempContentRecord::~TempContentRecord()
{
	::DeleteCriticalSection(&m_criticalSection);
	if (m_arrEditorForSave) {
		delete []m_arrEditorForSave;
		m_arrEditorForSave = nullptr;
	}
}

void TempContentRecord::AddEditorForSave(ScintillaEdit* pEditor)
{
	//创建一个保存临时内容的线程
	if (m_hSaveThread == NULL) {
		m_hSaveThread = ::CreateThread(NULL,0,SaveProcThread,this,0,0);
		assert(m_hSaveThread != NULL);
	}
	if (nullptr == m_arrEditorForSave) {
		m_arrEditorForSave = new ScintillaEdit*[CONTENT_MAX_COUNT];
		::memset(m_arrEditorForSave,0,sizeof(ScintillaEdit*)*CONTENT_MAX_COUNT);
	}

	::EnterCriticalSection(&m_criticalSection);
	int null_index(-1);
	for (int i=0; i<CONTENT_MAX_COUNT; i++) {
		if (m_arrEditorForSave[i] == pEditor) {
			if (null_index > -1) {
				m_arrEditorForSave[null_index] = 0;
			}
			break;
		} else if (null_index < 0 && 0 == m_arrEditorForSave[i]) {
			null_index = i;
			m_arrEditorForSave[null_index] = pEditor;
		}
	}
	::LeaveCriticalSection(&m_criticalSection);
}

DWORD WINAPI TempContentRecord::SaveProcThread(LPVOID lpParam)
{
	TempContentRecord *pMe = (TempContentRecord*)lpParam;
	SQLite sqlite;
	bool db_connected(false);
	ScintillaEdit* pEditor = NULL;
	std::string str_utf8;
	std::wstring sql;
	int sql_result(0);
	while (true) {
		::Sleep(3000);
		if (nullptr == pMe->m_arrEditorForSave) {
			continue;
		}
		
		db_connected = false;
		::EnterCriticalSection(&(pMe->m_criticalSection));		
		for (int i=0; i<TempContentRecord::CONTENT_MAX_COUNT; i++) {
			pEditor = pMe->m_arrEditorForSave[i];
			if (0 == pEditor) {
				continue;
			}
			if (!db_connected) {
				if (!ConnectSqlite(sqlite)) {
					::LeaveCriticalSection(&(pMe->m_criticalSection));
					assert(false);
					return 0;
				}
				db_connected = true;
			}
			str_utf8 = pEditor->GetText_UTF8();
			if (str_utf8.length() < 1) {
				sql = StringUtils::format(TEXT("delete from T_CONFIG_RECORD_TEMPCONTENT where id=%d"),pEditor);
				sql_result = sqlite.Execute(sql.c_str());
			} else {
				sql = StringUtils::format(TEXT("update T_CONFIG_RECORD_TEMPCONTENT set CONTENT=?,CREATE_TIME=datetime(CURRENT_TIMESTAMP,'localtime') where id=%d"),pEditor);
				sql_result = sqlite.ExecuteWithUTF8StrParam(sql.c_str(),str_utf8.c_str());
				if (sql_result < 1) {//更新无记录，便插入
					sql = StringUtils::format(TEXT("insert into T_CONFIG_RECORD_TEMPCONTENT(id,content,CREATE_TIME) VALUES(%d,?,datetime(CURRENT_TIMESTAMP,'localtime'))"),pEditor);
					sql_result = sqlite.ExecuteWithUTF8StrParam(sql.c_str(),str_utf8.c_str());
				}
			}
			pMe->m_arrEditorForSave[i] = 0;
		}
		if (db_connected) {
			sqlite.Close();
		}
		::LeaveCriticalSection(&(pMe->m_criticalSection));
		
	}
	return 0;
}

void TempContentRecord::DeleteTempContent(ScintillaEdit* pEditor)
{
	if (nullptr == m_arrEditorForSave) {
		return;
	}
	::EnterCriticalSection(&m_criticalSection);
	for (int i=0; i<TempContentRecord::CONTENT_MAX_COUNT; i++) {
		if (m_arrEditorForSave[i] == pEditor) {
			m_arrEditorForSave[i] = 0;
			break;
		}
	}
	::LeaveCriticalSection(&m_criticalSection);

	SQLite sqlite;
	if (!ConnectSqlite(sqlite)) {
		return ;
	}
	std::wstring sql = StringUtils::format(TEXT("delete from T_CONFIG_RECORD_TEMPCONTENT where id=%d"),pEditor);
	sqlite.Execute(sql.c_str());
}

//如果有临时记录且成功打开则返回true,否则返回false
bool TempContentRecord::ShowRecords()
{
	SQLite sqlite;
	if (!ConnectSqlite(sqlite)) {
		return false;
	}

	EditorXFrame& frame = EditorXFrame::GetInstance();
	ScintillaEdit* pEditor = NULL;
	std::list<std::vector<std::pair<StringUtils::T_STRING,StringUtils::T_STRING>>> rs;
	std::wstring sql = TEXT("SELECT id,content FROM T_CONFIG_RECORD_TEMPCONTENT ORDER BY CREATE_TIME");
	std::vector<std::wstring> sqls;
	const int count = sqlite.Query(sql.c_str(),rs);
	if (count < 0) {//尚未创建该表，则创建之
		sql = TEXT("CREATE TABLE T_CONFIG_RECORD_TEMPCONTENT(ID INT PRIMARY KEY UNIQUE NOT NULL,CONTENT TEXT,CREATE_TIME TIME NOT NULL)");
		int rs = sqlite.Execute(sql.c_str());
		assert(rs > -1);
		return false;
	} else if (count < 1) {
		return false;
	}
	std::list<std::vector<std::pair<StringUtils::T_STRING,StringUtils::T_STRING>>>::iterator iter = rs.begin();
	for (; iter != rs.end(); iter++) {
		pEditor = frame.CreateEditor(TEXT("history temp"));
		pEditor->SetText((*iter)[1].second.c_str(),TRUE);

		//将id更新为当前编辑器的id
		sql = StringUtils::format(TEXT("update T_CONFIG_RECORD_TEMPCONTENT set id=%d where id=%s"), pEditor,(*iter)[0].second.c_str());
		sqls.push_back(sql);
	}
	if (sqls.size() > 0) {
		sqlite.ExecuteBatch(sqls);
	} else {
		assert(false);
	}
	
	return true;
}