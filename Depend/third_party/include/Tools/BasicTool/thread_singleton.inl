#include "singleton_lock.h"

namespace bt
{
	template<class T>
	thread_singleton_tlp<T>::thread_singleton_tlp()
	{
		set_destructor_cb(this);
	}

	template<class T>
	void thread_singleton_tlp<T>::on_tlp_free_resource(int threadid, void* value, ptr_int user_data)
	{
		thread_singleton<T>* p = (thread_singleton<T>*)value;
		delete p;
	}

	template<class T>
	thread_singleton_tlp<T> thread_singleton<T>::_instance;

	template<class T>
	inline T* thread_singleton<T>::instance()
	{
		T* t = (T*)_instance.get_value();
		if (t == NULL)
		{
			t = _new_instance();
		}
		return t;
	}

	//一定要在线程关闭的时候调用
	template<class T>
	inline void thread_singleton<T>::uninstance()
	{
		_delete_instance();
	}

	template<class T>
	T* thread_singleton<T>::_new_instance()
	{
		T* t = new T;
		_instance.set_value(t);
		return t;
	}

	template<class T>
	void thread_singleton<T>::_delete_instance()
	{
		T* t = (T*)_instance.get_value();
		if (t)
		{
			delete t;
			_instance.set_value(NULL);
		}
	}
}

