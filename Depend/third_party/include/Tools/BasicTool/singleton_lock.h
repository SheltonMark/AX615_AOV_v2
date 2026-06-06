
/*
作者：文昌鱼
时间：2012.02.26
说明：解决单件类在多线程下会创建多个对象的隐患。

这里面有个很小概率的隐患，但是为了解决前面那个隐患，没办法了。
隐患：singleton_lock::_instance构造启动另一个线程，而且另一个线程在调用_instance_impl()，而且和另一个线程同时构建同一个单件类
另外，即使singleton_lock_impl创建两个也关系不大。


*/


#ifndef __BASICTOOL_SINGLETON_LOCK__
#define __BASICTOOL_SINGLETON_LOCK__

#include "define.h"
#include "mutex.h"
#include "stl.h"

namespace bt
{
	//实现简单的指针锁
	class singleton_lock_impl
	{
	public:
		singleton_lock_impl();

	public:
		void lock(void* ppsingleton);		//存放单件类指针的地址
		void unlock(void* ppsingleton);

	protected:
		mutex _mtx;
		std::set<void*> _ppsingleton;
	};


	//
	class singleton_lock
	{
	public:
		singleton_lock();

	//接口
	public:
		static singleton_lock* instance()
		{
			return &_instance;
		}

		void lock(void* ppsingleton);
		void unlock(void* ppsingleton);


	//内部函数和数据
	protected:
		singleton_lock_impl* _instance_impl();

	protected:
		static singleton_lock _instance;
		singleton_lock_impl* _impl;
	};

}


#endif


