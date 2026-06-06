
/*
作者：文昌鱼
时间：2010.06.15
说明：定义锁的读写互斥体
*/


#ifndef __BASICTOOL_RWMUTEX__
#define __BASICTOOL_RWMUTEX__

#include "lock.h"

namespace bt
{

	class rwmutex
	{
	public:
		class read_mutex : public base_lock
		{
		public:
			read_mutex(rwmutex* rw);
			virtual void virtual_lock();
			virtual bool virtual_try_lock();
			virtual void virtual_unlock();
			void lock();
			bool try_lock();
			void unlock();
		private:
			rwmutex* _rw;
		};

		class write_mutex : public base_lock
		{
		public:
			write_mutex(rwmutex* rw);
			virtual void virtual_lock();
			virtual bool virtual_try_lock();
			virtual void virtual_unlock();
			void lock();
			bool try_lock();
			void unlock();
		private:
			rwmutex* _rw;
		};

	public:
		rwmutex();
		~rwmutex();

		read_mutex& get_read_mutex()		{return _read_mutex;}
		write_mutex& get_write_mutex()		{return _write_mutex;}

	protected:
		void _read_lock();
		bool _read_trylock();
		void _read_unlock();

		void _write_lock();
		bool _write_trylock();
		void _write_unlock();

	protected:
		read_mutex _read_mutex;
		write_mutex _write_mutex;

	private:
#if defined(OS_WIN32)
		CRITICAL_SECTION _cs_write;
		CRITICAL_SECTION _cs_read;
		int _read_count;
#elif defined(OS_LINUX)
		pthread_rwlock_t _rwlock_h;
#endif
	};

} //namespace bt

#endif //__BASICTOOL_RWMUTEX__

