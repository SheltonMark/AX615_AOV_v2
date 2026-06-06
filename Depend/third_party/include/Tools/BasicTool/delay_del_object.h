/*
作者：文昌鱼
时间：2011.02.22
说明：这个其实是解决回调开销而准备的。比如说timer回调，每次回调需要加引用索引，后又减引用索引，增加无谓的开销。
用索引是防止回调指针实效，怕回调的过程中指针销毁。而延时销毁，保证回调时间绝对不会超过延时销毁的间隔，这样就不需要每次都增加减少引用索引。
默认延时2秒

有个副作用，这个对象所占用的内存会延时一段时间销毁，所以频率相当高的，不建议使用。

针对回调的引用索引，补充一下：在回调函数调用的同一个线程里延时销毁，就不会有漏洞存在

*/

#ifndef __BASICTOOL_DELAY_DEL_OBJECT__
#define __BASICTOOL_DELAY_DEL_OBJECT__

#include "public_include.h"
#include "heartbeat.h"
#include "dyna_lock.h"
#include "btstl_container.h"
#include "refer.h"
#include "watch_timer.h"

namespace bt
{
	template<class TYPE>
	class delay_del_object : public heartbeat_body
	{
	public:
		delay_del_object();
		virtual ~delay_del_object();

	public:
		virtual int heartbeat(ptr_int param);

	public:
		void delay_del(TYPE obj);
		void del_all();

		void set_delay_interval(uint interval);
		void set_lock_type(dyna_lock::DYNA_LOCK_TYPE type);
		nint get_obj_size();

	protected:
		virtual void del_object(TYPE obj) = 0;

	protected:
		dyna_lock _mtx;
		btlist<TYPE> _obj;
		btlist<TYPE> _obj2;
		watch_timer _timer;
	};

	template<class TYPE>
	class delay_del_point : public delay_del_object<TYPE>
	{
	public:
		virtual ~delay_del_point()
		{
			delay_del_object<TYPE>::del_all();
		}
	protected:
		virtual void del_object(TYPE obj)
		{
			if (obj)
			{
				delete obj;
			}
		}
	};

	class delay_dec_refer : public delay_del_object<bt::refer*>
	{
	public:
		virtual ~delay_dec_refer()
		{
			delay_del_object<bt::refer*>::del_all();
		}
	protected:
		virtual void del_object(bt::refer* obj)
		{
			if (obj)
			{
				obj->dec_ref();
			}
		}
	};
}

#include "delay_del_object.inl"

#endif //__BASICTOOL_DELAY_DEL_OBJECT__

