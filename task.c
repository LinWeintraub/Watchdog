/* ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ *
 ~ Author        Lin Weintraub                                   ~
 * Version       0.0.1                                           *
 ~ Date          25/10/22                                        ~
 * Description   Task                                            *
 * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ */
 
#include <stdlib.h> /* malloc, free */
#include <assert.h> /* assert */

#include "task.h"

struct task
{
    op_func_t task_func;
    uid_ty uid;
    time_t execution_time;
    size_t interval_time;
    void *param;
};

task_t *TaskCreate(op_func_t task_func, time_t execution_time, size_t interval_time, void *param)
{
	task_t *task = malloc(sizeof(task_t));
	uid_ty task_uid = UIDCreate();	
	
	assert(task_func);
	assert(param);
	
	if (NULL == task)
	{
		return NULL;
	}
	task->task_func = task_func;
		
	if (1 == UIDIsEqual(task_uid, bad_uid))
	{
		free(task); task = NULL;
		return NULL;
	}
	task->uid = task_uid;
	

	task->execution_time = execution_time;
	
	task->interval_time = interval_time;
	task->param = param;

	return task;
}

void TaskDestroy(task_t *task)
{
	assert(task);
	
	free(task);
	task = NULL;
}

uid_ty GetUid(const task_t *task)
{
	assert(task);

	return (task->uid);
}

time_t TaskGetExecutionTime(task_t *task)
{
	assert(task);

	return (task->execution_time);
}

time_t TaskSetExecutionTime(task_t *task, time_t new_time)
{	
	time_t temp_time = time(NULL);
	
	assert(task);
	assert(new_time);
	
	if (-1 == temp_time)
	{
		return 0;
	}
	
	task->execution_time = temp_time + new_time;
	
	return (task->execution_time);
}

size_t TaskGetIntervalTime(task_t *task)
{
	assert(task);

	return (task->interval_time);
}

int TaskRun(task_t *task)
{
	assert(task);

	return (task->task_func(task->param));
}
