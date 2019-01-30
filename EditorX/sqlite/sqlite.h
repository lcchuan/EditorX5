#pragma once

#include "sqlite3.h"
#include <string>
#include <list>
#include <vector>
#include "../StringUtils.h"

/**
 * 对sqlite的操作封装
 * 20180126 lcc
 */
class SQLite {
protected:
	static HINSTANCE m_hInstDll;

public:
	//加载sqlite dll，不要重复调用该函数
	static bool LoadSqliteDll(const StringUtils::T_CHAR* pDllPath);
	static void UnloadSqliteDll();

	SQLite();
	~SQLite();

	bool Connect(const StringUtils::T_CHAR* pDatabasePath);
	void Close();
	inline StringUtils::T_STRING GetLastError() {return m_errmsg;}

	/**
	 * 执行非select语句
	 * @return 返回受影响的记录数
	 */
	int Execute(const StringUtils::T_CHAR* sql);
	//参数化执行，参数仅支持UTF8字符串 char*
	int ExecuteWithUTF8StrParam(const StringUtils::T_CHAR* sql,...);
	int ExecuteBatch(const std::vector<StringUtils::T_STRING>& sqls);

	/**
	 * 执行select查询
	 * @param sql[in] select sql
	 * @param rs[out] 查询的输出结果
	 * @return <0表示查询失败（可通过GetLastError获取错误信息）;成功返回结果集的行数
	 */
	int Query(const StringUtils::T_CHAR* sql,std::list<std::vector<std::pair<StringUtils::T_STRING,StringUtils::T_STRING>>>& rs);

protected:
	sqlite3 *m_pDB;
	StringUtils::T_STRING m_errmsg;

	/**
	 * 用于sqlite3_exec的第三个参数
	 * @param data 对应sqlite3_exec的第四个参数
	 */
	static int QueryCallback(void* data, int n_columns, char** column_values,char** column_names);

	/**
	 * sqlite3仅接收utf8编码，故需要先将unicode或ansi转换为utf8
	 * @return 使用完后需要delete[]
	 */
	static std::string ConvertToUTF8(const StringUtils::T_CHAR* str);
	//@param str utf8字符串
	static StringUtils::T_STRING FromUTF8(const char* str);

////////////////////////////////////////////////
//导出sqlite3 dll的函数 
protected:
	/**
	 * @param filename Database filename (UTF-8)
	 * @param ppDb[OUT] SQLite db handle
	 * @param flags SQLITE_OPEN_NOMUTEX: 设置数据库连接运行在多线程模式(没有指定单线程模式的情况下)
     *              SQLITE_OPEN_FULLMUTEX：设置数据库连接运行在串行模式。
     *              SQLITE_OPEN_SHAREDCACHE：设置运行在共享缓存模式。
     *              SQLITE_OPEN_PRIVATECACHE：设置运行在非共享缓存模式。
     *              SQLITE_OPEN_READWRITE：指定数据库连接可以读写。
     *              SQLITE_OPEN_CREATE：如果数据库不存在，则创建。
	 */
	static int (_stdcall *m_p_sqlite3_open_v2)(const char *filename,sqlite3 **ppDb, int flags,const char *zVfs);

	//关闭数据库连接，若关闭时连接上有未提交的事务，该事务会自动回滚。
	static int (_stdcall *m_p_sqlite3_close_v2)(sqlite3*);

	/**
	 *
	 * @param sqlite3 An open database
	 * @param sql SQL to be evaluated,sql可以包含多个SQL命令，语句之间以分号隔开
	 * @param callback Callback function
	 * @param data 1st argument to callback
	 * @param errmsg Error msg written here ！！！sqlite3_free()！！！
     * @remark sqlite3_exec()可以执行多个SQL命令，但是函数不保证事务，即已执行成功的语句，不会因为后面执行失败的语句而回滚。
	 */
	static int (_stdcall *m_p_sqlite3_exec)(sqlite3*,const char *sql,int (*callback)(void*,int,char**,char**),void *data,char **errmsg);
	static void (_stdcall *m_p_sqlite3_free)(void*);

	/**
     * @param zSql SQL statement, UTF-8 encoded
     * @param nByte Maximum length of zSql in bytes
	 * @param ppStmt[out] Statement handle
	 * @param pzTail[out] Pointer to unused portion of zSql
	 */
	static int (_stdcall *m_p_sqlite3_prepare_v2)(sqlite3*,const char * zSql,int nByte,sqlite3_stmt **ppStmt,const char **pzTail);
	static int (_stdcall *m_p_sqlite3_step)(sqlite3_stmt*);
	static int (_stdcall *m_p_sqlite3_finalize)(sqlite3_stmt *pStmt);
	static const char* (_stdcall *m_p_sqlite3_errmsg)(sqlite3*);

	//得到语句影响的行数
	static int (_stdcall *m_p_sqlite3_changes)(sqlite3*);

	//获取参数化sql中的参数个数
	static int (_stdcall *m_p_sqlite3_bind_parameter_count)(sqlite3_stmt*);
	static int (_stdcall *m_p_sqlite3_bind_null)(sqlite3_stmt*, int);
	static int (_stdcall *m_p_sqlite3_bind_text)(sqlite3_stmt*,int,const char*,int,void(*)(void*));
	static int (_stdcall *m_p_sqlite3_reset)(sqlite3_stmt *pStmt);
};