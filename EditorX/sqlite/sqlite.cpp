#include "stdAfx.h"
#include "sqlite.h"

HINSTANCE SQLite::m_hInstDll = NULL;
int (_stdcall *SQLite::m_p_sqlite3_open_v2)(const char *filename,sqlite3 **ppDb, int flags,const char *zVfs);
int (_stdcall *SQLite::m_p_sqlite3_close_v2)(sqlite3*);
int (_stdcall *SQLite::m_p_sqlite3_exec)(sqlite3*,const char *sql,int (*callback)(void*,int,char**,char**),void *data,char **errmsg);
void (_stdcall *SQLite::m_p_sqlite3_free)(void*);
int (_stdcall *SQLite::m_p_sqlite3_prepare_v2)(sqlite3*,const char *,int ,sqlite3_stmt **,const char **);
int (_stdcall *SQLite::m_p_sqlite3_step)(sqlite3_stmt*);
int (_stdcall *SQLite::m_p_sqlite3_finalize)(sqlite3_stmt*);
const char* (_stdcall *SQLite::m_p_sqlite3_errmsg)(sqlite3*);
int (_stdcall *SQLite::m_p_sqlite3_changes)(sqlite3*);
int (_stdcall *SQLite::m_p_sqlite3_bind_parameter_count)(sqlite3_stmt*);
int (_stdcall *SQLite::m_p_sqlite3_bind_null)(sqlite3_stmt*, int);
int (_stdcall *SQLite::m_p_sqlite3_bind_text)(sqlite3_stmt*,int,const char*,int,void(*)(void*));
int (_stdcall *SQLite::m_p_sqlite3_reset)(sqlite3_stmt *pStmt);

SQLite::SQLite()
{
	m_pDB = nullptr;
}

SQLite::~SQLite()
{
	Close();
}

bool SQLite::LoadSqliteDll(const StringUtils::T_CHAR* pDllPath)
{
	if (m_hInstDll) {
		return true;
	}
	m_hInstDll = ::LoadLibrary(pDllPath);
	if (!m_hInstDll) {
		unsigned long err = ::GetLastError();
		assert(false);
		return false;
	}

	m_p_sqlite3_open_v2 = (int (_stdcall *)(const char *,sqlite3 **, int ,const char *))(GetProcAddress(m_hInstDll,"sqlite3_open_v2"));
	m_p_sqlite3_close_v2 = (int (_stdcall *)(sqlite3*))(GetProcAddress(m_hInstDll,"sqlite3_close_v2"));
	m_p_sqlite3_exec = (int (_stdcall *)(sqlite3*,const char *,int (*)(void*,int,char**,char**),void *,char **))
		(GetProcAddress(m_hInstDll,"sqlite3_exec"));
	m_p_sqlite3_free = (void (_stdcall *)(void*))(GetProcAddress(m_hInstDll,"sqlite3_free"));
	m_p_sqlite3_prepare_v2 = (int (_stdcall *)(sqlite3*,const char *,int ,sqlite3_stmt **,const char **))(GetProcAddress(m_hInstDll,"sqlite3_prepare_v2"));
	m_p_sqlite3_step = (int (_stdcall *)(sqlite3_stmt*))(GetProcAddress(m_hInstDll,"sqlite3_step"));
	m_p_sqlite3_finalize = (int (_stdcall *)(sqlite3_stmt*))(GetProcAddress(m_hInstDll,"sqlite3_finalize"));
	m_p_sqlite3_errmsg = (const char* (_stdcall *)(sqlite3*))(GetProcAddress(m_hInstDll,"sqlite3_errmsg"));
	m_p_sqlite3_changes = (int (_stdcall *)(sqlite3*))(GetProcAddress(m_hInstDll,"sqlite3_changes"));
	m_p_sqlite3_bind_parameter_count = (int (_stdcall *)(sqlite3_stmt*))(GetProcAddress(m_hInstDll,"sqlite3_bind_parameter_count"));
	m_p_sqlite3_bind_null = (int (_stdcall *)(sqlite3_stmt*, int))(GetProcAddress(m_hInstDll,"sqlite3_bind_null"));
	m_p_sqlite3_bind_text = (int (_stdcall *)(sqlite3_stmt*,int,const char*,int,void(*)(void*)))(GetProcAddress(m_hInstDll,"sqlite3_bind_text"));
	m_p_sqlite3_reset = (int (_stdcall *)(sqlite3_stmt*))(GetProcAddress(m_hInstDll,"sqlite3_reset"));
	return true;
}

