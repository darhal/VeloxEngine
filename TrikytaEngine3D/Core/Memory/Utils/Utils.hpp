#pragma once

#include <Core/Misc/Defines/Common.hpp>

static const usize CalculatePadding(const usize baseAddress, const usize alignment) {
	const usize multiplier = (baseAddress / alignment) + 1;
	const usize alignedAddress = multiplier * alignment;
	const usize padding = alignedAddress - baseAddress;
	return padding;
}

template<typename OBJ_TYPE_TO_BE_ALIGNED>
static const usize CalculatePadding(const usize baseAddress) {
	const usize alignment  = alignof(OBJ_TYPE_TO_BE_ALIGNED);
	const usize multiplier = (baseAddress / alignment) + 1;
	const usize alignedAddress = multiplier * alignment;
	const usize padding = alignedAddress - baseAddress;
	return padding;
}