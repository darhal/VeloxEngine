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

static const std::size_t CalculatePaddingWithHeader(const std::size_t baseAddress, const std::size_t alignment, const std::size_t headerSize) {
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