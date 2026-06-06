
/*
作者：文昌鱼
时间：2010.06.13
说明：定义系统的信号量类
*/


#ifndef __BASICTOOL_SEMA__
#define __BASICTOOL_SEMA__

#include "public_include.h"

namespace bt
{
	class sema
	{
	public:
		sema(uint init_count = 0);
		~sema();

		void post(uint count = 1);
		bool wait(uint timeout = BT_MAX_UINT);

	private:
#if defined(OS_WIN32)
		HANDLE _sema_h;
#elif defined(OS_LINUX)
		sem_t _sema_h;
#endif
	};

} //namespace bt

#endif //__BASICTOOL_SEMA__

