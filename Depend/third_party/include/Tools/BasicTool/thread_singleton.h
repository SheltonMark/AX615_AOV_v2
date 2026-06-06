
/*
作者：文昌鱼
时间：2010.06.23
说明：单件类

时间：2012.02.26
说明：解决单件类在多线程下会创建多个对象的隐患。

*/


#ifndef __BASICTOOL_THREAD_SINGLETON__
#define __BASICTOOL_THREAD_SINGLETON__

#include "define.h"
#include "thread_local_ptr.h"
#include "mutex.h"

namespace bt
{

	template<class T>
	class thread_singleton_tlp : public thread_local_ptr, public tlp_listener
	{
	public:
		thread_singleton_tlp();
	protected:
		virtual void on_tlp_free_resource(int threadid, void* value, ptr_int user_data);
	};

	template<class T>
	class thread_singleton
	{
	public:
		inline static T* instance();
		//最好在所有子线程关闭后调用
		inline static void uninstance();

	protected:
		static T* _new_instance();
		static void _delete_instance();

	private:
		static thread_singleton_tlp<T> _instance;
	};


} //bt

#include "thread_singleton.inl"

#endif //__BASICTOOL_THREAD_SINGLETON__