void SQLite::UnloadSqliteDll()
{
	if (m_hInstDll) {
		::FreeLibrary(m_hInstDll);
		m_hInstDll = nullptr;
	}
}

bool SQLite::Connect(const StringUtils::T_CHAR* pDatabasePath)
{
	m_errmsg.empty();
	if (!m_hInstDll) {
		assert(false);
		m_errmsg = TEXT("尚未加载sqltie3 dll");
		return false;
	}

	Close();

	std::string utf8_str = ConvertToUTF8(pDatabasePath);
	if (utf8_str.length() < 1) {
		m_errmsg = TEXT("无效的空数据库地址");
		assert(false);
		return false;
	}
	int result = m_p_sqlite3_open_v2(utf8_str.c_str(), &m_pDB,SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE,0);
  
    if(SQLITE_OK != result)  
    {
		m_errmsg = StringUtils::format(TEXT("连接失败：%s"),pDatabasePath);
        assert(false);  
        return false;  
    }  
  
    return true;
}

void SQLite::Close()
{
	if(nullptr == m_pDB) {  
        return ;  
    }  
 
    m_p_sqlite3_close_v2(m_pDB);
	m_pDB = nullptr;
}

int SQLite::Execute(const StringUtils::T_CHAR* sql)
{
	m_errmsg.empty();
	if (nullptr == m_pDB) {
		m_errmsg = TEXT("空数据库连接");
		assert(false);
		return -1;
	}	

	std::string utf8_str = ConvertToUTF8(sql);
	if (utf8_str.length() < 1) {
		m_errmsg = TEXT("无效的空SQL语句");
		assert(false);
		return -1;
	}

	int count(0);
	sqlite3_stmt *pStmt;
	int result = m_p_sqlite3_prepare_v2(m_pDB, utf8_str.c_str(), -1, &pStmt, 0);
	if(SQLITE_OK != result){
		m_errmsg = FromUTF8(m_p_sqlite3_errmsg(m_pDB));
		return -1;
	}
	result = m_p_sqlite3_step(pStmt);
    if(SQLITE_DONE != result){
		assert(false);
		m_errmsg = FromUTF8(m_p_sqlite3_errmsg(m_pDB));
		m_p_sqlite3_finalize(pStmt);
		return -1;
    }
	//统计影响记录数目
    count = m_p_sqlite3_changes(m_pDB);
	//清理语句句柄
	m_p_sqlite3_finalize(pStmt);
	return count;
}

int SQLite::ExecuteWithUTF8StrParam(const StringUtils::T_CHAR* sql,...)
{
	m_errmsg.empty();
	if (nullptr == m_pDB) {
		m_errmsg = TEXT("空数据库连接");
		assert(false);
		return -1;
	}	

	std::string utf8_str = ConvertToUTF8(sql);
	if (utf8_str.length() < 1) {
		m_errmsg = TEXT("无效的空SQL语句");
		assert(false);
		return -1;
	}

	int count(0);
	sqlite3_stmt *pStmt;
	int result = m_p_sqlite3_prepare_v2(m_pDB, utf8_str.c_str(), -1, &pStmt, 0);
	if(SQLITE_OK != result){
		m_errmsg = FromUTF8(m_p_sqlite3_errmsg(m_pDB));
		return -1;
	}

	const int param_count = m_p_sqlite3_bind_parameter_count(pStmt);
	if (param_count > 0) {
		char* pParam = NULL;
		va_list arg;
		va_start(arg, sql);
		for (int i=1; i<=param_count; i++) {
			pParam = va_arg(arg, char*);
			if (pParam == nullptr || *pParam == 0) {
				m_p_sqlite3_bind_null(pStmt,i);
			} else {
				m_p_sqlite3_bind_text(pStmt,i,pParam,-1,SQLITE_STATIC);
			}
		}
		va_end(arg);
	}

	result = m_p_sqlite3_step(pStmt);
    if(SQLITE_DONE != result){
		assert(false);
		m_errmsg = FromUTF8(m_p_sqlite3_errmsg(m_pDB));
		m_p_sqlite3_finalize(pStmt);
		return -1;
    }
	if (param_count > 0) {
		m_p_sqlite3_reset(pStmt);
	}

	//统计影响记录数目
    count = m_p_sqlite3_changes(m_pDB);
	//清理语句句柄
	m_p_sqlite3_finalize(pStmt);
	return count;
}

