#pragma once

#include <vector>
#include <string>
#include <map>

template<typename T>
using Vector = std::vector<T>;
typedef std::string String;
template<typename K, typename T>
using HashMap = std::map<K, T>;

template <usize N>
static FORCEINLINE usize GetArraySize(float(&p_Array)[N]) {
	usize num = (sizeof(p_Array) / sizeof(p_Array[0]));
	return num;
}