
/*
作者：文昌鱼
时间：2010.06.13
说明：

时间：2010.07.10
修改：LOCK_TYPE增加LOCK_TYPE模板，可以选择锁方式

时间：2010.10.17
修改：增加一个最简单的轮询计时器

时间：2011.04.16
修改：用get_usec_tick().


时间：2011.04.21
修改：对计时器做大的整改，增加HASH模式计时器

*/


#ifndef __BASICTOOL_TIMER__
#define __BASICTOOL_TIMER__

#include "heartbeat.h"
#include "refer.h"
#include "btstl_container.h"
#include "osapi.h"
#include "dyna_lock.h"
#include "_timer_kernal.h"
#include "callback.h"
#include "table.h"
#include "delay_del_object.h"

#if defined(OS_WIN32)
#pragma warning(push)
#pragma warning(disable:4311)
//#pragma warning(disable:4267)
#endif

namespace bt
{

	/////////////////////////////////////////////////////////////////////////////////////////////
	//计时器监听类
	class timer_listener
	{
	public:
		virtual void ontime(int timer_id, bt::ptr_int param) = 0;
	};

	/////////////////////////////////////////////////////////////////////////////////////////////
	//普通计时器，一般用在单个类。数量级0(lgn)。注意：ID多大，内存就会多大。
	//本来考虑用hashmap代替table,但是hashmap在Linux下占用的内存太大，不适合小对象使用
	class timer : public heartbeat_body, public timer_kernal_parent
	{
	public:
		timer();
		virtual ~timer();

	//重载
	protected:
		virtual void on_kernal_active_time(uint64_t cur_time, uint64_t act_time, int data);
	public:
		virtual int heartbeat(ptr_int param);

	//接口
	public:
		void set_listener(timer_listener* lsner);
		void set_lock_type(dyna_lock::DYNA_LOCK_TYPE type);
		void set_watch_interval(uint interval = 100);

		void set_timer(int timer_id, uint delay, uint interval = 0, ptr_int param = 0);
		void kill_timer(int timer_id);

	//数据
	protected:
		dyna_lock _mtx;
		watch_timer _watch;
		timer_listener* _lsner;

		struct timer_item
		{
			ptr_int			param;

			uint			interval;
			uint64_t		next_active_time;
		};
		table<timer_item> _timer_map;

		struct active_item
		{
			int timer_id;
			ptr_int param;
		};
		btdeque<active_item> _active;

		timer_kernal_map _kernal;
	};

	/////////////////////////////////////////////////////////////////////////////////////////////

	class complex_timer : public heartbeat_body, public timer_kernal_parent
	{
	public:
		complex_timer();
		virtual ~complex_timer();

	//重载
	protected:
		virtual void on_kernal_active_time(uint64_t cur_time, uint64_t act_time, int data);
	public:
		virtual int heartbeat(ptr_int param);

	//
	public:
		void set_lock_type(dyna_lock::DYNA_LOCK_TYPE type);
		void enable_hash();											//增加性能，但占用内存
		void enable_big_hash();										//支持小于100秒的计时器
		void set_watch_interval(uint interval = 10);

		/*
		参数:
		ref_lsner:防止调用回调时，对象已经销毁或正在销毁
			即使在同一线程，对象销毁时记得调用kill_timer和kill_timeout，要不然ref_lsner不能为空
		*/
		void set_timer(timer_listener* lsner, int timer_id, uint delay = 0, uint interval = 0, ptr_int param = 0, refer* ref_lsner = NULL);
		void kill_timer(timer_listener* lsner, int timer_id);
		int  set_timeout(timer_listener* lsner, int timer_id, uint delay, ptr_int param = 0, refer* ref_lsner = NULL);
		void kill_timeout(timer_listener* lsner, int timer_id, int inside_timer_id);						//inside_timer_id为set_timeout的返回值，大于等于0
		void clear();

		uint get_timer_count();
		uint get_active_times();

	//内部接口
	protected:
		int  new_timer_id();
		void add_timer_to_kernal(uint64_t act_time, uint data, uint interval);
		void del_timer_from_kernal(uint64_t act_time, uint data, uint interval);

	//数据
	protected:
		dyna_lock	_mtx;
		watch_timer _watch;

		struct timer_key
		{
			void*			lsner;
			int				timer_id;
		};
		struct timer_key_less
		{
			bool operator()(const timer_key& _Left, const timer_key& _Right) const;
		};
		struct timer_item
		{
			callback_variable_ex<timer_listener, ptr_int> cb;
			
			int				timer_id;
			uint			interval;
			uint64_t		next_active_time;
		};
		table<timer_item> _timer_map;
		btmap<timer_key, int, timer_key_less> _help_map;

		struct active_item
		{
			int timer_id;
			call_callback_ex<timer_listener, ptr_int> cb;
		};
		btdeque<active_item> _active;
		uint _active_times;

		timer_kernal_map _kernal;
		timer_kernal_hash* _hash_kernal[2];
		uint _hash_max_interval;

		//将销毁的索引
		//delay_dec_refer _delay_dec;
	};

	class safe_sub_timer
	{
	public:
		safe_sub_timer();
		~safe_sub_timer();

	public:
		/*
		参数:同上
		*/
		void init(complex_timer* timer, timer_listener* lsner, int timer_id, refer* ref_lsner);

		void set_timer(uint delay = 0, uint interval = 0, ptr_int param = 0);
		void kill_timer();
		void set_timeout(uint delay, ptr_int param = 0);
		void kill_timeout();

	protected:
		void kill_timer_or_timeout();

	protected:
		complex_timer* _timer;
		timer_listener* _lsner;
		int _timer_id;
		refer* _ref_lsner;
		int _inside_timer_id;		//>=0timeout,==-1无,<-1timer
	};
}

#if defined(OS_WIN32)
#pragma warning(pop)
#endif

#endif //__BASICTOOL_TIMER__

