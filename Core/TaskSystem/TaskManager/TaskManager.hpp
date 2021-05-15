#pragma once

#include "Core/Misc/Defines/Common.hpp"
#include "Core/TaskSystem/TaskExecutor/TaskExecutor.hpp"
#include "Core/Memory/Allocators/LinearAlloc/LinearAllocator.hpp"
#include <thread>

TRE_NS_START

typedef class TaskManager TaskScheduler;

class TaskManager
{
public:
    TaskManager(uint8 worker_count = ::std::thread::hardware_concurrency() - 1) : 
        m_Workers(NULL), m_TasksAllocator(NULL), m_AllocatedJobIndex(NULL),
		m_Allocator(worker_count * (sizeof(Worker) + sizeof(uint32) + sizeof(Task) * MAX_TASK_CAPACITY_PER_QUEUE), true),
		m_WorkersMaxCount(worker_count)
    {
        // m_Allocator.SetSize(m_WorkersMaxCount * sizeof(Worker));
        // m_Allocator.Init();
    };

    void Init();

    FORCEINLINE WorkStealingQueue* GetWorkingQueueByID(uint8 id) const;

    FORCEINLINE uint8 GetWorkerThreadCount() const;

    template<typename... Args>
    void AddWorker(uint8 id, void (*function)(TaskExecutor*, Args...), Args... args);

	FORCEINLINE Task* AllocateTask(uint8 worker_id);

private:
    CONSTEXPR static usize MAX_TASK_CAPACITY_PER_QUEUE = 4096u;
    CONSTEXPR static usize SIZE_OF_TASKS_PTR_PER_QUEUE = MAX_TASK_CAPACITY_PER_QUEUE * sizeof(Task*);

    struct Worker {
        TaskExecutor m_Scheduler;
        WorkStealingQueue m_WorkStealingQueue;
        ::std::thread m_WorkerThread;
    };

    Worker* m_Workers;
    Task* m_TasksAllocator;
	uint32* m_AllocatedJobIndex;
    LinearAllocator m_Allocator;
    uint8 m_WorkersMaxCount;

public:
    //::std::atomic<int32_t> m_TasksToDeleteCount;
};

FORCEINLINE void TaskManager::Init() // shouldn't be inlined !
{
    Worker* worker_ptr = (Worker*) m_Allocator.Allocate(sizeof(Worker) * m_WorkersMaxCount, 0);
	m_TasksAllocator = (Task*) m_Allocator.Allocate(sizeof(Task) * m_WorkersMaxCount * MAX_TASK_CAPACITY_PER_QUEUE, 0);
	m_AllocatedJobIndex = (uint32*) m_Allocator.Allocate(sizeof(uint32) * m_WorkersMaxCount, 0);

    m_Workers = worker_ptr;

    for(uint8 id = 0; id < m_WorkersMaxCount; id++){
        // printf("Creating thread id = %d\n", id);
        new (&(worker_ptr[id].m_Scheduler)) TaskExecutor(this, id);
        new (&(worker_ptr[id].m_WorkStealingQueue)) WorkStealingQueue();
        void* buffer = operator new(SIZE_OF_TASKS_PTR_PER_QUEUE);
        reinterpret_cast<WorkStealingQueue*>(&(worker_ptr[id].m_WorkStealingQueue))->Init(MAX_TASK_CAPACITY_PER_QUEUE, buffer, SIZE_OF_TASKS_PTR_PER_QUEUE);
		m_AllocatedJobIndex[id] = 0;
    }
}

template<typename... Args>
void TaskManager::AddWorker(uint8 id, void(*function)(TaskExecutor*, Args...), Args... args)
{
    uint8 real_id = id % m_WorkersMaxCount;
    new (&(m_Workers[real_id].m_WorkerThread)) ::std::thread(function, &(m_Workers[real_id].m_Scheduler), args...);
	m_Workers[real_id].m_WorkerThread.detach();
}

FORCEINLINE WorkStealingQueue* TaskManager::GetWorkingQueueByID(uint8 id) const
{
    return &(m_Workers[id % m_WorkersMaxCount].m_WorkStealingQueue);
}

FORCEINLINE uint8 TaskManager::GetWorkerThreadCount() const
{
    return m_WorkersMaxCount;
}

FORCEINLINE Task* TaskManager::AllocateTask(uint8 worker_id)
{
	const uint32 index = m_AllocatedJobIndex[worker_id]++;
	return &m_TasksAllocator[worker_id * MAX_TASK_CAPACITY_PER_QUEUE * sizeof(Task) + index & (MAX_TASK_CAPACITY_PER_QUEUE - 1)];
}

TRE_NS_END