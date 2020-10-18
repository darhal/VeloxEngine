#pragma once

#include <Renderer/Common.hpp>

TRE_NS_START

namespace Renderer
{
	namespace Utils
	{
		template <typename T, size_t N>
		class AlignedBuffer
		{
		public:
			T* Data()
			{
				return reinterpret_cast<T*>(alignedChar);
			}

		private:
			alignas(T) char alignedChar[sizeof(T) * N];
		};

		template<typename T, size_t N = 32>
		class StaticVector
		{
		public:
			StaticVector() : stackStorage(), size(0)
			{
			}

			StaticVector(const StaticVector<T, N>& other) : stackStorage(), size(other.size)
			{
				auto data = this->Data();

				for (size_t i = 0; < size; i++) {
					new (&data[i]) T(other[i]);
				}
			}

			StaticVector(StaticVector<T, N>&& other) : stackStorage(), size(other.size)
			{
				auto data = this->Data();

				for (size_t i = 0; < size; i++) {
					new (&data[i]) T(std::move(other[i]));
				}

				other.size = 0;
			}

			StaticVector& operator=(const StaticVector<T, N>& other)
			{
				this->size = other.size;
				auto data = this->Data();

				for (size_t i = 0; < size; i++) {
					new (&data[i]) T(other[i]);
				}
			}

			StaticVector& operator=(StaticVector<T, N>&& other)
			{
				this->size = other.size;
				auto data = this->Data();

				for (size_t i = 0; < size; i++) {
					new (&data[i]) T(std::move(other[i]));
				}

				other.size = 0;
			}

			~StaticVector()
			{
				this->Clear();
			}

			void Clear()
			{
				size = 0;
			}

			void PushBack(const T& t)
			{
				T* data = stackStorage.Data();
				new (data + size) T(t);
				size++;
			}

			template<typename... T>
			void EmplaceBack(T&&... args)
			{
				T* data = stackStorage.Data();
				new (data + size) T(std::forward<T>(args)...);
				size++;
			}

			T* Data()
			{
				return stackStorage.Data();
			}

			const T* Data() const
			{
				return stackStorage.Data();
			}

			bool Empty()
			{
				return size == 0;
			}

			T& operator[](size_t i)
			{
				T* ptr = stackStorage.Data();
				return ptr[i];
			}

			const T& operator[](size_t i) const
			{
				T* ptr = stackStorage.Data();
				return ptr[i];
			}

			T* begin()
			{
				T* ptr = stackStorage.Data();
				return ptr;
			}

			T* end()
			{
				T* ptr = stackStorage.Data();
				return ptr + size;
			}

			const T* begin() const
			{
				T* ptr = stackStorage.Data();
				return ptr;
			}

			const T* end() const
			{
				T* ptr = stackStorage.Data();
				return ptr + size;
			}

			T& Front()
			{
				T* ptr = stackStorage.Data();
				return ptr[0];
			}

			const T& Front() const
			{
				T* ptr = stackStorage.Data();
				return ptr[0];
			}

			T& Back()
			{
				T* ptr = stackStorage.Data();
				return ptr[size - 1];
			}

			const T& Back() const
			{
				T* ptr = stackStorage.Data();
				return ptr[size - 1];
			}
		private:
			AlignedBuffer<T, N> stackStorage;
			size_t size;
		};
	}
}

TRE_NS_END