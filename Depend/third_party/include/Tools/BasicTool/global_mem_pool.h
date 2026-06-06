
/*
作者：文昌鱼
时间：2010.11.06
说明：全局内存池
*/
#ifndef __BASICTOOL_GLOBAL_MEMPOOL__
#define __BASICTOOL_GLOBAL_MEMPOOL__

#include "singleton.h"
#include "mem_pool.h"

namespace bt
{

#define BT_GLOBAL_POOL_NEW(p, type, param)\
	{\
		p = (type*)bt::global_mem_pool::instance()->new_mem(sizeof(type));\
		if (p)\
		{\
			BT_CALL_CONSTRUCT(p, type, param );\
		}\
	}

#define BT_GLOBAL_POOL_DELETE(p, type)\
	{\
		BT_CALL_DESTRUCTOR(p, type);\
		bt::global_mem_pool::instance()->delete_mem(p, sizeof(type));\
	}

	class global_mem_pool : public singleton<global_mem_pool>
	{
	public:
		//主要是解决，对于高出现频率global_mem_pool::instance()->new_mem展开的一种规避
		static void* static_new_mem(uint size);
		static void  static_delete_mem(void* mem, uint size);

	public:
		global_mem_pool();
		virtual ~global_mem_pool();

	public:
		//
		void* new_mem(uint size);
		void  delete_mem(void* mem, uint size);

		//内部函数
	protected:
		inline int get_pool_index(uint size);

	protected:
		const static uint SUB_POOL_NUM = 11;
		void* _sub_pool[SUB_POOL_NUM];
	};

}

#endif //__BASICTOOL_GLOBAL_MEMPOOL__

