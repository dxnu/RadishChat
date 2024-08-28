#include "MariaDBAccounts.h"
#include <iostream>
#include <boost/lexical_cast.hpp>



namespace Radish {
namespace SQL {


MariaDBAccounts::MariaDBAccounts()
{
	m_db = mysql_init(nullptr);
}

MariaDBAccounts::~MariaDBAccounts()
{
	mysql_close(m_db);
}

void MariaDBAccounts::Open(const char* dbName)
{
	std::cout << "Mysql Open\n";
	auto ret = mysql_real_connect(m_db, "localhost", "root", "", dbName, 0, nullptr, 0);
	if (ret == nullptr)
		throw std::runtime_error(mysql_error(m_db));
}

void MariaDBAccounts::Create()
{
	std::cout << "MariaDB Create\n";

	std::string sql = "CREATE TABLE IF NOT EXISTS Accounts( \
		account_id VARCHAR(20) PRIMARY KEY, \
		account_name VARCHAR(20) NOT NULL, \
		password_hash CHAR(32) NOT NULL, \
		salt BINARY(8) NOT NULL, \
		email VARCHAR(30) NOT NULL, \
		profile_photo VARCHAR(200), \
		gender TINYINT DEFAULT -1, \
		whats_up VARCHAR(50), \
		signin_date DATETIME NOT NULL \
		)ENGINE=InnoDB DEFAULT CHARSET=UTF8";
	auto ret = mysql_real_query(m_db, sql.c_str(), sql.size());
	if (ret)
		throw std::runtime_error(mysql_error(m_db));
}

void MariaDBAccounts::Insert(DataType& values)
{
	// 组装sql插入语句
	std::string sql("INSERT INTO Accounts(");

	auto pos = values.Begin();
	for (; pos != values.End(); ++pos)
	{
		if (pos == values.End() - 1)
			sql.append(pos->first);
		else
			sql.append(pos->first + ",");
	}
	sql.append(",signin_date) VALUES(");

	pos = values.Begin();
	for (; pos != values.End(); ++pos)
	{
		if (pos == values.End() - 1)
			sql.append("'" + pos->second + "'");
		else
			sql.append("'" + pos->second + "'" + ",");
	}
	sql.append(",now());");

	std::cout << sql << std::endl;

	auto ret = mysql_real_query(m_db, sql.c_str(), sql.size());
	if (ret)
		throw std::runtime_error(mysql_error(m_db));
}

void MariaDBAccounts::Delete(DataType& values)
{
	// 组装删除SQL语句
	std::string sql = "DELETE FROM Accounts WHERE ";
	auto pos = values.Begin();
	for (; pos != values.End(); ++pos)
	{
		if (pos == values.End() - 1)
			sql.append(pos->first + "='" + pos->second + "'");
		else
			sql.append(pos->first + "='" + pos->second + "' AND ");
	}
	sql.append(";");

	std::cout << sql << std::endl;

	auto ret = mysql_real_query(m_db, sql.c_str(), sql.size());
	if (ret)
		throw std::runtime_error(mysql_error(m_db));
}

void MariaDBAccounts::Update(DataType& values)
{
	std::cout << "MariaDB Update\n";

	// 组装更新SQL语句
	std::string sql("UPDATE Accounts SET ");
	auto pos = values.Begin() + 1;
	for (; pos != values.End(); ++pos)
	{
		if (pos == values.End() - 1)
			sql.append(pos->first + "='" + pos->second + "' ");
		else
			sql.append(pos->first + "='" + pos->second + "',");
	}

	pos = values.Begin();
	sql.append("WHERE " + pos->first + "='" + pos->second + "';");

	std::cout << sql << std::endl;

	auto ret = mysql_real_query(m_db, sql.c_str(), sql.size());
	if (ret)
		throw std::runtime_error(mysql_error(m_db));
}

void MariaDBAccounts::Select(DataType& values)
{
	// 组装查询的SQL语句
	std::string sql("SELECT * FROM Accounts WHERE ");
	auto pos = values.Begin();
	for (; pos != values.End(); ++pos)
	{
		if (pos == values.End() - 1)
			sql.append(pos->first + "='" + pos->second + "';");
		else
			sql.append(pos->first + "='" + pos->second + "' AND ");
	}

	std::cout << sql << std::endl;

	auto ret = mysql_real_query(m_db, sql.c_str(), sql.size());
	if (ret)
		throw std::runtime_error(mysql_error(m_db));

	// 清除ContentValues，用以返回结果集
	values.Clear();

	// 获得查询结果集
	MYSQL_RES* result = mysql_store_result(m_db);
	MYSQL_ROW row;
	while (row = mysql_fetch_row(result))
	{
		values.Put("account_id", row[0]);
		values.Put("account_name", row[1]);
		values.Put("password_hash", row[2]);
		values.Put("salt", row[3]);
		values.Put("email", row[4]);
		values.Put("profile_photo", row[5] == nullptr ? "" : row[5]);
		values.Put("gender", boost::lexical_cast<std::string>(row[6]));
		values.Put("whats_up", row[7] == nullptr ? "" : row[7]);
		values.Put("signin_date", row[8]);
	}

	// 遍历
	for (auto i = values.Begin(); i != values.End(); ++i)
	{
		std::cout << i->first << "--" << i->second << std::endl;
	}

	mysql_free_result(result);
}

const Accounts::DataType MariaDBAccounts::SelectAll()
{
	DataType values;

	std::string sql = "SELECT * FROM Accounts";
	auto ret = mysql_real_query(m_db, sql.c_str(), sql.size());
	if (ret)
		throw std::runtime_error(mysql_error(m_db));

	MYSQL_RES* result = mysql_store_result(m_db);
	MYSQL_ROW row;
	while (row = mysql_fetch_row(result))
	{
		values.Put("account_id", row[0]);
		values.Put("account_name", row[1]);
		values.Put("password_hash", row[2]);
		values.Put("salt", row[3]);
		values.Put("email", row[4]);
		values.Put("profile_photo", row[5] == nullptr ? "" : row[5]);
		values.Put("gender", boost::lexical_cast<std::string>(row[6]));
		values.Put("whats_up", row[7] == nullptr ? "" : row[7]);
		values.Put("signin_date", row[8]);
	}

	// 遍历
	for (auto i = values.Begin(); i != values.End(); ++i)
	{
		std::cout << i->first << "--" << i->second << std::endl;
	}

	mysql_free_result(result);

	return values;
}


} // namespace SQL
} // namespace Radish