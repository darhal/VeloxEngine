#pragma once

#include <Legacy/Misc/Defines/Common.hpp>

TRE_NS_START

template<typename T>
using ParallelTaskFunction = void(*)(TaskExecutor*, Task*, T*, uint32);

template<typename T, typename S>
struct ParallelForTaskData
{
	// assert if splitter have bigger size than uint32 (4 bytes)
	static_assert(sizeof(S) <= sizeof(uint32), "Size of the splitter must be equal or less than sizeof(uint32) (4 bytes)");

	typedef T DataType;
	typedef S SplitterType;

	ParallelForTaskData(DataType* data, uint32 count, ParallelTaskFunction<T> function, const SplitterType& splitter)
		: data(data), function(function), count(count), splitter(splitter)
	{
	}

	DataType* data;
	ParallelTaskFunction<T> function;
	uint32 count;
	SplitterType splitter;
};

TRE_NS_END