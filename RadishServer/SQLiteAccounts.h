//////////////////////////////////////////////////////////////////////////
// SQLiteAccounts.h
// 使用SQLite对账号表进行操作
//////////////////////////////////////////////////////////////////////////


#pragma once
#include "Accounts.h"
#include "CppSQLite3.h"


namespace Radish {
namespace SQL {


class SQLiteAccounts : public Accounts
{
public:

	~SQLiteAccounts();

	virtual void Open(const char* dbName) override;

	virtual void Create() override;

	virtual void Insert(DataType& values) override;

	virtual void Delete(DataType& values) override;

	virtual void Update(DataType& values) override;

	virtual void Select(DataType& values) override;

	virtual const DataType SelectAll() override;


private:
	CppSQLite3DB m_db;
};


} // namespace SQL
} // namespace Radish

