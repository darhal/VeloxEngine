#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <utility>

TRE_NS_START

template<typename FIRST, typename... REST>
struct Tuple : public Tuple<REST...> {

	Tuple(const FIRST& first, REST... rest) : Tuple<REST...>(rest...), first(first) {}
	Tuple(FIRST&& first, REST... rest) : Tuple<REST...>(rest...), first(first) {}

	FIRST first;

	template<int index, typename First, typename... Rest>
	struct GetImpl {
		static auto value(const Tuple<First, Rest...>* t) -> decltype(GetImpl<index - 1, Rest...>::value(t)) {
			return GetImpl<index - 1, Rest...>::value(t);
		}
	};

	template<typename First, typename... Rest>
	struct GetImpl<0, First, Rest...> {
		static First value(const Tuple<First, Rest...>* t) {
			return t->first;
		}
	};

	template<uint32 index, typename First, typename... Rest>
	static auto GetHelper(const Tuple<First, Rest...>& t) -> decltype(GetImpl<index, First, Rest...>::value(&t)) {
		return GetImpl<index, First, Rest...>::value(&t);
	}

	template<uint32 index>
	auto Get() -> decltype(GetHelper<index>(*this))
	{
		return GetHelper<index>(*this);
	}
};

template<typename FIRST>
struct Tuple<FIRST> {
	Tuple(const FIRST& first) : first(first) {}
	Tuple(FIRST&& first) : first(first) {}

	FIRST first;

	template<int index, typename First, typename... Rest>
	struct GetImpl {
		static auto value(const Tuple<First, Rest...>* t) -> decltype(GetImpl<index - 1, Rest...>::value(t)) {
			return GetImpl<index - 1, Rest...>::value(t);
		}
	};

	template<typename First, typename... Rest>
	struct GetImpl<0, First, Rest...> {
		static First value(const Tuple<First, Rest...>* t) {
			return t->first;
		}
	};

	template<uint32 index, typename First, typename... Rest>
	static auto GetHelper(const Tuple<First, Rest...>& t) -> decltype(GetImpl<index, First, Rest...>::value(&t)) {
		return GetImpl<index, First, Rest...>::value(&t);
	}

	template<uint32 index>
	auto Get() -> decltype(GetHelper<index>(*this))
	{
		return GetHelper<index>(*this);
	}
};


TRE_NS_END