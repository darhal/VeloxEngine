#pragma once

#include <Core/Misc/Defines/Common.hpp>

TRE_NS_START

template<typename First, typename... Rest>
struct Tuple : public Tuple<Rest...> {

	Tuple(const First& first, Rest... rest) : Tuple<Rest...>(rest...), first(first) {}
	Tuple(First&& first, Rest... rest) : Tuple<Rest...>(rest...), first(first) {}

	First first;

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

template<typename First>
struct Tuple<First> {
	Tuple(const First& first) : first(first) {}
	Tuple(First&& first) : first(first) {}

	First first;

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