#pragma once
#include "RadishChatDatabaseBase.h"


namespace Radish {
namespace SQL {


//////////////////////////////////////////////////////////////////////////
// Abstract class Accounts
// 对应数据库中的Accounts table
//////////////////////////////////////////////////////////////////////////
class Accounts : public RadishChatDatabaseBase
{
public:

	virtual void Open(const char* dbName) = 0;

	virtual void Create() = 0;

	virtual void Insert(DataType& values) = 0;

	virtual void Delete(DataType& values) = 0;

	virtual void Update(DataType& values) = 0;

	virtual void Select(DataType& values) = 0;

	virtual const DataType SelectAll() = 0;

public:
	int size{9};
	const char* account_id{"account_id"};
	const char* account_name{"account_name"};
	const char* password_hash{"password_hash"};
	const char* salt{"salt"};
	const char* email{"email"};
	const char* profile_photo{"profile_photo"};
	const char* gender{"gender"};
	const char* whats_up{"whats_up"};
};


} // namespace SQL
} // namespace Radish

