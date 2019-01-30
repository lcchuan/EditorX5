#include "stdAfx.h"
#include "../StringUtils.h"
#include "fileRecord.h"

FileRecord::~FileRecord()
{
	ConfigBase::~ConfigBase();
}

int FileRecord::GetRecords(std::vector<std::wstring>& arrRecord)
{
	arrRecord.clear();
	SQLite sqlite;
	if (!ConnectSqlite(sqlite)) {
		return -1;
	}
	
	std::list<std::vector<std::pair<StringUtils::T_STRING,StringUtils::T_STRING>>> rs;
	std::wstring sql = TEXT("SELECT FILE_PATH FROM T_CONFIG_RECORD_FILE ORDER BY CREATE_TIME DESC");
	const int count = sqlite.Query(sql.c_str(),rs);
	sqlite.Close();

	std::list<std::vector<std::pair<StringUtils::T_STRING,StringUtils::T_STRING>>>::iterator iter = rs.begin();
	for (; iter != rs.end(); iter++) {
		arrRecord.push_back((*iter)[0].second);
	}

	return count;
}

bool FileRecord::DeleteRecord(std::wstring file)
{
	if (file.length() < 1) {
		return true;
	}

	SQLite sqlite;
	if (!ConnectSqlite(sqlite)) {
		return false;
	}
	std::wstring sql = StringUtils::format(TEXT("delete from T_CONFIG_RECORD_FILE where FILE_PATH='%s'"),file.c_str());
	int result = sqlite.Execute(sql.c_str());
	sqlite.Close();
	return (result > -1);
}

bool FileRecord::AddRecord(std::wstring file)
{
	if (file.length() < 1) {
		return true;
	}

	SQLite sqlite;
	if (!ConnectSqlite(sqlite)) {
		return false;
	}

	std::wstring sql = StringUtils::format(TEXT("update T_CONFIG_RECORD_FILE set CREATE_TIME=datetime(CURRENT_TIMESTAMP,'localtime') where FILE_PATH='%s'"),file.c_str());
	int result = sqlite.Execute(sql.c_str());
	if (result > 0) {
		return true;
	} else if (result < 0) {//说明该表尚未创建
		sql = TEXT("CREATE TABLE T_CONFIG_RECORD_FILE(FILE_PATH STRING(500) NOT NULL UNIQUE,CREATE_TIME TIME NOT NULL)");
		result = sqlite.Execute(sql.c_str());
		assert(result > -1);
	}

	//插入文件记录
	sql = StringUtils::format(TEXT("insert into T_CONFIG_RECORD_FILE(FILE_PATH,CREATE_TIME) VALUES('%s',datetime(CURRENT_TIMESTAMP,'localtime'))")
					, file.c_str());
	result = sqlite.Execute(sql.c_str());
	assert(result > -1);

	//限制最多仅保存的记录数，防止无限增大
	sql = TEXT("delete from T_CONFIG_RECORD_FILE where create_time not in(select create_time from T_CONFIG_RECORD_FILE order by create_time desc limit 40)");
	result = sqlite.Execute(sql.c_str());
	assert(result > -1);

	sqlite.Close();
	return true;
}