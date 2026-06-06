
/*
作者：文昌鱼
时间：2010.06.13
说明：定义系统的互斥体类
*/


#ifndef __BASICTOOL_MUTEX__
#define __BASICTOOL_MUTEX__

#include "lock.h"

namespace bt
{
	class mutex : public base_lock
	{
	public:
		mutex();
		virtual ~mutex();

		virtual void virtual_lock();
		virtual bool virtual_try_lock();
		virtual void virtual_unlock();

		void lock();
		bool try_lock();
		void unlock();

	private:
#if defined(OS_WIN32)
		CRITICAL_SECTION _cs;
#elif defined(OS_LINUX)
		pthread_mutex_t _mutex_h;
#endif
		int				_tmp;				//统计用
	};

	typedef auto_lock<mutex> auto_lock_mutex;

} //namespace bt

#endif //__BASICTOOL_MUTEX__

