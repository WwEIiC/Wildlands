#pragma once
#include "Wildlands/Core/Base.h"

#define SINGLETON(T) friend class Wildlands::Singleton<T>;\
protected:\
	T() = default;\
	virtual ~T() override = default;

namespace Wildlands
{
	template<typename T>
	class  Singleton
	{
	public:
		static T& Get()
		{
			static T instance;
			return instance;
		}
	protected:
		Singleton() = default;
		virtual ~Singleton() = default;

		Singleton(const Singleton&) = delete;
		Singleton& operator=(const Singleton&) = delete;
		Singleton(Singleton&&) = delete;
		Singleton& operator=(Singleton&&) = delete;
	};
}
