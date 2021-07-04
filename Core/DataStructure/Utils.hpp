#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/DataStructure/Bitset.hpp>
#include <Core/DataStructure/String.hpp>

TRE_NS_START

// Compile time counter resources: http://b.atch.se/posts/constexpr-counter/
/*namespace Detail
{
#if defined(COMPILER_MSVC)
	template<int N>
	struct flag
	{
		friend CONSTEXPR int adl_flag(flag<N>);
	};
	template<int N>
	struct writer
	{
		friend CONSTEXPR int adl_flag(flag<N>)
		{
			return N;
		}

		static CONSTEXPR int value = N;
	};
	template<int N, class = char[noexcept(adl_flag(flag<N>())) ? +1 : -1]>
	int CONSTEXPR reader(int, flag<N>)
	{
		return N;
	}
	template<int N>
	int CONSTEXPR reader(float, flag<N>, int R = reader(0, flag<N - 1>()))
	{
		return R;
	}
	int CONSTEXPR reader(float, flag<0>)
	{
		return 0;
	}

#else
	template<uint32 N>
	struct flag
	{
		friend constexpr uint32 adl_flag(flag<N>);
	};
	template<uint32 N>
	struct writer
	{
		friend constexpr uint32 adl_flag(flag<N>)
		{
			return N;
		}

		static constexpr uint32 value = N;
	};
	template<uint32 N, uint32 = adl_flag(flag<N> {}) >
	uint32 constexpr reader(uint32, flag<N>)
	{
		return N;
	}

	template<uint32 N>
	uint32 constexpr reader(float, flag<N>, uint32 R = reader(0, flag<N - 1> {}))
	{
		return R;
	}

	uint32 constexpr reader(float, flag<0>)
	{
		return 0;
	}

#endif
}*/

namespace Helper
{
	FORCEINLINE static String ToString(const Bitset& bitset)
	{
		uint32 len = (uint32) bitset.Length();
		String res(len + 1);

		for (uint32 i = 0; i < len; i++) {
			res[i] = (bitset[i] ? '1' : '0');
		}

		return res;
	}


/*#if defined(COMPILER_MSVC)
	template<int N = 1, int C = Detail::reader(0, Detail::flag<32>())>
	uint32 CONSTEXPR static NextID(int R = Detail::writer<C + N>::value)
	{
		return (uint32)(R - 1);
	}

	template<int C = Detail::reader(0, Detail::flag<32>())>
	uint32 CONSTEXPR static GetLastID(int R = Detail::writer<C>::value)
	{
		return (uint32)(R - 1);
	}
#else
	template<uint32 N = 0>
	static uint32 constexpr NextID(uint32 R = Detail::writer < Detail::reader(0, Detail::flag<32> {}) + N >::value)
	{
		return R;
	}
#endif*/

};

TRE_NS_END
