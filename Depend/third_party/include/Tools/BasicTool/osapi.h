
/*
作者：文昌鱼
时间：2010.06.12
说明：定义一些与系统相关的API

时间：2011.04.16
说明：修改get_tick的实现，linux下用clock_gettime获得计时，更符合本意。

*/


#ifndef __BASICTOOL_OSAPI__
#define __BASICTOOL_OSAPI__

#include "public_include.h"

namespace bt
{
	//sleep(0)，表示切换线程
	inline void sleep(uint msec);

	//刷新CPU缓存。不支持原子操作的，用锁代替，比信号量性能好，信号量需要进入内核。
	void flush_all_cpu_cache();

	//编译器栅栏，干扰编译器的优化顺序
#if defined(OS_WIN32)
	#if !defined(BT_DEBUG)
		#define compiler_barrier()	_ReadWriteBarrier()
	#else
		#define compiler_barrier()
	#endif
#elif defined(OS_LINUX)
	#define compiler_barrier()	asm volatile("" ::: "memory")
#else
	#define compiler_barrier()
#endif

	//内存栅栏，用来代替flush_all_cpu_cache，提高性能。影响CPU CACHE
	void read_barrier();
	void write_barrier();
	void read_write_barrier();

	//得到一个递增的值
	uint get_inc_value();

	//字节顺序替换
	template<class TYPE>
	inline TYPE reverse_type(TYPE type);

	//网络序和主机序的转化
	template<class TYPE>
	inline TYPE net_to_host(TYPE data);
	template<class TYPE>
	inline TYPE host_to_net(TYPE data);

	//lg2 N计算
	inline uint log2(uint32_t v);
	inline uint log2byte(abyte v);
	inline int  compare_uint(uint a, uint b);

	//和宏的性能差不多，应该编译器已经优化了
	//template<class TYPE>
	//inline TYPE bt_min(TYPE a, TYPE b);
	//template<class TYPE>
	//inline TYPE bt_max(TYPE a, TYPE b);
	//template<class TYPE>
	//inline TYPE bt_range(TYPE n, TYPE m, TYPE v);

	//获取进程所有ID
	int  get_process_id();
	//uint get_all_thread_id(int* threadid, uint max_num);

	//打印宏
	void print_macro();
} //namespace bt

#include "osapi.inl"

#endif //__BASICTOOL_OSAPI__

