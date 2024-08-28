#pragma once
#include "Friends.h"


namespace Radish {
namespace SQL {


class MariaDBFriends : public Friends
{
public:
	MariaDBFriends();
	~MariaDBFriends();

	virtual void Open(const char* dbName) override;

	virtual void Create() override;

	virtual void Insert(DataType& values) override;

	virtual void Delete(DataType& values) override;

	virtual void Update(DataType& values) override;

	virtual void Select(DataType& values) override;

	virtual const DataType SelectAll() override;

};


} // namespace SQL
}  // namespace Radish
