
/*
作者：文昌鱼
时间：2010.06.13
说明：定义系统的事件类
*/


#ifndef __BASICTOOL_EVENT__
#define __BASICTOOL_EVENT__

#include "public_include.h"

namespace bt
{
	class event
	{
	public:
		event(bool auto_reset = true);
		~event();

		void signal(bool signal = true);
		bool wait(uint timeout = BT_MAX_UINT);

	private:
#if defined(OS_WIN32)
		HANDLE _event_h;
#elif defined(OS_LINUX)
		bool _auto_reset;
		bool _signal;
		pthread_mutex_t _mutex_h;
		pthread_cond_t _cond_h;
#endif
	};

} //namespace bt

#endif //__BASICTOOL_EVENT__

