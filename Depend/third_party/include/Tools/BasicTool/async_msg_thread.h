

#ifndef __BASICTOOL_ASYNC_MSG_THREAD__
#define __BASICTOOL_ASYNC_MSG_THREAD__

#include "public_include.h"
#include "pipeline.h"

namespace bt
{
	template <class MSG>
	class async_msg_listener
	{
	public:
		virtual void on_async_msg(const MSG& msg) = 0;
	};

	template <class MSG>
	struct amt_async_msg
	{
		MSG msg;
		refer* ref_lsner;
		async_msg_listener<MSG>* lsner;
	};

	template <class MSG>
	class async_msg_thread : public pipeline<amt_async_msg<MSG> >
	{
	public:
		async_msg_thread();

	//heartbeat_body重载
	protected:
		virtual void on_dealwith_object(amt_async_msg<MSG> const& amsg, ptr_int param);

	//接口
	public:
		/*
		参数:
		ref_lsner:不用于其他的应用索引，通用情况下是为了在调用的时候防止对象被另一个线程销毁。
			而这里，因为消息发送后不能撤除，防止在消息传递的过程中对象被销毁，所以即使同一个线程，ref_lsner也不能为空
		*/
		void async_msg(const MSG& msg, async_msg_listener<MSG>* lsner, refer* ref_lsner);			//必须提供ref_lsner，防止非法指针
	};

	template <class MSG>
	class async_msg_postor
	{
	public:
		async_msg_postor();

	public:
		/*
		参数:
		ref_lsner:同上
		*/
		void init(async_msg_thread<MSG>* thread, async_msg_listener<MSG>* lsner, refer* ref_lsner);
		void post_msg(const MSG& msg);

	protected:
		async_msg_thread<MSG>* _thread;
		async_msg_listener<MSG>* _lsner;
		refer* _ref_lsner;
	};

	//template <class MSG>
	//class async_msg_box 
	//{

	//};
}

#include "async_msg_thread.inl"


#endif //__BASICTOOL_ASYNC_MSG_THREAD__


