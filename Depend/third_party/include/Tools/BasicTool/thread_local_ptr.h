
/*
作者：文昌鱼
时间：2010.10.24
说明：线程局部对象

调用关系：
thread_local_ptr tlp;
tlp.set_destructor_cb(this);
.
.
tlp.set_value(p);
tlp.get_value();
.
.
.
tlp.set_destructor_cb(NULL)


void xxx::on_tlp_free_resource(int threadid, void* value, ptr_int user_data)
{
	delete value;
}

*/


#ifndef __BASICTOOL_THREAD_LOCAL_PTR__
#define __BASICTOOL_THREAD_LOCAL_PTR__

#include "public_include.h"
#include "mutex.h"
#include "thread_exit.h"
#include "stl.h"

namespace bt
{
	class tlp_listener
	{
	public:
		//注意，回调线程不是当初使用的线程，不能在回调里面set_value(NULL)
		virtual void on_tlp_free_resource(int threadid, void* value, ptr_int user_data) = 0;
	};

	class thread_local_ptr : public thread_exit_listener
	{
	public:
		thread_local_ptr();
		~thread_local_ptr();

	public:
		void* get_value();
		void  set_value(void* value);

		void  set_destructor_cb(tlp_listener* lsner, int user_date = 0);	//使用完必须调用set_destructor_cb(NULL);销毁还存在的值。

	protected:
		virtual void on_thread_exit(int threadid);
		virtual void on_thread_free_resource(int threadid);

	protected:
#if defined(OS_WIN32)
		DWORD _dwTlsIndex;
#elif defined(OS_LINUX)
		pthread_key_t _key;
#endif

		tlp_listener* _lsner;
		int _user_date;

		mutex _mtx;
		std::map<int, void*> _all_value;
	};
}

#endif //__BASICTOOL_THREAD_LOCAL_PTR__

