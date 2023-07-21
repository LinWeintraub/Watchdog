/* ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ *
 ~ Author        Lin Weintraub                                   ~
 * Version       1.0.0                                           *
 ~ Date          28/12/22                                        ~
 * Description   WatchDog                                        *
 * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ */
 
#define _POSIX_SOURCE
#define _GNU_SOURCE

#include <stdlib.h> /*malloc*/
#include <unistd.h> /*fork*/
#include <sys/types.h> /*pid_t*/
#include <signal.h> /*kill*/
#include <stdio.h> /*printf*/
#include <pthread.h> /*pthreads functions*/
#include <string.h> /*strdup*/
#include <assert.h> /*assert*/


#include <fcntl.h> /*For O_* constants*/
#include <sys/stat.h> /*For mode constants*/
#include <semaphore.h> /*semaphore*/

#include "scheduler_heap.h"
#include "watchdog.h"
#include "tools.h"

static char **InitArgs(int argc, 
                        char **argv, 
                        size_t interval, 
                        unsigned int threshold);

typedef enum 
{
	SUCCESS = 0,
    FAILURE = -1,
	PTHREAD_FAILURE = -2,
    FORK_FAILURE = -3,
    MEMORY_ALLOCATION_ERROR = -4,
    SIGACTION_FAILURE = -5,
    SEMAPHORE_FAILURE = -6
} status_t;   

int WDStart(int argc, char **argv, size_t interval, unsigned int threshold)
{
    pthread_t user_thread = 0;
    sem_t *semaphore_start = {0};
    sem_t *semaphore_thread = {0};
    pid_t pid = 0;
    scheduler_t *scheduler = NULL;
    struct sigaction sigaction_sigusr_1 = {0};
    struct sigaction sigaction_sigusr_2 = {0};
    int status = 0;
    char **args = NULL;
    watchdog_t *watchdog = NULL;

    assert(argc);
    assert(argv);
    assert(interval);
    assert(threshold);

    args = InitArgs(argc, argv, interval, threshold);
    if (NULL == args)
    {
        return MEMORY_ALLOCATION_ERROR;
    }

    watchdog = malloc(sizeof(watchdog_t));
    if (NULL == watchdog)
    {
        return MEMORY_ALLOCATION_ERROR;
    }

    scheduler = SchedulerCreate();
    if (NULL == scheduler)
    {
        return MEMORY_ALLOCATION_ERROR;
    }
    
    if (-1 == sem_unlink("semaphore_thread"))
    {
        return SEMAPHORE_FAILURE;
    }

    if (-1 == sem_unlink("semaphore_start"))
    {
        return SEMAPHORE_FAILURE;
    }

    sigaction_sigusr_1.sa_handler = HandlerSIGUSR1;
    sigaction_sigusr_2.sa_handler = HandlerSIGUSR2;

    if (-1 == sigaction(SIGUSR1, &sigaction_sigusr_1, NULL))
	{
		return SIGACTION_FAILURE;
	}

    if (-1 == sigaction(SIGUSR2, &sigaction_sigusr_2, NULL))
	{
        return SIGACTION_FAILURE;
	}
    
    semaphore_start = sem_open("semaphore_start", O_CREAT, S_IRWXU, 0);
    if (semaphore_start == SEM_FAILED)
    {
        return SEMAPHORE_FAILURE;
    }

    semaphore_thread = sem_open("semaphore_thread", O_CREAT, S_IRWXU, 0);
    if (semaphore_thread == SEM_FAILED)
    {
        return SEMAPHORE_FAILURE;
    }

    pid = fork();
    if (pid < 0)
    {
        return FORK_FAILURE;
    }
    
    watchdog->argv = argv;
    watchdog->argc = argc;
    watchdog->target_pid = pid;
    watchdog->exec_path = "./wd_process.out";
    watchdog->interval = interval;
    watchdog->threshold = threshold;
    watchdog->scheduler = scheduler;
    
    status = SetEnvWDPID(pid);
    if (0 != status)
    {
        return FAILURE;
    }
    
    if (pid > 0) /*parent - user_process*/
    {
        if (0 != sem_wait(semaphore_thread))
        {
            return SEMAPHORE_FAILURE;
        }

        if (0 != pthread_create(&user_thread, NULL, RunThread, (void *)watchdog))
        {
            return PTHREAD_FAILURE;
        }
        
        if (0 != pthread_detach(user_thread))
        {
            return PTHREAD_FAILURE;
        }
    }
    
    else if (pid == 0) /*child - wd_process*/
    {
        printf("execvp\n");
        execvp(watchdog->exec_path, args);
    }

    if (0 != sem_wait(semaphore_start))
    {
        return SEMAPHORE_FAILURE;
    }

    
    return SUCCESS;
}

int WDStop(void)
{
    sem_t *semaphore_start = {0};
    sem_t *semaphore_thread = {0};

    semaphore_thread = sem_open("semaphore_thread", O_CREAT, S_IRWXU, 0);
    if (semaphore_thread == SEM_FAILED)
    {
        return SEMAPHORE_FAILURE;
    }

    semaphore_start = sem_open("semaphore_start", O_CREAT, S_IRWXU, 0);
    if (semaphore_start == SEM_FAILED)
    {
        return SEMAPHORE_FAILURE;
    }

    printf("HELLO WDStop\n");

    if (0 != kill(atoi(getenv("WD_PID")), SIGUSR2))
    {
        return FAILURE;
    }

    if (-1 == sem_close(semaphore_thread))
    {
        return SEMAPHORE_FAILURE;
    }
    
    if (-1 == sem_close(semaphore_thread))
    {
        return SEMAPHORE_FAILURE;
    }
    
    return 0;
}

static char **InitArgs(int argc, 
                        char **argv, 
                        size_t interval, 
                        unsigned int threshold)
{
    char **args = NULL;
    char buffer[32] = {0};
    char *argc_str = NULL;
    char *interval_str = NULL;
    char *threshold_str = NULL;
    int i = 0;

    assert(argc);
    assert(argv);
    assert(interval);
    assert(threshold);
    
    args = malloc((argc + 4) * sizeof(char *));
    if (NULL == args)
    {
        return NULL;
    }
    
    for (i = 0; i < argc; i++) 
    {
        args[i] = strdup(argv[i]);
        if (NULL == args[i])
        {
            return NULL;
        }
    }

    if (-1 == sprintf(buffer, "%d", argc))
    {
        return NULL;
    }

    argc_str = strdup(buffer);
    if (NULL == argc_str)
    {
        return NULL;
    }

    if (-1 == sprintf(buffer, "%ld", interval))
    {
        return NULL;
    }

    interval_str = strdup(buffer);
    if (NULL == interval_str)
    {
        return NULL;
    }
    
    if (-1 == sprintf(buffer, "%u", threshold))
    {
        return NULL;
    }

    threshold_str = strdup(buffer);
    if (NULL == threshold_str)
    {
        return NULL;
    }

    args[argc] = argc_str;
    args[argc + 1] = interval_str;
    args[argc + 2] = threshold_str;
    args[argc + 3] = NULL;

    return args;
}
