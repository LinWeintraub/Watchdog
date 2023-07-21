#define _POSIX_SOURCE
#define _GNU_SOURCE

#include <stdlib.h> /*setenv, getenv*/
#include <pthread.h> /*pthreads functions*/
#include <stddef.h>
#include <sys/types.h> /*pid_t*/
#include <unistd.h> /*fork*/
#include <signal.h> /*sigaction, kill*/
#include <string.h> /*strcmp*/

#include <fcntl.h> /*For O_* constants*/
#include <sys/stat.h> /*For mode constants*/
#include <semaphore.h> /*semaphore*/

#include <stdio.h> /*printf*/
#include <assert.h> /*assert*/

#include "tools.h"
#include "scheduler_heap.h"
#include "watchdog.h"

#define UNUSED_VARIABLE(x) ((void)x)

size_t signal_counter = 0;
int flag_is_running = 1;

typedef enum 
{
	SUCCESS = 0,
    FAILURE = -1,
    FORK_FAILURE = -2,
    MEMORY_ALLOCATION_ERROR = -3,
    SEMAPHORE_FAILURE = -4
} status_t;   

static void ClearArgv(int argc, char *argv[]);

void *RunThread(void *watchdog)
{
    int status = 0;

    assert(argc);
    assert(argv);

    status = SignalsMonitor((void *)watchdog);
    if (FAILURE == status)
    {
        return NULL;
    }

    status = SchedulerRun(((watchdog_t *)watchdog)->scheduler);
    if (FAILURE == status)
    {
        return NULL;
    }

    return NULL;
}

int SignalsMonitor(void *watchdog)
{
    uid_ty send_signal = {0};
    uid_ty check_signal = {0};
    uid_ty check_stop = {0};

	time_t curr_time = 0;

    assert(watchdog);

    curr_time = time(NULL);
    if (-1 == curr_time)
    {
        return FAILURE;
    }

    send_signal = SchedulerAddTask(((watchdog_t *)watchdog)->scheduler, 
                                    TaskSendSignal,         
                                    (void *)watchdog, 
                                    curr_time, 
                                    ((watchdog_t *)watchdog)->interval);
	if (UIDIsEqual(bad_uid, send_signal))
	{
        return FAILURE;
	}

    check_signal = SchedulerAddTask(((watchdog_t *)watchdog)->scheduler, 
                                    TaskCheckSignal, 
                                    (void *)watchdog, 
                                    curr_time, 
                                    ((watchdog_t *)watchdog)->interval);
	if (UIDIsEqual(bad_uid, check_signal))
	{
        return FAILURE;
	}

    check_stop = SchedulerAddTask(((watchdog_t *)watchdog)->scheduler, 
                                    TaskCheckStop, 
                                    (void *)watchdog, 
                                    curr_time, 
                                    ((watchdog_t *)watchdog)->interval);
	if (UIDIsEqual(bad_uid, check_stop))
	{
        return FAILURE;
	}

    return SUCCESS;
}

int TaskSendSignal(void *watchdog)
{
    assert(watchdog);
    kill(((watchdog_t *)watchdog)->target_pid, SIGUSR1);
    /*if (0 != kill(((watchdog_t *)watchdog)->target_pid, SIGUSR1))
    {
        return FAILURE;
    }*/

    printf("target_pid: %d\n", ((watchdog_t *)watchdog)->target_pid);

    printf("signal_counter: %ld\n", signal_counter);
    ++signal_counter;

    return 0;
}

int TaskCheckSignal(void *watchdog)
{
    pid_t pid = 0;
    int status = 0;

    assert(watchdog);

    printf("CHECK SIGNAL\n");

    if (signal_counter > ((watchdog_t *)watchdog)->threshold)
    {
        printf("signal_counter > threshold\n");

        if (strcmp(((watchdog_t *)watchdog)->exec_path, "./wd_process.out") == 0) /*user - restart WD*/
        {
            pid = fork();
            if (pid < 0)
            {
                return FORK_FAILURE;
            }

            status = SetEnvWDPID(pid);
            if (0 != status)
            {
                return FAILURE;
            }              

            if (pid > 0) /*parent - user_process*/
            {
                signal_counter = 0;
                flag_is_running = 1;

                printf("USER after wd died\n");
            }

            else if (pid == 0) /*child - wd_process*/
            {
                printf("execvp after wd died\n");
                execvp(((watchdog_t *)watchdog)->exec_path, ((watchdog_t *)watchdog)->argv);
            }
        }

        else /*wd - restart user*/
        {
            execvp(((watchdog_t *)watchdog)->exec_path, ((watchdog_t *)watchdog)->argv);
        }
    }

    return status;
}

int TaskCheckStop(void *watchdog)
{
    assert(watchdog);

    printf("TaskCheckStop\n");

    if (0 == flag_is_running)
    {
        printf("SchedulerStop\n");

        SchedulerStop(((watchdog_t *)watchdog)->scheduler);

        printf("SchedulerDestroy\n");

        SchedulerDestroy(((watchdog_t *)watchdog)->scheduler);
                
        printf("AFTER SchedulerDestroy\n");

        ClearArgv(((watchdog_t *)watchdog)->argc, ((watchdog_t *)watchdog)->argv);
        
        printf("AFTER ClearArgv\n");
        
        /*free(((watchdog_t *)watchdog)->argv); ((watchdog_t *)watchdog)->argv = NULL;*/

        if (atoi(getenv("WD_PID")) == getpid())
        {
            kill(((watchdog_t *)watchdog)->target_pid, SIGUSR2);
        }
        
        free((watchdog_t *)watchdog); watchdog = NULL;

        printf("AFTER FREE\n");
    }
    

    return SUCCESS;
}

void HandlerSIGUSR1(int signal)
{
    signal_counter = 0;
    printf("SIGNAL Handler\n");
    UNUSED_VARIABLE(signal);
}

void HandlerSIGUSR2(int signal)
{
    flag_is_running = 0;
    printf("SIGNAL Handler2\n");
    UNUSED_VARIABLE(signal);
}

int SetEnvWDPID(pid_t pid)
{
    int status = 0;
    char pid_str[16]; /*buffer to hold the PID as a string*/

    assert(pid);

    if (-1 == sprintf(pid_str, "%d", pid)) /*convert PID to a string*/
    {
        return FAILURE;
    }

    status = setenv("WD_PID", pid_str, 1); /*set the WD_PID variable*/
    if (status != 0) 
    {
        perror("setenv");
        return FAILURE;
    }

    return SUCCESS;
}

static void ClearArgv(int argc, char **argv)
{
    int i = 0;
    printf("HIIIIIIII\n");

    for (i = 0; i < argc - 1; i++) 
    {
        free(argv[i]); argv[i] = NULL;
    }
}
