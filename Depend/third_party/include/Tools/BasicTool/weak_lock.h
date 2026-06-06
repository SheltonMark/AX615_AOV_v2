

/*
作者：文昌鱼
时间：2015.03.04
说明：弱锁。降低锁调用的代价，加大锁冲突的代价。所以要用在锁冲突小的地方。

weak_lock使用场景：
大部分时间在一个线程操作，偶尔在其他线程操作。使用的时候要区分线程（大部分在一个线程工作）

weak_spin_lock使用场景：
多个线程操作，极少的锁冲突。（多线程冲突小）

weak_rw_lock使用场景：
大部分时间在读，偶尔写。(读写冲突小)

*/

#ifndef __BASICTOOL_WEAK_LOCK__
#define __BASICTOOL_WEAK_LOCK__

#include "public_include.h"
#include "atomic.h"
#include "mutex.h"
#include "lock.h"
#include "sema.h"

namespace bt
{
	class weak_lock
	{
	public:
		class my_mutex : public base_lock
		{
		public:
			my_mutex(weak_lock* rw);
			virtual void virtual_lock();
			virtual bool virtual_try_lock();
			virtual void virtual_unlock();
			void lock();
			bool try_lock();
			void unlock();
		private:
			weak_lock* _rw;
		};

		class other_mutex : public base_lock
		{
		public:
			other_mutex(weak_lock* rw);
			virtual void virtual_lock();
			virtual bool virtual_try_lock();
			virtual void virtual_unlock();
			void lock();
			bool try_lock();
			void unlock();
		private:
			weak_lock* _rw;
		};

	public:
		weak_lock();

	public:
		void my_lock();
		bool my_try_lock();
		void my_unlock();

		void other_lock();
		bool other_try_lock();
		void other_unlock();

		my_mutex& get_my_lock();
		other_mutex& get_other_lock();

	public:
		atomic _my_state;			//0表示未进入，1进入
		atomic _other_state;		//0表示未进入，1进入
		mutex _other_mtx;

		my_mutex _my_mutex;
		other_mutex _other_mutex;
	};

	class weak_spin_lock : public base_lock
	{
	public:
		weak_spin_lock();

	public:
		virtual void virtual_lock();
		virtual bool virtual_try_lock();
		virtual void virtual_unlock();

		void lock();
		bool try_lock();
		void unlock();

	private:
		atomic _at;				//等于0表示无锁
	};

	class weak_rw_lock
	{
	public:
		class read_mutex : public base_lock
		{
		public:
			read_mutex(weak_rw_lock* rw);
			virtual void virtual_lock();
			virtual bool virtual_try_lock();
			virtual void virtual_unlock();
			void lock();
			bool try_lock();
			void unlock();
		private:
			weak_rw_lock* _rw;
		};

		class write_mutex : public base_lock
		{
		public:
			write_mutex(weak_rw_lock* rw);
			virtual void virtual_lock();
			virtual bool virtual_try_lock();
			virtual void virtual_unlock();
			void lock();
			bool try_lock();
			void unlock();
		private:
			weak_rw_lock* _rw;
		};
	public:
		weak_rw_lock();

	public:
		void read_lock();
		bool read_try_lock();
		void read_unlock();

		void write_lock();
		bool write_try_lock();
		void write_unlock();

		read_mutex& get_read_lock();
		write_mutex& get_write_lock();

	public:
		atomic _read_state;			//0表示未进入，>0表示进入
		atomic _write_state;		//0表示未进入，1进入
		mutex _write_mtx;

		read_mutex _read_mutex;
		write_mutex _write_mutex;
	};

	//class weak_lock2
	//{
	//public:
	//	weak_lock2();

	//public:
	//	void my_lock();
	//	void my_unlock();

	//	void other_lock();
	//	void other_unlock();

	//public:
	//	atomic _my_state;		//0表示未进入，1表示锁判断，2进入
	//	atomic _other_state;	//0表示未进入，1进入
	//};
}


#endif