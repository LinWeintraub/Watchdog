#ifndef __TASK__
#define __TASK__

#include "uid.h" 

typedef struct task task_t;
typedef int (*op_func_t)(void *param);

task_t *TaskCreate(op_func_t task_func, time_t execution_time, size_t interval_time, void *param);
/*
Description: The function creates a new task.
Return: The function returns a pointer to the new task or NULL if it fails.
*/

void TaskDestroy(task_t *task);
/*
Description: The function frees the task.
*/

uid_ty GetUid(const task_t *task);
/*
Description: The function return uid of the task.
Return: The function returns uid or bad_uid if it fails.
*/

time_t TaskGetExecutionTime(task_t *task);
/*
Description: The function return time to run of the task.
Return: time_t execution_time.
*/

size_t TaskGetIntervalTime(task_t *task);
/*
Description: The function return interval time of the task.
Return: size_t interval_time.
*/

time_t TaskSetExecutionTime(task_t *task, time_t new_time);
/*
Description: The function update the execution_time of the task.
*/

int TaskRun(task_t *task);
/*
Description: The function run the task function.
*/

#endif /* __TASK__ */
