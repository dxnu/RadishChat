//////////////////////////////////////////////////////////////////////////
// MariaDBAccounts.h
// 使用MariaDB对账号表进行操作
//////////////////////////////////////////////////////////////////////////


#pragma once
#include "Accounts.h"
#include <mysql/mysql.h>
#pragma comment(lib, "libmariadb.lib")


namespace Radish {
namespace SQL {


class MariaDBAccounts : public Accounts
{
public:

	MariaDBAccounts();
	~MariaDBAccounts();

	virtual void Open(const char* dbName) override;

	virtual void Create() override;

	virtual void Insert(DataType& values) override;

	virtual void Delete(DataType& values) override;

	virtual void Update(DataType& values) override;

	virtual void Select(DataType& values) override;

	virtual const DataType SelectAll() override;

private:
	MYSQL* m_db;
};


} // namespace SQL
} // namespace Radish

