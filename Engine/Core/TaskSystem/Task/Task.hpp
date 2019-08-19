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
 

    FORCEINLINE static Task* CreateTask(TaskFunction func);

    FORCEINLINE static Task* CreateTaskAsChild(Task* parent, TaskFunction func);
};

FORCEINLINE Task* Task::CreateTask(TaskFunction func)
{
    Task* task = (Task*) operator new(sizeof(Task));
    task->m_Function = func;
    task->m_Parent = nullptr;
    task->m_UnfinishedJobs = 1;
 
    return task;
}

FORCEINLINE Task* Task::CreateTaskAsChild(Task* parent, TaskFunction func)
{
    parent->m_UnfinishedJobs++;
 
    Task* task = (Task*) operator new(sizeof(Task));
    task->m_Function = func;
    task->m_Parent = nullptr;
    task->m_UnfinishedJobs = 1;
 
    return task;
}


TRE_NS_END