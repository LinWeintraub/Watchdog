
#ifndef __WatchDog__
#define __WatchDog__

int WDStart(int argc, char **argv, size_t interval, unsigned int threshold);

int WDStop(void);

#endif /* __WatchDog__ */

