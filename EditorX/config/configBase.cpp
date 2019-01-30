#include "stdAfx.h"
#include "configBase.h"
#include<string>
#include "../EditorXFrame.h"
#include "../StringUtils.h"
#include "../WinSDKUtils.h"

bool ConfigBase::ConnectSqlite(SQLite& sqlite)
{
	std::wstring db = WinSDKUtils::GetModulePath()+TEXT("\\data");
	if (!sqlite.Connect(db.c_str())) {
		std::wstring str = TEXT("连接数据库失败：");
		str += sqlite.GetLastError();
		EditorXFrame::GetInstance().MessageBox(str.c_str());
		return false;
	}
	return true;
}