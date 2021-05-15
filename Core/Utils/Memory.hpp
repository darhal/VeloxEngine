#pragma once

#include <type_traits>
#include <cstring>
#include <utility>
#include <Core/Misc/Defines/Common.hpp>

namespace TRE::Utils
{
	FORCEINLINE void* AllocateBytes(usize sz)
	{
		return ::operator new(sz);
	}

	template<typename T>
	FORCEINLINE T* Allocate(usize sz)
	{
		if (sz == 0)
			return nullptr;

		return static_cast<T*>(::operator new(sz * sizeof(T)));
	}

	FORCEINLINE void FreeMemory(void* ptr)
	{
		if (ptr)
			::operator delete(ptr);
	}

#if __cplusplus >= 202002L
	template<typename T>
	concept POD = (::std::is_standard_layout_v<T> && ::std::is_trivial_v<T>) || ::std::is_trivially_copyable_v<T> || ::std::is_trivially_destructible_v<T>;

	// POD TYPES:
	template<POD T>
	FORCEINLINE void Copy(T* dst, const T* src, usize count = 1) noexcept
	{
		::std::memcpy(dst, src, count * sizeof(T));
	}

	template<POD T>
	FORCEINLINE void CopyConstruct(T* dst, const T* src, usize count = 1) noexcept
	{
		Copy(dst, src, count);
	}

	template<POD T>
	FORCEINLINE void Move(T* dst, T* src, usize count = 1) noexcept
	{
		Copy(dst, src, count);
	}

	template<POD T>
	FORCEINLINE void MoveConstruct(T* dst, T* src, usize count = 1) noexcept
	{
		Copy(dst, src, count);
	}

	template<POD T>
	FORCEINLINE void MemSet(T* dst, const T& src, usize count = 1) noexcept
	{
		for (usize i = 0; i < count; i++) {
			dst[i] = src;
		}
	}

	template<POD T>
	FORCEINLINE void MemSet(T* dst, T&& src, usize count = 1) noexcept
	{
		for (usize i = 0; i < count; i++) {
			dst[i] = src;
		}
	}

	template<POD T>
	FORCEINLINE void Destroy(T* ptr, usize count = 1) noexcept
	{
	}

	template<POD T>
	FORCEINLINE void Free(T* ptr, usize count = 1) noexcept
	{
		FreeMemory(ptr);
	}
#endif

	// NON POD TYPES :
	template<typename T>
	FORCEINLINE void Copy(T* dst, const T* src, usize count = 1)
	{
		for (usize i = 0; i < count; i++) {
			dst[i] = T(src[i]);
		}
	}

	template<typename T>
	FORCEINLINE void CopyConstruct(T* dst, const T* src, usize count = 1)
	{
		for (usize i = 0; i < count; i++) {
			new (&dst[i]) T(src[i]);
		}
	}

	template<typename T>
	FORCEINLINE void Move(T* dst, T* src, usize count = 1)
	{
		for (usize i = 0; i < count; i++) {
			dst[i] = T(::std::move(src[i]));
		}
	}

	template<typename T>
	FORCEINLINE void MoveConstruct(T* dst, T* src, usize count = 1)
	{
		for (usize i = 0; i < count; i++) {
			new (&dst[i]) T(::std::move(src[i]));
		}
	}

	template<typename T>
	FORCEINLINE void MemSet(T* dst, const T& src, usize count = 1)
	{
		for (usize i = 0; i < count; i++) {
			dst[i] = T(src);
		}
	}

	template<typename T>
	FORCEINLINE void MemSet(T* dst, T&& src, usize count = 1)
	{
		for (usize i = 0; i < count; i++) {
			dst[i] = T(::std::forward(src));
		}
	}

	template<typename T>
	FORCEINLINE void Destroy(T* ptr, usize count = 1)
	{
		for (usize i = 0; i < count; i++) {
			ptr[i].~T();
		}
	}

	template<typename T>
	FORCEINLINE void Free(T* ptr, usize count = 1)
	{
		Utils::Destroy(ptr, count);
		Utils::FreeMemory(ptr);
	}
}