#include "TaskExecutor.hpp"
#include "Core/TaskSystem/TaskManager/TaskManager.hpp"
#include <Core/Misc/Utils/Logging.hpp>

TRE_NS_START

TaskExecutor::TaskExecutor(TaskManager* tmanager, uint8 id) : 
    m_TaskManager(tmanager), m_WorkerID(id), m_IsWorkerThreadActive(true)
{
}

Task* TaskExecutor::CreateTask(TaskFunction func, uint32 aux_mem)
{
	Task* task = m_TaskManager->AllocateTask(m_WorkerID);
	task->m_Function = func;
	task->m_Parent = NULL;
	task->m_UnfinishedJobs = 1;
	// TODO: Add au memory allocation if ncesseray otherwise put it in the padding

	return task;
}

Task* TaskExecutor::CreateTaskAsChild(Task* parent, TaskFunction func, uint32 aux_mem)
{
	parent->m_UnfinishedJobs++;

	Task* task = m_TaskManager->AllocateTask(m_WorkerID);
	task->m_Function = func;
	task->m_Parent = NULL;
	task->m_UnfinishedJobs = 1;
	// TODO: Add au memory allocation if ncesseray otherwise put it in the padding

	return task;
}

void TaskExecutor::Activate(bool is_active)
{
	m_IsWorkerThreadActive = is_active;

	if (is_active)
		this->DoWork();
}

bool TaskExecutor::IsActive() const
{
	return m_IsWorkerThreadActive;
}

bool TaskExecutor::IsEmptyTask(Task* t) const
{
    return !t;
}

WorkStealingQueue* TaskExecutor::GetWorkerThreadQueue() const
{
    return m_TaskManager->GetWorkingQueueByID(m_WorkerID);
}

Task* TaskExecutor::GetTask()
{
    WorkStealingQueue* queue = GetWorkerThreadQueue();
    Task* job = queue->Pop();

    if (this->IsEmptyTask(job)){
        // this is not a valid job because our own queue is empty, so try stealing from some other queue
        uint8 randomIndex = GenerateRandomNumber(0, m_TaskManager->GetWorkerThreadCount() - 1);
		//LOG::Write(LOG::WARN, "[THREAD ID : %d] Random(0, %d) Index is %d ...", m_WorkerID, m_TaskManager->GetWorkerThreadCount() - 1, randomIndex);
		WorkStealingQueue* stealQueue = m_TaskManager->GetWorkingQueueByID(randomIndex);

        if (stealQueue == queue){
            // don't try to steal from ourselves. Attempt to steal from the main queue
            TASK_YIELD();
            return NULL;
        }
    
        Job* stolenJob = stealQueue->Steal();
        if (this->IsEmptyTask(stolenJob)){
            // we couldn't steal a job from the other queue either, so we just yield our time slice for now
            TASK_YIELD();
            return NULL;
        }

        stolenJob->m_WorkerID = m_WorkerID;
        return stolenJob;
    }

    job->m_WorkerID = m_WorkerID;
    return job;
}

void TaskExecutor::DoWork()
{
    while (m_IsWorkerThreadActive){
        Task* t = this->GetTask();

        if (t && !this->HasTaskCompleted(t)){
            this->Execute(t);
		}
    }
}

void TaskExecutor::Execute(Task* t)
{
    (t->m_Function)(t, t->m_Data);
    this->Finish(t);
}

void TaskExecutor::Finish(Task* t)
{
    const int32_t unfinishedJobs = --t->m_UnfinishedJobs;

    if (unfinishedJobs == 0){

        // const int32_t index = m_TaskManager->m_TasksToDeleteCount++;
        // m_TaskManager->QueueTaskForDeletion(index, t); // Delete task
		// m_TaskManager->m_TasksToDeleteCount++;
 
        if (t->m_Parent) {
            this->Finish(t->m_Parent);
        }
 
        t->m_UnfinishedJobs--;
    }
}

bool TaskExecutor::HasTaskCompleted(const Task* t) const
{
    return t->m_UnfinishedJobs <= 0;
}

void TaskExecutor::Run(Task* t)
{
    WorkStealingQueue* queue = GetWorkerThreadQueue();
    queue->Push(t);
}

void TaskExecutor::Wait(const Task* t)
{
    // wait until the job has completed. in the meantime, work on any other job.
    while (!HasTaskCompleted(t)){
        Task* nextTask = this->GetTask();

        if (nextTask){
            this->Execute(nextTask);
        }
    }
}

TRE_NS_END