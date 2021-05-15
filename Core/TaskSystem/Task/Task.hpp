#pragma once

#include <atomic>
#include "Core/Misc/Defines/Common.hpp"

TRE_NS_START

typedef struct Task Job;
typedef void (*TaskFunction)(class TaskExecutor*, Task*, const void*);

struct Task
{
    TaskFunction m_Function;
    Task* m_Parent;
    ::std::atomic<int32> m_UnfinishedJobs; // atomic
    
	::std::atomic<uint32> m_ContinuationCount;
	Task* m_Continuations[15];

    union {
        char m_Data[64 * 3];
    };
};

TRE_NS_END