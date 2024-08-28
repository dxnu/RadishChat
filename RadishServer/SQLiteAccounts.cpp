#include "SQLiteAccounts.h"
#include <iostream>
#include <boost/lexical_cast.hpp>




namespace Radish {
namespace SQL {


SQLiteAccounts::~SQLiteAccounts()
{
	m_db.close();
}

// ���ļ������ݿ�
void SQLiteAccounts::Open(const char* dbName)
{
	try {
		m_db.open(dbName);  // �����ݿ�
	}
	catch (const CppSQLite3Exception& e)
	{
		throw std::runtime_error(e.errorMessage());
	}
}

// ����Accounts��
void SQLiteAccounts::Create()
{
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
		);";

	try {
		// ��������ڱ����½�Accounts��
		int nRows = m_db.execDML(sql.c_str());
		std::cout << nRows << " rows inserted.\n";
	}
	catch (const CppSQLite3Exception& e)
	{
		throw std::runtime_error(e.errorMessage());
	}
}

void SQLiteAccounts::Insert(DataType& values)
{
	// ��װsql�������
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
	sql.append(",datetime('now','localtime'));");

	//std::cout << sql << std::endl;

	try {
		int rows = m_db.execDML(sql.c_str());
		std::cout << rows << " rows in set\n";
	}
	catch (const CppSQLite3Exception& e)
	{
		throw std::runtime_error(e.errorMessage());
	}
}

void SQLiteAccounts::Delete(DataType& values)
{
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

	//std::cout << sql << std::endl;

	try {
		int rows = m_db.execDML(sql.c_str());
		std::cout << rows << " rows in set\n";
	}
	catch (const CppSQLite3Exception& e)
	{
		throw std::runtime_error(e.errorMessage());
	}
}

void SQLiteAccounts::Update(DataType& values)
{
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

	// ����
	try {
		int rows = m_db.execDML(sql.c_str());
		std::cout << rows << " rows in set\n";
	}
	catch (const CppSQLite3Exception& e)
	{
		throw std::runtime_error(e.errorMessage());
	}
}

void SQLiteAccounts::Select(DataType& values)
{
	// ��װ��ѯ��SQL���
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

	// ���ContentValue�����ؽ����
	values.Clear();

	// ��ѯ
	try {
		CppSQLite3Query query = m_db.execQuery(sql.c_str());

		if (!query.eof())
		{
			values.Put("account_id", query.getStringField("account_id"));
			values.Put("account_name", query.getStringField("account_name"));
			values.Put("password_hash", query.getStringField("password_hash"));
			values.Put("salt", query.getStringField("salt"));
			values.Put("email", query.getStringField("email"));
			values.Put("profile_photo", query.getStringField("profile_photo"));
			values.Put("gender", boost::lexical_cast<std::string>(query.getIntField("gender")));
			values.Put("whats_up", query.getStringField("whats_up"));
			values.Put("signin_date", query.getStringField("signin_date"));
		}
	}
	catch (const CppSQLite3Exception& e)
	{
		throw std::runtime_error(e.errorMessage());
	}

	// �����������ã�
	for (auto i = values.Begin(); i != values.End(); ++i)
	{
		std::cout << i->first << "--" << i->second << std::endl;
	}
}

const Accounts::DataType SQLiteAccounts::SelectAll()
{
	DataType values;

	std::string sql = "SELECT * FROM Accounts";

	try {
		CppSQLite3Query query = m_db.execQuery(sql.c_str());
		while (!query.eof())
		{
			values.Put("account_id", query.getStringField("account_id"));
			values.Put("account_name", query.getStringField("account_name"));
			values.Put("password_hash", query.getStringField("password_hash"));
			values.Put("salt", query.getStringField("salt"));
			values.Put("email", query.getStringField("email"));
			values.Put("profile_photo", query.getStringField("profile_photo"));
			values.Put("gender", boost::lexical_cast<std::string>(query.getIntField("gender")));
			values.Put("whats_up", query.getStringField("whats_up"));
			values.Put("signin_date", query.getStringField("signin_date"));

			query.nextRow();
		}
	}
	catch (const CppSQLite3Exception& e)
	{
		throw std::runtime_error(e.errorMessage());
	}

	// ����
	for (auto i = values.Begin(); i != values.End(); ++i)
	{
		std::cout << i->first << "--" << i->second << std::endl;
	}

	return values;
}


} // namespace SQL
} // namespace Radish
