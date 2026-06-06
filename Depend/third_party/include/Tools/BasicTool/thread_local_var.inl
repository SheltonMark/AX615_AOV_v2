
namespace bt
{
	inline thread_local_var::thread_local_var()
	{
#if defined(OS_WIN32)
		_dwTlsIndex = TlsAlloc();
#elif defined(OS_LINUX)
		pthread_key_create(&_key, NULL);
#endif
	}

	inline thread_local_var::~thread_local_var()
	{
#if defined(OS_WIN32)
		TlsFree(_dwTlsIndex);
#elif defined(OS_LINUX)
		pthread_key_delete(_key);
#endif
	}

	inline void* thread_local_var::get_value()
	{
#if defined(OS_WIN32)
		return (void*)TlsGetValue(_dwTlsIndex);
#elif defined(OS_LINUX)
		return pthread_getspecific(_key);
#endif
	}

	inline void  thread_local_var::set_value(void* value)
	{
#if defined(OS_WIN32)
		TlsSetValue(_dwTlsIndex, value);
#elif defined(OS_LINUX)
		pthread_setspecific(_key, value);
#endif
	}
}

