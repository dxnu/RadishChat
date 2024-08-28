#pragma once

#include "SQLiteAccounts.h"
#include "SQLiteFriends.h"
#include "MariaDBAccounts.h"
#include "MariaDBFriends.h"
#include "ReflectionFactory.h"
#include "Singleton.h"
#include <functional>


using namespace Utils;

namespace Radish {
namespace SQL {




//////////////////////////////////////////////////////////////////////////
// DataFactory factory;
//////////////////////////////////////////////////////////////////////////
class DataAccessFactory
{
public:
	DataAccessFactory()
		: m_dbName("SQLite")  // 当前实现的数据库有：SQLite, MariaDB
	{
		DBFactory::Instance().Register("SQLiteAccounts", []() { return std::make_shared<SQLiteAccounts>(); });
		DBFactory::Instance().Register("SQLiteFriends", []() { return std::make_shared<SQLiteFriends>(); });
		DBFactory::Instance().Register("MariaDBAccounts", []() { return std::make_shared<MariaDBAccounts>(); });
		DBFactory::Instance().Register("MariaDBFriends", []() { return std::make_shared<MariaDBFriends>(); });
	}

	// 根据类型创建对象
	auto Create(const std::string& className)
	{
		return DBFactory::Instance().CreateObject(m_dbName + className);
	}

private:
	std::string m_dbName;
	using DBFactory = Singleton<ReflectionFactory<RadishChatDatabaseBase,
		std::string, std::function<std::shared_ptr<RadishChatDatabaseBase>()>>>;
};

} // namespace SQL
} // namespace Radish
