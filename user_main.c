#include <stddef.h>
#include <stdio.h>

#include "watchdog.h"
#include "tools.h"

int main(int argc, char *argv[]) 
{
    size_t interval = 2;
    unsigned int threshold = 5;
    int i = 0;
    /*char *str = NULL;*/

    WDStart(argc, argv, interval, threshold);

    for (i = 0; i < 10; ++i)
    {
        printf("TEST: %d \n", i);
        sleep(1);
    }

    /*printf("%d\n", *str);*/
    WDStop();

    return 0;
}