
/*
作者：文昌鱼
时间：2010.07.02
说明：


时间：2011.04.16
说明：添加heartbeat_container容器

*/


#ifndef __BASICTOOL_HEARTBEAT__
#define __BASICTOOL_HEARTBEAT__

#include "public_include.h"
#include "dyna_lock.h"
#include "btstl_container.h"

namespace bt
{

	class heartbeat_body
	{
	public:
		//返回值：<0错误，=0忙，>0空闲(毫秒数)
		virtual int heartbeat(bt::ptr_int param) = 0;
	};

	class heartbeat_container : public heartbeat_body
	{
	public:
		heartbeat_container();
		virtual ~heartbeat_container();

	public:
		virtual int heartbeat(ptr_int param);

	public:
		void add_heartbeat(heartbeat_body* hb, ptr_int param);
		void del_heartbeat(heartbeat_body* hb, ptr_int param);

		void set_lock_type(dyna_lock::DYNA_LOCK_TYPE type);

	protected:
		struct hb_item
		{
			heartbeat_body* hb;
			ptr_int param;

			nint last_call;
			nint expire;
		};
		dyna_lock _mtx;
		bool _modify;
		btdeque<hb_item> _data;
	};
}

#endif //__BASICTOOL_HEARTBEAT__

