
/*
作者：文昌鱼
时间：2010.06.13
说明：放linux操作系统相关的
*/


#ifndef __BASICTOOL_LINUX__
#define __BASICTOOL_LINUX__

#include "compile_env.h"

#if defined(OS_LINUX)

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 
#include <unistd.h>
#include <semaphore.h>
#include <sys/time.h>
#include <sys/times.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/syscall.h>

#if defined(BT_SUPPORT_UNICODE)
#include <wchar.h>
#include <wctype.h>
#endif

//返回当前时间偏移后的绝对时间
inline void get_off_abstime(struct timespec* abs_time, uint msec_off)
{
	//clock_gettime(CLOCK_REALTIME, abs_time);
	//abs_time->tv_nsec += (msec_off % 1000) * 1000 * 1000;
	//abs_time->tv_sec += msec_off / 1000;
	//if (abs_time->tv_nsec >= 1000000000)
	//{

	//}

    struct timeval now; 
    gettimeofday(&now, NULL); 
    abs_time->tv_nsec = now.tv_usec * 1000 + (msec_off % 1000) * 1000000; 
    abs_time->tv_sec = now.tv_sec + msec_off / 1000;
	if (abs_time->tv_nsec >= 1000000000)
	{
		abs_time->tv_sec++;
		abs_time->tv_nsec -= 1000000000;
	}
}


#endif

#endif //__BASICTOOL_LINUX__

