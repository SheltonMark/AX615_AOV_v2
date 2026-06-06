
/*
作者：文昌鱼
时间：2010.06.23
说明：单件类

时间：2012.02.26
说明：解决单件类在多线程下会创建多个对象的隐患。

*/


#ifndef __BASICTOOL_SINGLETON__
#define __BASICTOOL_SINGLETON__

#include "define.h"

namespace bt
{
	//////////////////////////////////////////////////////////////////////////////////////
	template<class T>
	class singleton
	{
	public:
		inline static T* instance();
		inline static void uninstance();

	protected:
		static void _new_instance();
		static void _delete_instance();

	private:
		static T* _instance;
	};

} //bt

#include "singleton.inl"

#endif //__BASICTOOL_SINGLETON__

