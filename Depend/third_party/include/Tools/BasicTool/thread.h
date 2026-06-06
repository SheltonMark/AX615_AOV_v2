
/*
作者：文昌鱼
时间：2010.06.12
说明：线程类，不是线程安全


作者：文昌鱼
时间：2010.11.13
说明：因为LINUX不支持线程暂停，移除相关接口

*/


#ifndef __BASICTOOL_THREAD__
#define __BASICTOOL_THREAD__

#include "public_include.h"
#include "thread_local_var.h"
#include "event.h"

namespace bt
{

	class thread_body
	{
	public:
		virtual void run(bt::ptr_int user_data) = 0;
	};

	class thread : public thread_body
	{
	public:
		thread();
		virtual ~thread();

		//设置回调
		void set_body(thread_body* body, ptr_int user_data);

		//动作
		bool start(uint stack_size = 0);
		void stop(int* exit_code = NULL);

		//状态
		enum
		{
			STATE_IDLE = 0,
			STATE_RUN,
			STATE_WILL_STOP,
			//STATE_IDLE
		};
		int  get_state() const;

		//优先级
		enum
		{
			PRIORITY_CLASS_NORMAL =				0x00000020,
			PRIORITY_CLASS_IDLE =				0x00000040,
			PRIORITY_CLASS_HIGH =				0x00000080,
			PRIORITY_CLASS_REALTIME =			0x00000100
		};
		void set_priority(int priority);

		//ID
		int get_thread_id() const;
		static int get_current_thread_id();

		//自己的ID，数值不具意义。为了快速表示线程
		long get_my_thread_id();
		static long get_my_current_thread_id();

		//得到当前线程的对象
		static thread* get_current_thread();

	//重载
	protected:
		virtual void run(ptr_int user_data);

	//真正的线程函数
	protected:
#if defined(OS_WIN32)
		static DWORD WINAPI static_thread_fun(LPVOID param);
#elif defined(OS_LINUX)
		static void* static_thread_fun(void* param);
#endif
		void thread_fun();

	//内部函数
	protected:
		void set_state(int state);

	//数据
	private:
		thread_body* _body;
		ptr_int _user_data;

		//状态
		volatile int _state;

		//线程对象
		static thread_local_var _my_this;

		//句柄
#if defined(OS_WIN32)
		HANDLE _thread_h;
#elif defined(OS_LINUX)
		pthread_t _thread;

		//等待启动对象
		event _wait_run;
#endif

		int _thread_id;
		long _my_thread_id;
	};

} //namespace bt



#endif //__BASICTOOL_THREAD__

