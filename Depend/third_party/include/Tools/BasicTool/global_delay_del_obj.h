
#ifndef __BASICTOOL_GLOBAL_DELAY_DEL_OBJECT__
#define __BASICTOOL_GLOBAL_DELAY_DEL_OBJECT__

#include "public_include.h"
#include "singleton.h"
#include "mutex.h"
#include "thread.h"
#include "delay_del_object.h"

namespace bt
{

	class global_delay_del_obj : public singleton<global_delay_del_obj>, public thread_body
	{
	public:
		global_delay_del_obj();
		virtual ~global_delay_del_obj();

	protected:
		virtual void run(ptr_int user_data);

	public:
		void delay_del_obj(refer* ref);				//延时销毁某对象
		void del_all_obj();							//立刻销毁缓存队列里的所有对象

		void stop_thread();

	protected:
		mutex _mtx;
		thread _thread;
		delay_dec_refer _delay_op;
	};

}



#endif

