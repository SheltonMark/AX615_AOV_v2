
/*
作者：文昌鱼
时间：2010.07.25
说明：多线程类
*/


#ifndef __BASICTOOL_MULTI_THREAD__
#define __BASICTOOL_MULTI_THREAD__

#include "public_include.h"
#include "thread.h"

namespace bt
{

	class multi_thread : public thread_body
	{
	public:
		multi_thread(void);
		virtual ~multi_thread(void);

	public:
		//设置线程数。
		void set_thread_num(uint num);
		thread* get_thread(uint index);

		//设置回调
		void set_body(thread_body* body, ptr_int user_data);

		//动作
		bool start(uint stack_size = 0);
		void stop();
		void set_priority(int priority);

		//状态
		int  get_state() const;

	//重载
	protected:
		virtual void run(ptr_int user_data);

	//内部函数
	protected:
		void set_state(int state);

	protected:
		std::vector<thread*> _thread;
		volatile int _state;
		thread_body* _body;
		ptr_int _user_data;
	};

} //namespace bt



#endif //__BASICTOOL_MULTI_THREAD__


