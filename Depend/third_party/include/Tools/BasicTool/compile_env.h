
/*
作者：文昌鱼
时间：2010.06.12
说明：定义编译环境的宏。

*/


#ifndef __BASICTOOL_COMPILE_ENV__
#define __BASICTOOL_COMPILE_ENV__

namespace bt
{

//操作系统
#if defined(WIN32)
#define OS_WIN32
#pragma warning(disable:4996)
#elif defined(UCLINUX)
#define OS_LINUX
#define OS_UCLINUX			//OS_UCLINUX是OS_LINUX的子集
#elif defined(LINUX)
#define OS_LINUX
#elif defined(__GNUC__)
#define OS_LINUX
#endif

//LINUX的线程模型
//#define BT_LINUX_THREAD_MODE_LINUXTHREADS				//以进程模拟线程，默认这种
//#define BT_LINUX_THREAD_MODE_NPTL						//Native POSIX Thread Library.真正的多线程
#if defined(OS_LINUX)
	#if !defined(BT_LINUX_THREAD_MODE_LINUXTHREADS) && !defined(BT_LINUX_THREAD_MODE_NPTL)
		#if defined(UCLINUX)
			#define BT_LINUX_THREAD_MODE_LINUXTHREADS	//UCLINUX默认线程
		#else
			#define BT_LINUX_THREAD_MODE_NPTL			//其他LINUX默认线程
		#endif
	#endif
#endif

//是否调试
#if defined(OS_WIN32)
	#if defined(_DEBUG)
		#define BT_DEBUG
	#endif
#elif defined(OS_LINUX)
	#if defined(DEBUG) || defined(_DEBUG)
		#define BT_DEBUG
	#endif
#endif
#if !defined(BT_DEBUG) && !defined(NDEBUG)
	#define NDEBUG
#endif

//是否支持UNICODE。UCLINUX不支持UNICODE
#if !defined(OS_UCLINUX) && !defined(BT_SUPPORT_UNICODE)
#define BT_SUPPORT_UNICODE
#endif

//是否支持CPU的原子操作。OS_UCLINUX系统暂时不支持
#if !defined(OS_UCLINUX) && !defined(BT_SUPPORT_ATOMIC)
#define BT_SUPPORT_ATOMIC
#endif

//CPU类型
#if !defined(BT_CPU_TYPE_ARM)
#	if defined(_MSC_VER)
#		if defined(_WIN64)
#			define BT_CPU_TYPE_X64
#		else
#			define BT_CPU_TYPE_X86
#		endif
#	elif defined(__GNUC__)
#		if defined(__x86_64__)
#			define BT_CPU_TYPE_X64
#		else
#			define BT_CPU_TYPE_X86
#		endif
#	else
#		define BT_CPU_TYPE_X86
#	endif
#endif

//位数判断
#if defined(BT_CPU_TYPE_X64)
#	define BT_64
#else
#	define BT_32
#endif

//是否支持DMB
//#define BT_DONT_SUPPORT_ARM_DMB

//主机序形式。BT_BIG_ENDIAN表示高字节在前面
//#define BT_BIG_ENDIAN

//是否是单核CPU
//#define BT_SINGLE_CPU

//其他
#if defined(OS_WIN32)
	#if !defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x403)
		#undef _WIN32_WINNT
		#define _WIN32_WINNT    0x403
	#endif
#endif

//测试
//#define BT_STATISTICS

} //namespace bt

#endif //__BASICTOOL_COMPILE_ENV__

