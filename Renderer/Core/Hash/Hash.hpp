#pragma once

#include <Renderer/Common.hpp>

TRE_NS_START

namespace Renderer
{
	namespace Utils
	{
		using Hash = uint64;

		// TODO: put this some where where it fits
		template<typename T>
		FORCEINLINE static Hash Data(const T* data_, size_t size)
		{
			Hash h = 0xcbf29ce484222325ull;
			size /= sizeof(*data_);
			for (size_t i = 0; i < size; i++)
				h = (h * 0x100000001b3ull) ^ data_[i];
			return h;
		}

		class Hasher
		{
		public:
			explicit Hasher(Hash h);

			explicit Hasher() : h(0xcbf29ce484222325ull) {}

			template<typename T>
			FORCEINLINE void Data(const T* data_, size_t size)
			{
				size /= sizeof(*data_);
				for (size_t i = 0; i < size; i++)
					h = (h * 0x100000001b3ull) ^ data_[i];
			}

			FORCEINLINE void u32(uint32_t value)
			{
				h = (h * 0x100000001b3ull) ^ value;
			}

			FORCEINLINE void s32(int32_t value)
			{
				u32(uint32_t(value));
			}

			FORCEINLINE void f32(float value)
			{
				union
				{
					float f32;
					uint32_t u32;
				} u;
				u.f32 = value;
				u32(u.u32);
			}

			FORCEINLINE void u64(uint64_t value)
			{
				u32(value & 0xffffffffu);
				u32(value >> 32);
			}

			template<typename T>
			FORCEINLINE void Pointer(T* ptr)
			{
				u64(reinterpret_cast<uintptr_t>(ptr));
			}

			FORCEINLINE void String(const char* str)
			{
				char c;
				u32(0xff);
				while ((c = *str++) != '\0')
					u32(uint8_t(c));
			}

			FORCEINLINE Hash Get() const
			{
				return h;
			}
		private:
			Hash h;
		};
	}
}

TRE_NS_END