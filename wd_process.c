#define _POSIX_SOURCE
#define _GNU_SOURCE

#include <stdlib.h> /*setenv, getenv*/
#include <stdio.h> /*sprintf*/
#include <pthread.h> /*pthreads functions*/
#include <unistd.h> /*fork*/
#include <sys/types.h> /*pid_t*/
#include <signal.h> /*sigaction, kill*/
#include <string.h> /*strdup, strcmp*/
#include <assert.h> /*assert*/

#include <fcntl.h> /*For O_* constants*/
#include <sys/stat.h> /*For mode constants*/
#include <semaphore.h> /*semaphore*/

#include "scheduler_heap.h"
#include "watchdog.h"
#include "tools.h"

typedef enum 
{
	SUCCESS = 0,
  FAILURE = -1,
	PTHREAD_FAILURE = -2,
  MEMORY_ALLOCATION_ERROR = -3,
  SIGACTION_FAILURE = -4,
  SEMAPHORE_FAILURE = -5
} status_t;    

static void *InitStruct(int argc, char *argv[]);

int main(int argc, char *argv[]) 
{
  watchdog_t *watchdog = NULL;
  struct sigaction sigaction_sigusr_1 = {0};
  struct sigaction sigaction_sigusr_2 = {0};
  pthread_t wd_thread = 0;
  sem_t *semaphore_thread = {0};
  sem_t *semaphore_start = {0};

  int status = 0;

  assert(argc);
  assert(argv);

  status = SetEnvWDPID(getpid());
  if (0 != status)
  {
    return FAILURE;
  }

  watchdog = InitStruct(argc, argv);
  if (NULL == watchdog)
  {
    return FAILURE;
  }

  /*if (0 != kill(getppid(), SIGSTOP))
  {
    return FAILURE;
  }*/

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

  semaphore_thread = sem_open("semaphore_thread", O_CREAT, S_IRWXU, 0);
  if (semaphore_thread == SEM_FAILED)
  {
      return SEMAPHORE_FAILURE;
  }

  if (0 != sem_post(semaphore_thread))
  {
    return SEMAPHORE_FAILURE;
  }

  semaphore_start = sem_open("semaphore_start", O_CREAT, S_IRWXU, 0);
  if (semaphore_start == SEM_FAILED)
  {
      return SEMAPHORE_FAILURE;
  }
  
  if (0 != pthread_create(&wd_thread, NULL, RunThread, (void *)watchdog))
  {
    return PTHREAD_FAILURE;
  }
  
  if (0 != sem_post(semaphore_start))
  {
    return SEMAPHORE_FAILURE;
  }

  /*if (0 != kill(getppid(), SIGCONT))
  {
    return FAILURE;
  }*/

  if (0 != pthread_join(wd_thread, NULL))
  {
    return PTHREAD_FAILURE;
  }
 
  return 0;
}

static void *InitStruct(int argc, char *argv[])
{
  char **args = NULL;
  watchdog_t *watchdog = NULL;
  scheduler_t *scheduler = NULL;

  const char *argc_str = NULL;
  const char *interval_str = NULL;
  const char *threshold_str = NULL;

  int argc_old = 0;
  size_t interval = 0;
  unsigned int threshold = 0;
  
  assert(argc);
  assert(argv);

  args = malloc((argc + 4) * sizeof(char *));
  if (NULL == args)
  {
    return NULL;
  }

  watchdog = malloc(sizeof(watchdog_t));
  if (NULL == watchdog)
  {
    return NULL;
  }

  scheduler = SchedulerCreate();
  if (NULL == scheduler)
  {
    return NULL;
  }

  argc_str = argv[argc - 3];
  interval_str = argv[argc - 2];
  threshold_str = argv[argc - 1];

  argc_old = atoi(argc_str);
  interval = atoi(interval_str);
  threshold = atoi(threshold_str);
  
  watchdog->argv = argv;
  watchdog->argc = argc_old;
  watchdog->target_pid = getppid();
  watchdog->exec_path = argv[0];
  watchdog->interval = interval;
  watchdog->threshold = threshold;
  watchdog->scheduler = scheduler;

  return watchdog;
}