#pragma once

#include <vector>
#include <string>
#include <map>

template<typename T>
using Vector = std::vector<T>;
typedef std::string String;
template<typename K, typename T>
using HashMap = std::map<K, T>;

template <uint64 N>
static FORCEINLINE uint64 GetArraySize(float(&p_Array)[N]) {
	uint64 num = (sizeof(p_Array) / sizeof(p_Array[0]));
	return num;
};