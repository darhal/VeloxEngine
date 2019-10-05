#pragma once

#include "Core/Misc/Defines/Common.hpp"
#include <atomic>

TRE_NS_START

typedef struct Task Job;
typedef void (*TaskFunction)(Task*, const void*);

struct Task
{
    TaskFunction m_Function;
    Task* m_Parent;
    std::atomic<int32> m_UnfinishedJobs; // atomic
    uint8 m_WorkerID;
    
    union {
        int8* m_Padding;
        void* m_Data;
    };
 
    //FORCEINLINE static Task* CreateTask(TaskFunction func, uint32 aux_mem = 0);

    //FORCEINLINE static Task* CreateTaskAsChild(Task* parent, TaskFunction func, uint32 aux_mem = 0);
};

TRE_NS_END