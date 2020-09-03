#pragma once

#include <Renderer/Common.hpp>

TRE_NS_START

namespace Renderer
{
	namespace Utils
	{
		template<typename T>
		class Handle
		{
		public:
			Handle() = default;

			explicit Handle(T* handle)
				: data(handle)
			{
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
		private:
			T* data = NULL;
		};
	}
}

TRE_NS_END