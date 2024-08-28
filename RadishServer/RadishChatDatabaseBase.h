#pragma once
#include "ContentValues.h"
#include <string>


namespace Radish {
namespace SQL {


class RadishChatDatabaseBase
{
public:
	typedef Utils::ContentValues<std::string, std::string> DataType;

public:

	virtual void Open(const char* dbName) = 0;

	virtual void Create() = 0;

	virtual void Insert(DataType& values) = 0;

	virtual void Delete(DataType& values) = 0;

	virtual void Update(DataType& values) = 0;

	virtual void Select(DataType& values) = 0;

	virtual const DataType SelectAll() = 0;

};


} // namespace SQL
} // namespace Radish

