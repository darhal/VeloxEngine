#pragma once

#include <Renderer/Backend/Common.hpp>

TRE_NS_START

namespace Renderer
{
	namespace Utils
	{
		class SingleThreadRefCounter
		{
		public:
			FORCEINLINE void AddRef()
			{
				count++;
			}

			FORCEINLINE bool Release()
			{
				return --count == 0;
			}

			FORCEINLINE size_t Get() const
			{
				return count;
			}
		private:
			size_t count = 1;
		};

		template <typename T>
		class ObjectHandle;

		template<typename T, typename DeleterT = std::default_delete<T>, typename CounterT = SingleThreadRefCounter>
		class RefCounterEnabled
		{
		public:
			using Handle = ObjectHandle<T>;
			using Base = T;
			using Deleter = DeleterT;
			using Counter = CounterT;

			void ReleaseReference()
			{
				if (counter.Release()) {
					Deleter()(static_cast<T*>(this));
				}
			}

			void AddReference()
			{
				counter.AddRef();
			}

			size_t GetCounter() const
			{
				return counter.Get();
			}

			RefCounterEnabled() = default;

			RefCounterEnabled(const RefCounterEnabled&) = delete;

			void operator=(const RefCounterEnabled&) = delete;
		private:
			Counter counter;
		};

		template<>
		class RefCounterEnabled<void, void, void>
		{
		public:
			using Handle = ObjectHandle<void*>;
			using Base = void;
			using Deleter = void;
			using Counter = void;
			void ReleaseReference() {}
			void AddReference() {}
			size_t GetCounter() const { return 0; }
		};

		template<typename T>
		class ObjectHandle
		{
		public:
			ObjectHandle() = default;

			explicit ObjectHandle(T* handle)
				: data(handle)
			{
			}

			~ObjectHandle()
			{
				this->Reset();
			}

			ObjectHandle(const ObjectHandle& other)
			{
				*this = other;
			}

			ObjectHandle(ObjectHandle&& other) noexcept
			{
				*this = std::move(other);
			}

			ObjectHandle& operator=(const ObjectHandle& other)
			{
				using ReferenceBase = RefCounterEnabled<typename T::Base, typename T::Deleter, typename T::Counter>;

				if (this != &other) {
					this->Reset(); // Release any old data held
					data = other.data;
					if (data)
						static_cast<ReferenceBase*>(data)->AddReference();
				}

				return *this;
			}

			ObjectHandle& operator=(ObjectHandle&& other) noexcept
			{
				if (this != &other) {
					this->Reset();  // Release any old data held
					data = other.data;
					other.data = NULL;
				}
				return *this;
			}

			void Reset()
			{
				using ReferenceBase = RefCounterEnabled<typename T::Base, typename T::Deleter, typename T::Counter>;

				if (data) {
					static_cast<ReferenceBase*>(data)->ReleaseReference();
					data = NULL;
				}
			}

			T& operator*()
			{
				return *data;
			}

			const T& operator*() const
			{
				return *data;
			}

			T* operator->()
			{
				return data;
			}

			const T* operator->() const
			{
				return data;
			}

			explicit operator bool() const
			{
				return data != NULL;
			}

			bool operator==(const ObjectHandle& other) const
			{
				return data == other.data;
			}

			bool operator!=(const ObjectHandle& other) const
			{
				return data != other.data;
			}

			T* Get()
			{
				return data;
			}

			const T* Get() const
			{
				return data;
			}

			operator T*() const
			{
				return data;
			}

			operator T*()
			{
				return data;
			}
		private:
			T* data = NULL;
		};
	}
}

TRE_NS_END
