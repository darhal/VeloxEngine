#pragma once

#include <type_traits>

#define HAVE_DTOR(T) (!std::is_trivially_destructible<T>{} && (std::is_class<T>{} || std::is_union<T>{}))
#define NO_DTOR(T) (std::is_trivially_destructible<T>::value)

#define ENABLE_FOR_DTOR(T) typename std::enable_if<HAVE_DTOR(U), int>::type = 0
#define ENABLE_FOR_NON_DTOR(T) typename std::enable_if<NO_DTOR(U), int>::type = 0

#define ENABLE_FOR_POD(T) typename std::enable_if<std::is_pod<T>::value, bool>::type = true
#define ENABLE_FOR_NON_POD(T) typename std::enable_if<!std::is_pod<T>::value, bool>::type = false