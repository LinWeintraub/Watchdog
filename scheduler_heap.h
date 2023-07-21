#ifndef __SCHEDULER__
#define __SCHEDULER__

#include "pq_heap.h"

#include "uid.h"

#include <stddef.h> /* size_t */

typedef struct scheduler scheduler_t;

typedef int (*action_func_t)(void *param);

scheduler_t *SchedulerCreate(void);

void SchedulerDestroy(scheduler_t *scheduler);

uid_ty SchedulerAddTask(scheduler_t *scheduler, action_func_t op_func, void *param, time_t execution_time, size_t interval_time);

int SchedulerRemoveTask(scheduler_t *scheduler, uid_ty task_uid);

void SchedulerClear(scheduler_t *scheduler);
 
size_t SchedulerSize(const scheduler_t *scheduler);

int SchedulerIsEmpty(const scheduler_t *scheduler);

int SchedulerRun(scheduler_t *scheduler);

void SchedulerStop(scheduler_t *scheduler);

#endif /* __SCHEDULER__ */
