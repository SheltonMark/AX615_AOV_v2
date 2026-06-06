
/*
作者：文昌鱼
时间：2013.12.10
说明：全局资源管理
*/
#ifndef __BASICTOOL_GLOBAL_RESOURCE__
#define __BASICTOOL_GLOBAL_RESOURCE__

#include "singleton.h"
#include "thread.h"
#include "stl.h"
#include "seq_lock.h"

namespace bt
{
	class global_resource : public singleton<global_resource>, public thread_body
	{
	public:
		global_resource();
		~global_resource();

	public:
		virtual void run(ptr_int user_data);

	public:
		nint		get_tick()				{return _tick;}				//直接从变量获取，减少CPU
		time_t		get_time()				{return _time;}
		nint64		get_usec_time()			{return _usec_time.get_data();}
		void		get_time_tm(struct tm* ptm){*ptm = _time_tm.get_data();}
		ZLTIME		get_zl_time()			{return _zl_time;}

		void		stop_thread();

	protected:
		bt::thread _thread_get_tick;
		volatile nint _tick;
		volatile time_t _time;			//_time可能是64位的，但只有到2036年才会在高位跳1位，基本可以考虑高位不变。所以也不需要锁保护
		seq_lock<nint64> _usec_time;	//微妙级别的绝对时间
		seq_lock<struct tm> _time_tm;
		volatile ZLTIME _zl_time;

		//监控线程关闭
#if defined(OS_WIN32)
	public:
		void add_watch_thread(int threadid);
	protected:
		void _watch_thread_exit();
		void _all_thread_exit();

	protected:
		mutex _thread_mtx;
		bt::thread _thread_watch_handle;
		std::map<int, HANDLE> _id_to_handle;
#endif
	};
}

#endif //__BASICTOOL_GLOBAL_RESOURCE__

