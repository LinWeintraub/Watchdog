/* ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ *
 ~ Author        Lin Weintraub                                   ~
 * Version       0.0.1                                           *
 ~ Date          24/10/22                                        ~
 * Description   Unique identifier (UID)                         *
 * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ * ~ */

#include <string.h> /* memcpy */
#include <assert.h> /* assert */
#include <arpa/inet.h> /* inet_ntoa	*/
#include <sys/socket.h> /* sockaddr_in */
#include <ifaddrs.h> /* getifaddrs, ifap */

#include "uid.h"

static size_t Counter(size_t *number);
static void GetIPAddress(char *addr);

const uid_ty bad_uid = {0, 0, 0, {0}};
static size_t number = 0;

uid_ty UIDCreate(void)
{
	uid_ty uid;
	time_t temp_time = time(NULL);
	
	if (-1 == temp_time)
	{
		uid = bad_uid;
	}
	
	uid.pid = getpid();
	uid.time = temp_time;
	uid.counter = Counter(&number);
	GetIPAddress(uid.ip_address);
		
	return uid;	
}

static size_t Counter(size_t *number)
{	
	assert(number);
	
	*number = *number + 1;
	
	return *number;
}

int UIDIsEqual(uid_ty uid_1, uid_ty uid_2)
{
	int status = 1;
	
	if ((uid_1.pid != uid_2.pid) || (uid_1.time != uid_2.time) || (uid_1.counter != uid_2.counter) || (0 != strcmp(uid_1.ip_address, uid_2.ip_address)))
	{
		status = 0;
	}
	
	return status;
}

static void GetIPAddress(char *addr)
{
	struct ifaddrs *ifap, *ifa;
	struct sockaddr_in *sa;

	assert(addr);
	
	getifaddrs(&ifap);
	for (ifa = ifap; !(ifa->ifa_addr && ifa->ifa_addr->sa_family==AF_INET); ifa = ifa->ifa_next)
	{
		/* do nothing*/
	}

	sa = (struct sockaddr_in *) ifa->ifa_addr;
	memcpy(addr, inet_ntoa(sa->sin_addr), 14);

	freeifaddrs(ifap);
}

