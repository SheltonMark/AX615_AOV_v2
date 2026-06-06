
/*
作者：文昌鱼
时间：2010.07.09
说明：管道
*/


#ifndef __BASICTOOL_PIPELINE__
#define __BASICTOOL_PIPELINE__

#include "heartbeat.h"
#include "event.h"
#include "sema.h"
#include "nolock_queue.h"
#include "btstl_container.h"

namespace bt
{	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//减少多个线程同时访问event的锁冲突
	class pipeline_event
	{
	public:
		pipeline_event();

	public:
		void active();
		bool wait(uint timeout = BT_MAX_UINT);

	protected:
		//event _e;				//在LINUX下锁冲突
		sema _e;
		bool _wait;
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<class OBJECT_TYPE>
	class pipeline_dealwith
	{
	public:
		virtual void on_dealwith_object(OBJECT_TYPE const& obj, bt::ptr_int param) = 0;
	};

	template<class OBJECT_TYPE>
	class pipeline : public heartbeat_body, public pipeline_dealwith<OBJECT_TYPE>
	{
	public:
		pipeline();
		virtual ~pipeline();

	//heartbeat_body
	public:
		virtual int heartbeat(ptr_int param);

	//pipeline_dealwith
	protected:
		virtual void on_dealwith_object(OBJECT_TYPE const& obj, ptr_int param);

	//接口
	public:
		void post_object(OBJECT_TYPE const& obj, bool active = true);
		void post_object(OBJECT_TYPE const& obj, uint count, bool active = true);
		void dealwith_all_object();
		void set_callback(pipeline_dealwith<OBJECT_TYPE>* cb, ptr_int param);
		void set_wait_time(uint time);					//设置WAIT时间，才会等待
		void set_dealwith_count_ontimes(uint count);
		void set_max_object(uint size);
		int  get_object_size();

	//数据
	protected:
		hydra_queue<OBJECT_TYPE> _asy_list;
		uint _dealwith_count_ontimes;

		pipeline_event _asy_event;
		uint _wait_time;

		pipeline_dealwith<OBJECT_TYPE>* _callback;
		ptr_int _user_param;
	};

} //namespace bt

#include "pipeline.inl"

#endif //__BASICTOOL_PIPELINE__

