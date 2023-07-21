#ifndef __WatchDog_Tools__
#define __WatchDog_Tools__

#include <semaphore.h> /*semaphore*/

#include "scheduler_heap.h"

typedef struct watchdog
{
    char **argv;
    int argc;
    pid_t target_pid;
    size_t interval;
    unsigned int threshold;
    char *exec_path;
    scheduler_t *scheduler;
} watchdog_t;

void *RunThread(void *watchdog);
int SignalsMonitor(void *watchdog);
int TaskSendSignal(void *watchdog);
int TaskCheckSignal(void *watchdog);
int TaskCheckStop(void *watchdog);
void HandlerSIGUSR1(int signal);
void HandlerSIGUSR2(int signal);
int SetEnvWDPID(pid_t pid);

#endif /* __WatchDog_Tools__ */
