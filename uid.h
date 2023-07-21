#ifndef __UID__
#define __UID__

#include <unistd.h> /* pid_t */
#include <sys/types.h> /* pid_t */
#include <time.h> /* time */
#include <stddef.h> /* size_t */

struct uid
{
    pid_t pid;
    time_t time;
    size_t counter;
    char ip_address[14];
};

typedef struct uid uid_ty;

extern const uid_ty bad_uid; /* You should assign the appropriate value */

uid_ty UIDCreate(void);

int UIDIsEqual(uid_ty uid_1, uid_ty uid_2);

#endif /* __UID__ */
