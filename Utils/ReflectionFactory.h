#pragma once
#include <memory>
#include <map>
#include <functional>
#include <iostream>



namespace Utils
{


template <typename ProductType, typename CreatorType>
class CreatorTypeTraits
{
public:
	typedef ProductType* ResultType;
};

template <typename ProductType>
class CreatorTypeTraits<ProductType, std::function<std::shared_ptr<ProductType>()>>
{
public:
	typedef std::shared_ptr<ProductType>  ResultType;
};

template <typename ProductType>
class CreatorTypeTraits<ProductType, std::function<std::unique_ptr<ProductType>()>>
{
public:
	typedef std::unique_ptr<ProductType> ResultType;
};

//////////////////////////////////////////////////////////////////////////
// class template DefaultFactoryError
// 未有相对应的类型反射时，默认的处理方式
//////////////////////////////////////////////////////////////////////////
template
<
	typename IdentifierType,
	typename ProductType,
	typename CreatorType
>
class DefaultFactoryError
{
	class Exception : public std::exception
	{
	public:
		Exception(const IdentifierType& unknownId)
			: m_unknownId(unknownId)
		{
		}

		virtual char const* what() const override
		{
			return "Unknown object type passed to Factory.";
		}

		const IdentifierType GetId()
		{
			return m_unknownId;
		}

	private:
		IdentifierType m_unknownId;
	};


protected:
	using ResultType = typename CreatorTypeTraits<ProductType, CreatorType>::ResultType;
	ResultType OnUnknownType(const IdentifierType& id)
	{
		throw Exception(id);
	}
};



//////////////////////////////////////////////////////////////////////////
// class template Factory
// 建立型别标识符与类型之间的映射关系
// 藉此可通过型别标识符生成对象
//////////////////////////////////////////////////////////////////////////
template
<
	typename AbstractProduct,
	typename IdentifierType,
	typename ProductCreator = std::function<AbstractProduct*()>/*AbstractProduct*(*)()*/,
	template <typename, typename, typename>
	class FactoryErrorPolicy = DefaultFactoryError
>
class ReflectionFactory
	: public FactoryErrorPolicy<IdentifierType, AbstractProduct, ProductCreator>
{
	using AssocMap = std::map<IdentifierType, ProductCreator>;
public:

	// 注册型别标识符
	template<typename T>
	bool Register(T&& id, ProductCreator creator)
	{
		return m_associations.emplace(AssocMap::value_type(
			std::forward<T>(id), creator)).second;
	}

	// 取消注册型别标识符
	template<typename T>
	bool UnRegister(T&& id)
	{
		return m_associations.erase(std::forward<T>(id)) == 1;
	}

	// 根据型别标识符产生对象
	template<typename T>
	auto CreateObject(T&& id)
	{
		typename AssocMap::const_iterator i = m_associations.find(std::forward<T>(id));
		if (i != m_associations.end())
		{
			return (i->second)();
		}

		return this->OnUnknownType(id);
	}

private:
	AssocMap m_associations;
};


} // namespace Utils