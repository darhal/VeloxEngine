#pragma once

#include <Renderer/Common.hpp>

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
		private:
			size_t count = 1;
		};

		template <typename T>
		class Handle;

		template<typename T, typename DeleterT = std::default_delete<T>, typename CounterT = SingleThreadRefCounter>
		class RefCounterEnabled
		{
		public:
			using Handle = Handle<T>;
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
			using Handle = Handle<void*>;
			using Base = void;
			using Deleter = void;
			using Counter = void;
			void ReleaseReference() {}
			void AddReference() {}
		};

		template<typename T>
		class Handle
		{
		public:
			Handle() = default;

			explicit Handle(T* handle)
				: data(handle)
			{
			}

			~Handle()
			{
				this->Reset();
			}

			Handle(const Handle& other)
			{
				*this = other;
			}

			Handle(Handle&& other) noexcept
			{
				*this = std::move(other);
			}

			Handle& operator=(const Handle& other)
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

			Handle& operator=(Handle&& other) noexcept
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

			bool operator==(const Handle& other) const
			{
				return data == other.data;
			}

			bool operator!=(const Handle& other) const
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