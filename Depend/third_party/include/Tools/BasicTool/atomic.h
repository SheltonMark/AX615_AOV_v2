
/*
作者：文昌鱼
时间：2010.06.13
说明：定义系统的原子操作类

时间：2010.09.25
说明：添加一个count接口，方便直接对原子计数操作

时间：2010.09.25
说明：false_atomic替代单线程下的原子操作

时间：2011.09.19
说明：有些心得体会，请看lock.h

*/


#ifndef __BASICTOOL_ATOMIC__
#define __BASICTOOL_ATOMIC__

#include "public_include.h"
#include "mutex.h"

namespace bt
{
	class atomic
	{
	public:
		atomic();
		~atomic();

		//不加锁的操作
		void set_count(long count);
		long  get_count() const;							//这个加不加锁的调用是一样的，因为每个变化都会刷新到各个CPU
		volatile long& count();

		//加锁
		long  inc_count();
		long  dec_count();
		long  comp_exchange(long comp, long exchange);		//返回变化前的值
		long  exchange(long exchange);						//同上
		long  exchange_add(long add);						//同上

#if !defined(BT_SUPPORT_ATOMIC)
		mutex& get_mutex();
#endif

	private:
		volatile long _count;
	};

	//用来代替单线程下的原子操作，提高性能
	class false_atomic
	{
	public:
		false_atomic();
		~false_atomic();

		void set_count(long count);
		long  get_count() const;
		volatile long& count();

		long  inc_count();
		long  dec_count();
		long  comp_exchange(long comp, long exchange);		//返回变化前的值
		long  exchange(long exchange);						//同上
		long  exchange_add(long add);						//同上
	private:
		volatile long _count;
	};

} //namespace bt

#endif //__BASICTOOL_ATOMIC__


