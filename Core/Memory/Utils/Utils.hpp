#pragma once

#include <Core/Misc/Defines/Common.hpp>
#include <Core/Misc/Defines/Types.hpp>
#include <utility>
#include <cstring>
#include <new>

FORCEINLINE static usize CalculatePadding(const usize baseAddress, const usize alignment) 
{
	if(alignment == 0)
		return 0;
	
	const usize multiplier = (baseAddress / alignment) + 1;
	const usize alignedAddress = multiplier * alignment;
	const usize padding = alignedAddress - baseAddress;
	return padding;
}

template<typename T>
FORCEINLINE static usize CalculatePadding(const usize baseAddress) 
{
	const usize alignment  = alignof(T);
	const usize multiplier = (baseAddress / alignment) + 1;
	const usize alignedAddress = multiplier * alignment;
	const usize padding = alignedAddress - baseAddress;
	return padding;
}

FORCEINLINE static std::size_t CalculatePaddingWithHeader(const std::size_t baseAddress, const std::size_t alignment, const std::size_t headerSize) 
{
	if(alignment == 0)
		return 0;
	
	std::size_t padding = CalculatePadding(baseAddress, alignment);
	std::size_t neededSpace = headerSize;

	if (padding < neededSpace) {
		neededSpace -= padding; // Header does not fit - Calculate next aligned address that header fits
		// How many alignments I need to fit the header        
		if (neededSpace % alignment > 0) {
			padding += alignment * (1 + (neededSpace / alignment));
		}else{
			padding += alignment * (neededSpace / alignment);
		}
	}

	return padding;
}


template<typename T>
FORCEINLINE static T* Allocate(usize size)
{
	return (T*)(::operator new (sizeof(T) * size));
}

template<typename T>
FORCEINLINE static void Free(T* ptr, usize sz = 1)
{
	if (ptr == NULL) 
		return;

	::operator delete((void*)ptr);
}

template<typename T>
FORCEINLINE static void CopyConstructRange(T* begin, T* end, const T& fillWith)
{
	while (begin != end) {
		new ((void*)begin) T(fillWith);
		begin++;
	}
}

template<typename T, typename... Args>
FORCEINLINE static void ConstructRange(T* begin, T* end, Args&&... args)
{
	while (begin != end) {
		new ((void*)begin) T(std::forward<Args>(args)...);
		begin++;
	}
}

template<typename T>
FORCEINLINE static void MoveRange(T* begin, T* dest, usize num_obj)
{
	memmove(dest, begin, num_obj*sizeof(T));
}

template<typename T, typename std::enable_if<!std::is_pod<T>::value, bool>::type = false>
FORCEINLINE static void Deallocate(T* ptr, usize sz = 1)
{
	//printf("*** NON Pod Deallocate\n");
	if (ptr == NULL) 
		return;

	for (usize i = 0; i < sz; i++) {
		ptr[i].~T();
	}

	::operator delete((void*)ptr);
}

template<typename T, typename std::enable_if<!std::is_pod<T>::value, bool>::type = false>
FORCEINLINE static void CopyRangeTo(T* begin, T* dest, usize len)
{
	usize i = 0;
	while (i < len) {
		new (dest) T(*begin);
		begin++;
		dest++;
		i++;
	}
	//memcpy(dest, begin, len * sizeof(T));
}

template<typename T, typename std::enable_if<!std::is_pod<T>::value, bool>::type = false>
FORCEINLINE static void FillRange(const T& obj, T* dest, usize len)
{
	usize i = 0;
	while (i < len) {
		new (dest) T(obj);
		dest++;
		i++;
	}
	//memcpy(dest, begin, len * sizeof(T));
}

template<typename T, typename std::enable_if<!std::is_pod<T>::value, bool>::type = false>
FORCEINLINE static void MoveRangeTo(T* begin, T* dest, usize len)
{
	usize i = 0;
	while (i < len) {
		new (dest) T(std::move(*begin));
		begin++;
		dest++;
		i++;
	}
}

template<typename T, typename std::enable_if<!std::is_pod<T>::value, bool>::type = false>
FORCEINLINE static void DeleteRange(T* begin, T* end)
{
	//printf("*** NON Pod DELETE\n");
	while (begin != end) {
		begin->~T();
		begin++;
	}
}

template<typename T, typename std::enable_if<!std::is_pod<T>::value, bool>::type = false>
FORCEINLINE static void DefaultConstructRange(T* begin, T* end)
{
	while (begin != end) {
		new ((void*)begin) T();
		begin++;
	}
}

template<typename T, typename std::enable_if<!std::is_pod<T>::value, bool>::type = false>
FORCEINLINE static void DestroyObjects(T* ptr, usize sz = 0)
{
	//printf("*** NON Pod Deallocate\n");
	if (ptr == NULL) return;
	for (usize i = 0; i < sz; i++) {
		ptr[i].~T();
	}
}


/**********FOR POD TYPES********/


template<typename T, typename std::enable_if<std::is_pod<T>::value, bool>::type = true>
FORCEINLINE static void CopyRangeTo(T* begin, T* dest, usize len)
{
	memcpy(dest, begin, len * sizeof(T));
	//printf("Pod Copy\n");
}

template<typename T, typename std::enable_if<std::is_pod<T>::value, bool>::type = true>
FORCEINLINE static void FillRange(const T& obj, T* dest, usize len)
{
	usize i = 0;
	while (i < len) {
		dest[i] = obj;
		dest++;
		i++;
	}
	//printf("Pod Copy\n");
}

template<typename T, typename std::enable_if<std::is_pod<T>::value, bool>::type = true>
FORCEINLINE static void MoveRangeTo(T* begin, T* dest, usize len)
{
	memcpy(dest, begin, len * sizeof(T));
}

template<typename T, typename std::enable_if<std::is_pod<T>::value, bool>::type = true>
FORCEINLINE static void DeleteRange(T* begin, T* end)
{
}

template<typename T, typename std::enable_if<std::is_pod<T>::value, bool>::type = true>
FORCEINLINE static void DefaultConstructRange(T* begin, T* end)
{
	while (begin != end) {
		new ((void*)begin) T;
		begin++;
	}
	//printf("Pod Construct\n");
}

template<typename T, typename std::enable_if<std::is_pod<T>::value, bool>::type = true>
FORCEINLINE static void Deallocate(T* ptr, usize sz = 1)
{
	::operator delete((void*)ptr);

	//printf("Pod Deallocate\n");
}

template<typename T, typename std::enable_if<std::is_pod<T>::value, bool>::type = true>
FORCEINLINE static void DestroyObjects(T* ptr, usize sz = 0)
{
	return;
}

template<typename T, typename std::enable_if<HAVE_DTOR(T), int>::type = 0>
FORCEINLINE static void Destroy(T* obj)
{
	obj->T::~T();
}

template<typename T, typename std::enable_if<NO_DTOR(T), int>::type = 0>
FORCEINLINE static void Destroy(T* obj)
{
	// Do nothing
}
