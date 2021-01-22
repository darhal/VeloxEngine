#pragma once

#include <Renderer/Common.hpp>
#include "Hash.hpp"

TRE_NS_START

namespace Renderer
{
	namespace Utils
	{
		class Hashable
		{
		public:
			FORCEINLINE Hash GetHash() const { return hash; }

			// FORCEINLINE void SetHash(Hash h) { hash = h; }
		protected:
			Hash hash;
		};
	}
}

TRE_NS_END