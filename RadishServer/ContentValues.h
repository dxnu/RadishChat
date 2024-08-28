#pragma once
#include <vector>
#include <list>
#include <utility>



namespace Utils {

#define GET_VALUE_FAILED -1

//////////////////////////////////////////////////////////////////////////
// class template ContentValues
//////////////////////////////////////////////////////////////////////////
template<typename ContentType, typename ValueType>
class ContentValues
{
public:
	using value_type = std::pair<ContentType, ValueType>;
	using assoc_type = std::vector<value_type>;
public:

	template<typename T, typename F>
	void Put(T&& content, F&& value)
	{
		m_association.emplace_back(value_type(std::forward<T>(content), std::forward<F>(value)));
	}


	ValueType Get(const ContentType& c)
	{
		LastError = 0;
		for (auto& elem : m_association)
		{
			if (elem.first == c)
				return elem.second;
		}
		
		LastError = GET_VALUE_FAILED;
		return ValueType();
	}

	void Remove(const ContentType& c)
	{
		for (auto iter = Begin(); iter != End(); ++iter)
		{
			if (iter->first == c)
			{
				m_association.erase(iter);
				return;
			}
		}
	}

	typename assoc_type::const_iterator Begin()
	{
		return m_association.cbegin();
	}

	typename assoc_type::const_iterator End()
	{
		return m_association.cend();
	}

	std::size_t Size() const
	{
		return m_association.size();
	}

	bool Empty() const
	{
		return m_association.empty();
	}

	void Clear()
	{
		m_association.clear();
	}

	value_type operator[](int n)
	{
		return m_association[n];
	}

private:
	assoc_type m_association;
public:
	int LastError = 0;
};


} // namespace Utils

