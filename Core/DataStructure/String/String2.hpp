#pragma once

#include <Core/Misc/Defines/Common.hpp>

TRE_NS_START

template<typename T>
class BasicString2
{
public:
	template<usize N>
	BasicString2(const T(&str)[S]);

	~BasicString2();

private:
	CONSTEXPR static const auto SSO_SIZE = 3 * sizeof(usize);

private:
	T* m_Data;

	union
	{
		struct
		{
			usize m_Size;
			usize m_Capacity;
		};

		T m_Buffer[SSO_SIZE];
	};
};

TRE_NS_END