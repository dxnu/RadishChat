#pragma once
#include <map>


namespace Utils {

template 
<
	typename IdentifierType,
	typename CallbackType
>
class Delegate
{
public:
	using AssocMap = std::map<IdentifierType, CallbackType>;
public:

	template<typename T>
	bool Insert(T&& id, CallbackType callback)
	{
		return associations_.emplace(AssocMap::value_type(
			std::forward<T>(id), callback)).second;
	}

	template<typename T>
	bool Remove(T&& id)
	{
		return associations_.erase(std::forward<T>(id)) == 1;
	}

	template<typename T>
	bool Find(T&& id, typename AssocMap::const_iterator& iter)
	{
		iter = associations_.find(std::forward<T>(id));
		if (iter != associations_.end())
			return true;

		return false;
	}

private:
	AssocMap associations_;
};

} // namespace Utils

