#pragma once
#include <string>
#include <vector>
#include "configBase.h"

class FileRecord : public ConfigBase{
public:
	~FileRecord();

	/**
	 * 获取曾经打开的文件记录
	 * @param arrRecord[out] 曾经打开的文件记录
	 * @return 曾经打开文件的数量
	 */
	int GetRecords(std::vector<std::wstring>& arrRecord);

	//删除文件历史记录
	bool DeleteRecord(std::wstring file);

	//记录曾经打开的文件路径
	bool AddRecord(std::wstring file);
};