#pragma once

#include <random>

#include "Core/Misc/Defines/Common.hpp"
#include "Core/TaskSystem/Task/Task.hpp"
#include "Core/TaskSystem/WorkStealingQueue/WorkStealingQueue.hpp"

TRE_NS_START

static uint8 GenerateRandomNumber(uint8 min, uint8 max)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(min, max);
    return uint8(dist(rng));
}

class TaskManager;

class TaskExecutor
{
public:
    TaskExecutor(TaskManager* tmanager, uint8 id);

    void DoWork();

    void Execute(Task* t);

    void Finish(Task* t);

    Task* GetTask();

    void Run(Task* t);

    void Wait(const Task* t);

    bool IsEmptyTask(Task* t) const;

    bool HasTaskCompleted(const Task* t) const;

    WorkStealingQueue* GetWorkerThreadQueue() const;
private:
    TaskManager* m_TaskManager;
    uint8 m_WorkerID;
    bool m_IsWorkerThreadActive;
};

TRE_NS_END