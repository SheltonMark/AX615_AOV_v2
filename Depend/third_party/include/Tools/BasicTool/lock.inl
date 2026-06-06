

namespace bt
{
	template<class LOCK_TYPE>
	inline auto_lock<LOCK_TYPE>::auto_lock(LOCK_TYPE& l, bool init_lock)
		:_lock(l), _locked(false)
	{
		if (init_lock)
		{
			lock();
		}
	}

	template<class LOCK_TYPE>
	inline auto_lock<LOCK_TYPE>::~auto_lock()
	{
		unlock();
	}

	template<class LOCK_TYPE>
	inline void auto_lock<LOCK_TYPE>::lock()
	{
		if (_locked == false)
		{
			_locked = true;
			_lock.LOCK_TYPE::lock();			//直接调用实函数，不调用虚函数
		}
	}

	template<class LOCK_TYPE>
	inline void auto_lock<LOCK_TYPE>::unlock()
	{
		if (_locked)
		{
			_locked = false;
			_lock.LOCK_TYPE::unlock();
		}
	}


} //namespace bt

