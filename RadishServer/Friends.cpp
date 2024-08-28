#include "Friends.h"
#include "DataAccessFactory.h"


namespace Radish {
namespace SQL {


bool Friends::CheckReferences(const char* dbName, const std::string& id)
{
	// ������Լ��
	SQL::DataAccessFactory factory;
	auto tabAccounts = factory.Create("Accounts");
	tabAccounts->Open(dbName);

	DataType data;

	// ����Ƿ���ڴ�id���û�
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
