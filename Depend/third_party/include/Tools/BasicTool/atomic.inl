
namespace bt
{
	inline atomic::atomic()
	{
		_count = 0;
	}

	inline atomic::~atomic()
	{
	}

	inline void atomic::set_count(sint32 count)
	{
		_count = count;
	}

	inline sint32  atomic::get_count() const
	{
		return _count;
	}

	inline volatile sint32& atomic::count()
	{
		return _count;
	}

	inline sint32  atomic::inc_count()
	{
#if defined(BT_SUPPORT_ATOMIC)
	#if defined(OS_WIN32)
		return InterlockedIncrement((long*)&_count);
	#elif defined(OS_LINUX)
		return __sync_add_and_fetch(&_count, 1);
	#endif
#else
		auto_lock<mutex> l(_mtx);
		return ++_count;
#endif
	}

	inline sint32  atomic::dec_count()
	{
#if defined(BT_SUPPORT_ATOMIC)
	#if defined(OS_WIN32)
		return InterlockedDecrement((long*)&_count);
	#elif defined(OS_LINUX)
		return __sync_sub_and_fetch(&_count, 1);
	#endif
#else
		auto_lock<mutex> l(_mtx);
		return --_count;
#endif
	}

	inline sint32  atomic::comp_exchange(sint32 comp, sint32 exchange)
	{
#if defined(BT_SUPPORT_ATOMIC)
	#if defined(OS_WIN32)
		return InterlockedCompareExchange((long*)&_count, exchange, comp);
	#elif defined(OS_LINUX)
		return __sync_val_compare_and_swap((long*)&_count, comp, exchange);
	#endif
#else
		auto_lock<mutex> l(_mtx);
		sint32 ret = _count;
		if (_count == comp)
		{
			_count = exchange;
		}
		return ret;
#endif
	}

	inline sint32  atomic::exchange(sint32 exchange)
	{
#if defined(BT_SUPPORT_ATOMIC)
	#if defined(OS_WIN32)
		return InterlockedExchange((long*)&_count, exchange);
	#elif defined(OS_LINUX)
		return __sync_lock_test_and_set(&_count, exchange);
	#endif
#else
		auto_lock<mutex> l(_mtx);
		sint32 ret = _count;
		_count = exchange;
		return ret;
#endif
	}

	inline sint32  atomic::exchange_add(sint32 add)
	{
#if defined(BT_SUPPORT_ATOMIC)
	#if defined(OS_WIN32)
		return InterlockedExchangeAdd((long*)&_count, add);
	#elif defined(OS_LINUX)
		return __sync_fetch_and_add(&_count, add);
	#endif
#else
		auto_lock<mutex> l(_mtx);
		sint32 ret = _count;
		_count += add;
		return ret;
#endif
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	inline false_atomic::false_atomic()
	{
		_count = 0;
	}

	inline false_atomic::~false_atomic()
	{
	}

	inline void false_atomic::set_count(sint32 count)
	{
		_count = count;
	}

	inline sint32  false_atomic::get_count() const
	{
		return _count;
	}

	inline volatile sint32& false_atomic::count()
	{
		return _count;
	}

	inline sint32  false_atomic::inc_count()
	{
		return ++_count;
	}

	inline sint32  false_atomic::dec_count()
	{
		return --_count;
	}

	inline sint32  false_atomic::comp_exchange(sint32 comp, sint32 exchange)
	{
		sint32 ret = _count;
		if (_count == comp)
		{
			_count = exchange;
		}
		return ret;
	}

	inline sint32  false_atomic::exchange(sint32 exchange)
	{
		sint32 ret = _count;
		_count = exchange;
		return ret;
	}

	inline sint32  false_atomic::exchange_add(sint32 add)
	{
		sint32 ret = _count;
		_count += add;
		return ret;
	}

}

