#pragma once

#include <type_traits>

#define HAVE_DTOR(T) !std::is_trivially_destructible<T>{} && (std::is_class<T>{} || std::is_union<T>{})
#define NO_DTOR(T) std::is_trivially_destructible<T>::value