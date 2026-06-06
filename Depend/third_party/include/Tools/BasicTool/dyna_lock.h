
#ifndef __BASICTOOL_DYNA_LOCK__
#define __BASICTOOL_DYNA_LOCK__

#include "public_include.h"
#include "lock.h"

namespace bt
{
	class dyna_lock : public base_lock
	{
	public:
		dyna_lock();
		~dyna_lock();

	public:
		virtual void virtual_lock();
		virtual bool virtual_try_lock();
		virtual void virtual_unlock();

		void lock();
		bool try_lock();
		void unlock();

		enum DYNA_LOCK_TYPE
		{
			LOCK_TYPE_NULL = 0,
			LOCK_TYPE_MUTEX,
			LOCK_TYPE_SPIN_MUTEX,
		};
		void set_lock_type(DYNA_LOCK_TYPE type);

	protected:
		static null_lock _null_lock;
		base_lock* _inside_lock;
	};

	typedef auto_lock<dyna_lock> auto_lock_dyna;
}

#endif //__BASICTOOL_DYNA_LOCK__

