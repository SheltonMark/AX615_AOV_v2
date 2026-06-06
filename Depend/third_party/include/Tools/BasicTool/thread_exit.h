/*
线程关闭回调管理类

*/

#ifndef __BASICTOOL_THREAD_EXIT__
#define __BASICTOOL_THREAD_EXIT__

#include "public_include.h"
#include "mutex.h"
#include "stl.h"
#include "singleton.h"

namespace bt
{
	class thread_exit_listener
	{
	public:
		virtual void on_thread_exit(int threadid) = 0;						//只有用THREAD类的线程才有这个回调
		virtual void on_thread_free_resource(int threadid) = 0;				//为线程局部变量销毁资源用的
	};

	class thread_exit : public singleton<thread_exit>
	{
	public:
		thread_exit();
		~thread_exit();

	public:
		//这两个接口和回调用的是同样的锁，调用的时尽量不要加锁
		void add_listener(thread_exit_listener* lsner);
		void del_listener(thread_exit_listener* lsner);

		//不是从thread重载的线程，线程关闭的时候都要调用这个。可以关闭一些依赖线程的资源
		void notify_thread_exit(int threadid);
		void notify_thread_free_resource(int threadid);
		//void notify_all_thread_free_resource();

	protected:
		mutex _mtx;
		bool _modify;
		std::set<thread_exit_listener*> _data;
	};

}

#endif //__BASICTOOL_THREAD_EXIT__

