#pragma once


namespace Utils {

template <typename T>
class Singleton
{
public:
	Singleton() = delete;
	~Singleton() = delete;
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

	static T& Instance()
	{
		static T obj;
		return obj;
	}
};

} // namespace Utils

