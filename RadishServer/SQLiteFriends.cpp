#include "SQLiteFriends.h"
#include <iostream>


namespace Radish {
namespace SQL {


SQLiteFriends::SQLiteFriends()
{

}

SQLiteFriends::~SQLiteFriends()
{
	m_db.close();
}

void SQLiteFriends::Open(const char* dbName)
{
	try {
		m_db.open(dbName);
	}
	catch (const CppSQLite3Exception& e)
	{
		throw std::runtime_error(e.errorMessage());
	}
}

void SQLiteFriends::Create()
{
	std::string sql = "CREATE TABLE IF NOT EXISTS Friends( \
		account_id VARCHAR(20) NOT NULL, \
		friend_id VARCHAR(20) NOT NULL, \
		PRIMARY KEY (account_id, friend_id) \
		);";

	try {
		m_db.execDML(sql.c_str());
	}
	catch (const CppSQLite3Exception& e)
	{
		throw std::runtime_error(e.errorMessage());
	}
}

void SQLiteFriends::Insert(DataType& values)
{
	// ������Լ��
	for (auto i = values.Begin(); i != values.End(); ++i)
	{
		auto ret = CheckReferences("radish_chat.db", i->second);
		if (!ret)
			throw std::runtime_error("�������û�" + i->second);
	}

	// ��װ�����SQL���
	std::string sql = "INSERT INTO Friends(";
	auto cpos = values.Begin();
	for (; cpos != values.End(); ++cpos)
	{
		if (cpos == values.End() - 1)
			sql.append(cpos->first);
		else
			sql.append(cpos->first + ",");
	}
	sql.append(") VALUES(");
	cpos = values.Begin();
	for (; cpos != values.End(); ++cpos)
	{
		if (cpos == values.End() - 1)
			sql.append("'" + cpos->second + "'");
		else
			sql.append("'" + cpos->second + "',");
	}
	sql.append(");");

	std::cout << sql << std::endl;

	try {
		m_db.execDML(sql.c_str());
	}
	catch (const CppSQLite3Exception& e)
	{
		throw std::runtime_error(e.errorMessage());
	}
}

void SQLiteFriends::Delete(DataType& values)
{
	if (values.Size() != 2)
		throw std::runtime_error("ɾ��������Ҫ����account_id,friend_id.");

	// ������Լ��
	for (auto i = values.Begin(); i != values.End(); ++i)
	{
		auto ret = CheckReferences("radish_chat.db", i->second);
		if (!ret)
			throw std::runtime_error("�������û�" + i->second);
	}

	// ����ɾ��sql���
	std::string sql = "DELETE FROM Friends WHERE ";
	auto cpos = values.Begin();
	for (; cpos != values.End(); ++cpos)
	{
		if (cpos == values.End() - 1)
			sql.append(cpos->first + "='" + cpos->second + "';");
		else
			sql.append(cpos->first + "='" + cpos->second + "' AND ");
	}

	std::cout << sql << std::endl;

	try {
		int nRows = m_db.execDML(sql.c_str());
	}
	catch (const CppSQLite3Exception& e)
	{
		throw std::runtime_error(e.errorMessage());
	}
}

void SQLiteFriends::Update(DataType& values)
{
	throw std::runtime_error("Can't update Friends table.");
}

void SQLiteFriends::Select(DataType& values)
{
	// ������Լ��
	for (auto i = values.Begin(); i != values.End(); ++i)
	{
		auto ret = CheckReferences("radish_chat.db", i->second);
		if (!ret)
			throw std::runtime_error("�������û�" + i->second);
	}

	std::string sql = "SELECT * FROM Friends WHERE ";
	auto cpos = values.Begin();
	sql.append(cpos->first + "='" + cpos->second + "';");

	std::cout << sql << std::endl;

	// ���ContentValue�����ؽ����
	values.Clear();

	try {
		CppSQLite3Query query = m_db.execQuery(sql.c_str());
		if (!query.eof())
		{
			values.Put("account_id", query.getStringField("account_id"));
			values.Put("friend_id", query.getStringField("friend_id"));
		}
	}
	catch (const CppSQLite3Exception& e)
	{
		throw std::runtime_error(e.errorMessage());
	}

	// ���� ������
	for (auto i = values.Begin(); i != values.End(); ++i)
	{
		std::cout << i->first << "--" << i->second << std::endl;
	}
}

const Friends::DataType SQLiteFriends::SelectAll()
{
	DataType values;

	std::string sql = "SELECT * FROM Friends";
	try {
		CppSQLite3Query query = m_db.execQuery(sql.c_str());
		while (!query.eof())
		{
			values.Put("account_id", query.getStringField("account_id"));
			values.Put("friend_id", query.getStringField("friend_id"));

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
