
/*
作者：文昌鱼
时间：2010.10.24
说明：线程局部对象
*/


#ifndef __BASICTOOL_THREAD_LOCAL_VAR__
#define __BASICTOOL_THREAD_LOCAL_VAR__

#include "public_include.h"

namespace bt
{
	//直接保存变量，不保存指针，不存在资源消耗的问题
	class thread_local_var
	{
	public:
		thread_local_var();
		~thread_local_var();

	public:
		void* get_value();
		void  set_value(void* value);

	protected:
#if defined(OS_WIN32)
		DWORD _dwTlsIndex;
#elif defined(OS_LINUX)
		pthread_key_t _key;
#endif
	};
}

#endif //__BASICTOOL_THREAD_LOCAL_VAR__

