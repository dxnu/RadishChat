#pragma once
#include "RadishChatDatabaseBase.h"


namespace Radish {
namespace SQL {


////////////////////////////////////////////////////////////////////////////////
// Abstract class Friends
// 对应数据库中的Friends table
////////////////////////////////////////////////////////////////////////////////
class Friends : public RadishChatDatabaseBase
{
public:

	virtual void Open(const char* dbName) = 0;

	virtual void Create() = 0;

	virtual void Insert(DataType& values) = 0;

	virtual void Delete(DataType& values) = 0;

	virtual void Update(DataType& values) = 0;

	virtual void Select(DataType& values) = 0;

	virtual const DataType SelectAll() = 0;

protected:

	virtual bool CheckReferences(const char* dbName, const std::string& id);
};


} // namespace SQL
} // namespace Radish
