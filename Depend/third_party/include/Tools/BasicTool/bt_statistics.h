
/*
作者：文昌鱼
时间：2010.10.08
说明：增加静态缓存数量，减少申请内存的频率
赋值操作符和构造函数是不继承的，要重新声明
*/

#ifndef __BASICTOOL_STATISTICS__
#define __BASICTOOL_STATISTICS__

#include "public_include.h"
#include "stl.h"
#include "refer.h"
#include "mutex.h"
#include "atomic.h"

namespace bt
{
	//引用索引统计
	extern nint					g_statis_refer_count;

	//SPIN调用SLEEP的次数
	extern nint					g_statis_spin_times;
	extern nint					g_statis_spin_sleep;
	extern nint					g_statis_spin_hit;

	//sleep的统计
	extern nint					g_statis_sleep_times;

	//event个性
	extern nint					g_statis_event_wait;
	extern nint					g_statis_event_signal;

	//atomic次数
	extern nint					g_statis_atomic_times;

	//锁
	extern nint					g_statis_mutex_lock;
	extern nint					g_statis_mutex_lock_conflicts;
	extern nint					g_statis_seq_lock;

	//弱锁
	extern nint					g_statis_weak_lock_conflicts1;
	extern nint					g_statis_weak_lock_conflicts2;

	//time
	extern nint					g_statis_get_time;

	//global resource线程运行次数
	extern nint					g_statis_global_resource_thread;

	//和网络相关的统计
	extern nint					g_statis_select_times;
	extern nint					g_statis_comm_data_size;

	//two_queue
	extern nint					g_statis_two_queue_failed;

	//线程局部对象数量
	extern nint					g_statis_thread_var_num;

	//打印统计值
	void dump_statistics();
	void reset_statistics();
}

#endif //__BASICTOOL_STATISTICS__

