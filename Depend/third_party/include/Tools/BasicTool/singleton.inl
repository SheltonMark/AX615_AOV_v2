#include "singleton_lock.h"
#include "osapi.h"

namespace bt
{
	template<class T>
	T* singleton<T>::_instance = NULL;

	//在对象还没创建的时候，会有多线程同步的问题。必须在创建其他线程的时候，先调用一次
	template<class T>
	inline T* singleton<T>::instance()
	{
		if (_instance == NULL)
		{
			//解决这个函数调用频繁，展开后代码体积大的问题
			_new_instance();
		}
		return _instance;
	}

	//最好在所有子线程关闭后调用
	template<class T>
	inline void singleton<T>::uninstance()
	{
		_delete_instance();
	}

	template<class T>
	void singleton<T>::_new_instance()
	{
		//如果这个地方死锁，说明不加锁的时候会创建两个全局对象，有不可预料的错误。
		singleton_lock::instance()->lock(&_instance);
		if (_instance == NULL)
		{
			//保持先构造再赋值
			T* tmp = new T;
			compiler_barrier();
			_instance = tmp;
		}
		singleton_lock::instance()->unlock(&_instance);
	}

	template<class T>
	void singleton<T>::_delete_instance()
	{
		singleton_lock::instance()->lock(&_instance);
		BT_SAFE_DELETE(_instance);
		singleton_lock::instance()->unlock(&_instance);
	}
}

