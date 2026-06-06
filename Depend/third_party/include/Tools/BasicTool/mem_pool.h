
/*
作者：文昌鱼
时间：2010.07.13
说明：内存池

时间：2010.09.21
说明：根据需求进行整改
功能说明：
1.外部销毁的功能？不支持，也不需要。
2.静态内存？要有。一构造就有可用内存，不需要NEW，提高效率
3.批量申请？要。提高性能，减少碎片。
4.跨内存池？这个可以支持。但与静态内存和批量申请冲突。是否写个简单的类，单独支持这个？不需要。
5.必须对内存池的容量做限定。
6.支持多线程。
7.支持各种类型。
注意：最好用自选锁。用临界区的性能和普通内存申请的性能差不多

时间：2010.11.03
说明：再次整改。提供一个让STL使用的内存池。

*/


#ifndef __BASICTOOL_MEMPOOL__
#define __BASICTOOL_MEMPOOL__

#include "public_include.h"
#include "stl.h"
#include "thread_local_ptr.h"
#include "spin_mutex.h"
#include "thread_exit.h"

namespace bt
{
	template<int ITEM_SIZE>
	class item_mem_proxy
	{
	public:
		char _data[ITEM_SIZE];
	};
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*
	需求：
	1.批量请求
	2.内存池申请的内存不可清除
	3.
	*/
	//class batch_mem_pool
	//{
	//public:
	//	batch_mem_pool(uint );
	//	~batch_mem_pool();

	//public:
	//	bool new_item(TYPE*& item, uint count = 1);				//只有两种结果，全部申请失败或全部申请成功
	//	void free_item(TYPE* const& item, uint count = 1);

	//};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	//template<class TYPE, class LOCK_TYPE, int BLOCK_SIZE, int STATIC_BLOCK_NUM = 0>
	//class mem_pool
	//{
	//public:
	//	mem_pool();
	//	~mem_pool();

	//public:
	//	bool new_item(TYPE*& item, uint count = 1);				//只有两种结果，全部申请失败或全部申请成功
	//	void free_item(TYPE* const& item, uint count = 1);
	//	void set_pool_size(uint size);
	//	void free_pool();

	////内部结构
	//protected:
	//	struct mem_node
	//	{
	//		TYPE data;

	//		uint index;
	//		mem_node* prev_free_node;
	//		mem_node* next_free_node;

	//		mem_node()
	//			:index(0), prev_free_node(NULL), next_free_node(NULL)
	//		{}
	//	};
	//	struct mem_block
	//	{
	//		mem_node node[BLOCK_SIZE];

	//		bool is_static;
	//		uint free_node_num;

	//		mem_block()
	//			:is_static(true), free_node_num(0)
	//		{
	//			for (uint i = 0;i < BLOCK_SIZE;i++) node[i].index = i;
	//		}
	//	};

	////内部函数
	//protected:
	//	mem_block* get_node_block(mem_node* node);
	//	void push_free_node(mem_node* node);
	//	mem_node* pop_free_node();
	//	void pop_free_node(mem_node* node);

	//	bool new_free_block(uint block_num);
	//	void remove_free_block(mem_block* block);

	//	mem_block* get_static_block();

	//protected:
	//	//
	//	LOCK_TYPE _mtx;

	//	//
	//	mem_node* _free_node;
	//	uint _free_node_num;
	//	uint _max_size;

	//	//
	//	char _static_block[sizeof(mem_block) * STATIC_BLOCK_NUM + 1];
	//};


	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*
	支持简单的内存重复使用
	*/
	template<class TYPE, class LOCK_TYPE>
	class simple_mem_pool
	{
	public:
		simple_mem_pool();
		virtual ~simple_mem_pool();

	public:
		bool new_item(TYPE*& item, uint count = 1);				//只有两种结果，全部申请失败或全部申请成功
		void delete_item(TYPE* const& item, uint count = 1);
		void set_pool_size(uint size);
		void free_pool();

	protected:
		struct mem_node
		{
			TYPE data;
			mem_node* next;
		};
		LOCK_TYPE _mtx;
		mem_node* _free_node;
		uint _free_num;
		uint _max_size;
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*
	功能：大部分不用锁、又可以在多线程使用、批量申请、支持数组
	缺点：没有最大堆积限制，不可删除
	*/
	template<class TYPE>
	class nolock_mem_pool : public tlp_listener
	{
	public:
		nolock_mem_pool();
		virtual ~nolock_mem_pool();

	public:
		//申请内存前调用。block_size:批量申请。backup_size:移到全局队列的大小。BT_MAX_UINT表示不修改
		void set_option(uint block_size = BT_MAX_UINT, uint backup_size = BT_MAX_UINT);
		void set_max_size(uint max_size);  //只有在block_size=1时有效
		void set_array_size(uint array_size);	//会自动更新block_size、backup_size

	public:
		bool new_item(TYPE*& item);				//只有两种结果，全部申请失败或全部申请成功
		void delete_item(TYPE* const& item);
		bool new_item(TYPE*& item, uint count);				//只有两种结果，全部申请失败或全部申请成功
		void delete_item(TYPE* const& item, uint count);

	//线程关闭
	protected:
		virtual void on_tlp_free_resource(int threadid, void* value, ptr_int user_data);

	//内部结构
	protected:
		struct mem_node
		{
			mem_node* next;
			TYPE data[1];
		};
		struct mem_node_list
		{
			mem_node* first;
			mem_node* last;
			uint size;

			mem_node_list();
			inline void push(mem_node* node);
			inline mem_node* pop();
			void push(mem_node_list& nl);
		};

	//内部函数
	protected:
		void push_global_backup_node(mem_node_list& nl);
		bool pop_global_backup_node(mem_node_list& nl);
		bool need_node_list(mem_node_list& nl);
		bool new_node_list(mem_node_list& nl);
		void delete_node_list(mem_node_list& nl);
		uint get_node_size();

		inline mem_node_list* get_thread_node_list();

		bool need_thread_node_list_size(mem_node_list* tnode, uint count);
		void dec_thread_node_list_size(mem_node_list* tnode);

	protected:
		uint _block_size;				//批量申请的大小
		uint _backup_size;				//
		uint _max_size;
		uint _array_size;

		thread_local_ptr _thread_node_list;

		mutex _global_backup_node_mtx[2];
		std::vector<mem_node_list> _global_backup_node[2];
		uint _global_node_size;
	};
}

#include "mem_pool.inl"

#endif //__BASICTOOL_MEMPOOL__

