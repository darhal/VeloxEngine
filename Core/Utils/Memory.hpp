#pragma once

#include <type_traits>
#include <cstring>
#include <utility>
#include <new>
#include <algorithm>
#include <Core/Misc/Defines/Common.hpp>

namespace TRE::Utils
{
	template <typename integral>
	constexpr bool IsAligned(integral x, usize a) noexcept
	{
		return (x & (integral(a) - 1)) == 0;
	}

	template <typename integral>
	constexpr integral AlignUp(integral x, usize a) noexcept
	{
		return integral((x + (integral(a) - 1)) & ~integral(a - 1));
	}

	template <typename integral>
	constexpr integral AlignDown(integral x, usize a) noexcept
	{
		return integral(x & ~integral(a - 1));
	}

	FORCEINLINE void* AllocateBytes(usize sz, usize al)
	{
		return ::operator new(sz, static_cast<std::align_val_t>(al));
	}

	FORCEINLINE void* AllocateBytes(usize sz)
	{
		return ::operator new(sz);
	}

	template<typename T>
	FORCEINLINE T* Allocate(usize sz)
	{
		// return static_cast<T*>(::operator new(sz * sizeof(T), static_cast<std::align_val_t>(alignof(T))));
		return static_cast<T*>(::operator new(sz * sizeof(T)));
	}

	FORCEINLINE void FreeMemory(void* ptr) noexcept
	{
		if (ptr)
			::operator delete(ptr);
		//delete ptr;
	}

#if __cplusplus >= 202002L
	template<typename T>
	concept POD = (std::is_standard_layout_v<T> && std::is_trivial_v<T>) || std::is_trivially_copyable_v<T> || std::is_trivially_destructible_v<T>;
	// #define POD typename

	// POD TYPES:
	template<POD T>
	FORCEINLINE void Copy(T* dst, const T* src, usize count = 1) noexcept
	{
		std::memcpy(dst, src, count * sizeof(T));
	}

	template<POD T>
	FORCEINLINE void CopyConstruct(T* dst, const T* src, usize count = 1) noexcept
	{
		Utils::Copy(dst, src, count);
	}

	template<POD T>
	FORCEINLINE void Move(T* dst, T* src, usize count = 1) noexcept
	{
		Utils::Copy(dst, src, count);
	}

	template<POD T>
	FORCEINLINE void MoveForward(T* dst, T* src, ssize start, ssize end) noexcept
	{
		Utils::Copy(dst + start, src + end, end - start);
	}

	template<POD T>
	FORCEINLINE void MoveBackward(T* dst, T* src, ssize start, ssize end) noexcept
	{
		std::memmove(dst + end, src + end, (start - end + 1) * sizeof(T));
	}

	template<POD T>
	FORCEINLINE void MoveConstruct(T* dst, T* src, usize count = 1) noexcept
	{
		Utils::Copy(dst, src, count);
	}

	template<POD T>
	FORCEINLINE void MoveConstructForward(T* dst, T* src, ssize start, ssize end) noexcept
	{
		Utils::Copy(dst + start, src + start, end - start);
	}

	template<POD T>
	FORCEINLINE void MoveConstructBackward(T* dst, T* src, ssize start, ssize end) noexcept
	{
		Utils::MoveBackward(dst, src, start, end);
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
		Utils::FreeMemory(ptr);
	}
#endif

// #if 0
	// NON POD TYPES :
	template<typename T>
	FORCEINLINE void Copy(T* dst, const T* src, usize count = 1) noexcept
	{
		for (usize i = 0; i < count; i++) {
			dst[i] = T(src[i]);
		}
	}

	template<typename T>
	FORCEINLINE void CopyConstruct(T* dst, const T* src, usize count = 1) noexcept
	{
		for (usize i = 0; i < count; i++) {
			new (&dst[i]) T(src[i]);
		}
	}

	template<typename T>
	FORCEINLINE void Move(T* dst, T* src, usize count = 1) noexcept
	{
		for (usize i = 0; i < count; i++) {
			dst[i] = T(std::move(src[i]));
			src[i].~T();
		}
	}

	template<typename T>
	FORCEINLINE void MoveForward(T* dst, T* src, ssize start, ssize end) noexcept
	{
		for (ssize i = start; i < end; i++) {
			dst[i] = T(std::move(src[i]));
			src[i].~T();
		}
	}

	template<typename T>
	FORCEINLINE void MoveBackward(T* dst, T* src, ssize start, ssize end) noexcept
	{
		for (ssize i = start; i >= end; i--) {
			dst[i] = T(std::move(src[i]));
			src[i].~T();
		}
	}

	template<typename T>
	FORCEINLINE void MoveConstruct(T* dst, T* src, usize count = 1) noexcept
	{
		for (usize i = 0; i < count; i++) {
			new (&dst[i]) T(std::move(src[i]));
		}
	}

	template<typename T>
	FORCEINLINE void MoveConstructForward(T* dst, T* src, ssize start, ssize end) noexcept
	{
		for (ssize i = start; i < end; i++) {
			new (&dst[i]) T(std::move(src[i]));
		}
	}

	template<typename T>
	FORCEINLINE void MoveConstructBackward(T* dst, T* src, ssize start, ssize end) noexcept
	{
		for (ssize i = start; i >= end; i--) {
			new (&dst[i]) T(std::move(src[i]));
		}
	}

	template<typename T>
	FORCEINLINE void MemSet(T* dst, const T& src, usize count = 1) noexcept
	{
		for (usize i = 0; i < count; i++) {
			dst[i] = T(src);
		}
	}

	template<typename T>
	FORCEINLINE void MemSet(T* dst, T&& src, usize count = 1) noexcept
	{
		for (usize i = 0; i < count; i++) {
			dst[i] = T(std::forward(src));
		}
	}

	template<typename T>
	FORCEINLINE void Destroy(T* ptr, usize count = 1) noexcept
	{
		for (usize i = 0; i < count; i++) {
			ptr[i].~T();
		}
	}

	template<typename T>
	FORCEINLINE void Free(T* ptr, usize count = 1) noexcept
	{
		Utils::Destroy(ptr, count);
		Utils::FreeMemory(ptr);
	}
// #endif
}