int SQLite::ExecuteBatch(const std::vector<StringUtils::T_STRING>& sqls)
{
	m_errmsg.empty();
	if (nullptr == m_pDB) {
		m_errmsg = TEXT("空数据库连接");
		assert(false);
		return -1;
	}

	int count(0),result(0);
	sqlite3_stmt *pStmt;
	std::string sql;
	for (int i=0; i<sqls.size(); i++) {
		if (sqls[i].length() < 6) {
			assert(false);
			continue;
		}
		sql = ConvertToUTF8(sqls[i].c_str());

		result = m_p_sqlite3_prepare_v2(m_pDB, sql.c_str(), -1, &pStmt, 0);
		if(SQLITE_OK != result){
			assert(false);
			m_errmsg = FromUTF8(m_p_sqlite3_errmsg(m_pDB));
			return -1;
		}

		result = m_p_sqlite3_step(pStmt);
		if(SQLITE_DONE != result){
			assert(false);
			m_errmsg = FromUTF8(m_p_sqlite3_errmsg(m_pDB));
			m_p_sqlite3_finalize(pStmt);
			return -1;
		}

		//统计影响记录数目
		count += m_p_sqlite3_changes(m_pDB);
		//清理语句句柄
		m_p_sqlite3_finalize(pStmt);
	}
	return count;
}

int SQLite::Query(const StringUtils::T_CHAR* sql,std::list<std::vector<std::pair<StringUtils::T_STRING,StringUtils::T_STRING>>>& rs)
{
	rs.clear();
	m_errmsg.empty();
	if (nullptr == m_pDB) {
		m_errmsg = TEXT("空数据库连接");
		assert(false);
		return -1;
	}

	char *errmsg = nullptr;
	std::string utf8_str = ConvertToUTF8(sql);
	if (utf8_str.length() < 1) {
		m_errmsg = TEXT("无效的空SQL语句");
		assert(false);
		return -1;
	}
	int result = m_p_sqlite3_exec(m_pDB,utf8_str.c_str(),QueryCallback,&rs,&errmsg);
	if (SQLITE_OK == result) {
		return (int)rs.size();
	} else {
		if (errmsg == nullptr) {
			m_errmsg = TEXT("未知错误");
		} else {
			m_errmsg = FromUTF8(errmsg);
			m_p_sqlite3_free(errmsg);
		}
		assert(false);
		return -1;
	}
}

//@param data 对应sqlite3_exec的第四个参数
int SQLite::QueryCallback(void* data, int n_columns, char** column_values,char** column_names)
{
	std::list<std::vector<std::pair<StringUtils::T_STRING,StringUtils::T_STRING>>> *pRs
		= (std::list<std::vector<std::pair<StringUtils::T_STRING,StringUtils::T_STRING>>> *)data;
	std::vector<std::pair<StringUtils::T_STRING,StringUtils::T_STRING>> record;
	for (int i=0; i<n_columns; i++) {
		record.push_back(std::pair<StringUtils::T_STRING,StringUtils::T_STRING>(FromUTF8(column_names[i]),FromUTF8(column_values[i])));
	}
	pRs->push_back(record);
	return 0;
}

std::string SQLite::ConvertToUTF8(const StringUtils::T_CHAR* str)
{
	std::string utf8_str;
#ifdef _UNICODE
	utf8_str = StringUtils::UnicodeToUTF8(str);
#else
	pDest = StringUtils::ANSIToUTF8(str);
#endif
	return utf8_str;
}

StringUtils::T_STRING SQLite::FromUTF8(const char* str)
{
	StringUtils::T_STRING dest;
	if (str == nullptr || *str == 0) {
		return dest;
	}
	
#ifdef _UNICODE
	dest = StringUtils::UTF8ToUnicode(str);
#else
	dest = StringUtils::UTF8ToANSI(str);
#endif
	return dest;
}