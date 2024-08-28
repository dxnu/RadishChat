#pragma once
#include "Friends.h"
#include "CppSQLite3.h"


namespace Radish {
namespace SQL {


class SQLiteFriends : public Friends
{
public:
	SQLiteFriends();
	~SQLiteFriends();

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
