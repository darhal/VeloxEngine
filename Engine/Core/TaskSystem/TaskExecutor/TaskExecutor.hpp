#pragma once

#include <random>

#include "Core/Misc/Defines/Common.hpp"
#include "Core/TaskSystem/Task/Task.hpp"
#include <Core/TaskSystem/ParallelTask/TaskSplitters.hpp>
#include <Core/TaskSystem/ParallelTask/ParallelTask.hpp>
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

	Task* CreateTask(TaskFunction func, const void* data = NULL, uint32 aux_mem = 0);

	Task* CreateTaskAsChild(Task* parent, TaskFunction func, const void* data = NULL, uint32 aux_mem = 0);

	void AddContinuation(Task* ancestor, Task* continuation);

	template<typename T, typename S>
	Task* ParallelFor(T* data, uint32 count, ParallelTaskFunction<T> function, const S& splitter);

    void DoWork();

    void Execute(Task* t);

    void Finish(Task* t);

    Task* GetTask();

    void Run(Task* t);

    void Wait(const Task* t);

    bool IsEmptyTask(Task* t) const;

    bool HasTaskCompleted(const Task* t) const;

    WorkStealingQueue* GetWorkerThreadQueue() const;

	void Activate(bool is_active);

	bool IsActive() const;

	uint8 GetWorkerID() const;
private:
    TaskManager* m_TaskManager;
    uint8 m_WorkerID;
    bool m_IsWorkerThreadActive;

	template <typename TaskData>
	static void ParallelForTask(TaskExecutor* te, Task* task, const void* taskData);
};

template <typename TaskData>
void TaskExecutor::ParallelForTask(TaskExecutor* te, Task* task, const void* taskData)
{
	const TaskData* data = static_cast<const TaskData*>(taskData);
	const TaskData::SplitterType& splitter = data->splitter;

	if (splitter.Split<TaskData::DataType>(data->count))
	{
		// split in two
		const uint32 leftCount = data->count / 2;
		const TaskData leftData(data->data, leftCount, data->function, splitter);
		Job* left = te->CreateTaskAsChild(task, &TaskExecutor::ParallelForTask<TaskData>, &leftData, sizeof(TaskData));
	
		const uint32 rightCount = data->count - leftCount;
		const TaskData rightData(data->data + leftCount, rightCount, data->function, splitter);
		Job* right = te->CreateTaskAsChild(task, &TaskExecutor::ParallelForTask<TaskData>, &rightData, sizeof(TaskData));

		te->Run(left);
		te->Run(right);
	}
	else {
		// execute the function on the range of data

		(data->function)(te, task, data->data, data->count);
	}
}

template<typename T, typename S>
Task* TaskExecutor::ParallelFor(T* data, uint32 count, ParallelTaskFunction<T> function, const S& splitter)
{
	typedef ParallelForTaskData<T, S> TaskData;
	const TaskData taskData(data, count, function, splitter);

	Task* task = this->CreateTask(&TaskExecutor::ParallelForTask<TaskData>, &taskData, sizeof(TaskData));
	return task;
}


TRE_NS_END