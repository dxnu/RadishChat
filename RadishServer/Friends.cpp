#include "Friends.h"
#include "DataAccessFactory.h"


namespace Radish {
namespace SQL {


bool Friends::CheckReferences(const char* dbName, const std::string& id)
{
	// 检查外键约束
	SQL::DataAccessFactory factory;
	auto tabAccounts = factory.Create("Accounts");
	tabAccounts->Open(dbName);

	DataType data;

	// 检测是否存在此id的用户
	data.Put("account_id", id);
	tabAccounts->Select(data);
	if (data.Size() == 0)
	{
		return false;
	}

	return true;
}

} // namespace SQL
} // namespace Radish
