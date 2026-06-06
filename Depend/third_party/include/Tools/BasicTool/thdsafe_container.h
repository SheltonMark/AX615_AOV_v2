
/*
作者：文昌鱼
时间：2011.05.03
说明：线程类，不是线程安全


*/


#ifndef __BASICTOOL_THDSAFE_CONTAINER__
#define __BASICTOOL_THDSAFE_CONTAINER__

#include "public_include.h"
#include "btstl_container.h"

namespace bt
{
	template<class KEY, class TYPE, class LOCK_TYPE>
	class refer_map
	{
	public:
		typedef typename btmap<KEY, TYPE*>::iterator iterator;

	public:
		refer_map();
		~refer_map();

	public:
		void insert(KEY key, TYPE* dat);		//加入成功会加1索引
		TYPE* find(KEY key);					//自动增加索引
		void erase(KEY key);					//删除成功会减索引
		void clear();

		void iterate_lock();
		iterator begin();
		iterator end();
		void iterate_unlock();

	protected:
		LOCK_TYPE _mtx;
		btmap<KEY, TYPE*> _data;
	};

}

#include "thdsafe_container.inl"

#endif //__BASICTOOL_THDSAFE_CONTAINER__

