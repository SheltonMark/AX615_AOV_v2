
/*
作者：文昌鱼
时间：2010.07.07
说明：自旋锁

时间：2010.09.25
说明：前面一个版本出现一个严重的BUG，同线程也不能两次LOCK。做修正

时间：2015.1.20
说明：自旋锁不是为了减少锁销毁的性嫩，而是减少锁冲突时线程切换

经测试，自旋锁（不管自己写的还是系统自带的）效果没系统锁好。可能自旋只有在关中断的情况下才有效果。所以自旋锁弃用。

所以与锁相关的性能有以下几点：
1. 减少锁的时间
2. 减少锁冲突，尽量避免全局锁
3. 可以尝试用读写锁

*/


#ifndef __BASICTOOL_SPINMUTEX__
#define __BASICTOOL_SPINMUTEX__

#include "lock.h"
#include "atomic.h"
#include "bt_statistics.h"

namespace bt
{

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class spin_mutex : public base_lock
	{
	public:
		spin_mutex();

	public:
		virtual void virtual_lock();
		virtual bool virtual_try_lock();
		virtual void virtual_unlock();
		
		void lock();
		bool try_lock();
		void unlock();

		virtual void set_spin_count(nint spin);

	private:
		atomic _at;				//等于0表示无锁
		long _own_thd_id;		/*
								说明：拥有锁的线程ID。考虑到线程同步的复杂性，特增加说明。

								修改值的时间：
									设置：加锁后。加锁前无法知道加锁是否成功，所以不能提前设置这个值，而且多个线程也会有冲突。
									清零：解锁前。否则，_own_thd_id可能为任何值，无意义。

								取值空间：
									加锁线程：解锁{0},加锁{拥有线程ID}
									其他线程：解锁{0},加锁{拥有线程ID,0}

								有些断言：
									1.（本线程ID==_own_thd_id）《==》（当前线程是加锁线程，处于加锁状态）
								*/
		nint _spin_count;
	};

	typedef auto_lock<spin_mutex> auto_lock_spin;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//linux下的自旋锁，不可重入，而且永远自旋，不会睡眠，这个不好。
	//性能提高1倍以上，只比一个原则操作多一点，所以怀疑解锁的时候没用原则草，那会影响加锁时的自旋次数。
#if defined(OS_LINUX)

	class system_spin_mutex : public base_lock
	{
	public:
		system_spin_mutex();
		virtual ~system_spin_mutex();

	public:
		virtual void virtual_lock();
		virtual bool virtual_try_lock();
		virtual void virtual_unlock();

		void lock();
		bool try_lock();
		void unlock();

	private:
		pthread_spinlock_t _lock;
	};

#elif defined(OS_WIN32)
	
	class system_spin_mutex : public base_lock
	{
	public:
		system_spin_mutex();
		virtual ~system_spin_mutex();

		virtual void virtual_lock();
		virtual bool virtual_try_lock();
		virtual void virtual_unlock();

		void lock();
		bool try_lock();
		void unlock();

	private:
		CRITICAL_SECTION _cs;
	};

#endif

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//重入的效果
	class mini_spin_mutex : public base_lock
	{
	public:
		mini_spin_mutex();

	public:
		virtual void virtual_lock();
		virtual bool virtual_try_lock();
		virtual void virtual_unlock();

		void lock();
		bool try_lock();
		void unlock();

		virtual void set_spin_count(nint spin);

	private:
		atomic _at;				//等于0表示无锁
		nint _spin_count;
	};

} //namespace bt

#endif //__BASICTOOL_SPINMUTEX__

