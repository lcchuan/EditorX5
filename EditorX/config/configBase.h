#pragma once
#include "../sqlite/sqlite.h"

class ConfigBase{
public:
	static bool ConnectSqlite(SQLite& sqlite);
};