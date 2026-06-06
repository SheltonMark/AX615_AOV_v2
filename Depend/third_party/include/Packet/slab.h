
/*
释放策略：
1. 先放入线程队列
2. 线程队列超过一定的大小，放入总队列
3. 总队列不够时，释放各个线程队列


申请策略：
1. 从线程队列请求
2. 如果线程队列没有，从总队列请求一批
3. 如果总队列一个都申请不过来，需要释放其他线程的队列

*/


#ifndef __PACKET_SLAB_INC__
#define __PACKET_SLAB_INC__

#include "basic_tool_mini.h"
#include "buddy.h"
#include "thread_local_ptr.h"
#include "weak_lock.h"
#include "mutex.h"
#include "nolock_queue.h"

namespace packet_memory
{
	class slab : public bt::tlp_listener
	{
		friend class packet_manager;
	public:
		slab(void);
		~slab(void);

	public:
		bool init(bt::nint mem_size, bt::nint init_size, bt::nint free_batch_num, bt::nint malloc_batch_num);
		bool uninit();
		//申请释放
		bt::nint malloc(bt::nint size, CPacket*& pkt);
		bt::nint malloc_ex(bt::nint size, CPacket*& pkt);			//可以不连续的块
		CPacket* malloc_empty_packet();
		void free(CPacket* pkt);

		void free_all_buffer();
		buddy& get_buddy();

		//直接调用BUDDY
		bt::nint buddy_malloc(bt::nint size, CPacket*& pkt);									//申请固定大小的块
		bt::nint buddy_malloc_ex(bt::nint size, bt::nint max_block_size, CPacket*& pkt);			//申请空间，大小无限制，要么申请成功，要么失败。
		bt::nint buddy_malloc_batch(bt::nint block_size, bt::nint block_num, CPacket*& pkt);		//申请固定大小的多个块。可能申请的结果小于block_num。
		void buddy_free(CPacket* pkt);
		CPacket* buddy_malloc_empty_packet(bt::nint num);

		//dump
		void dump();

	protected:
		virtual void on_tlp_free_resource(int threadid, void* value, long user_data);

	protected:
		//输入：单个线程，因为每个线程有一个对象
		//输出：1. 当前线程，数据量满的时候(不管移到总库，还是另一个线程的请求队列)
		//		2. 总库不够(低概率)
		struct free_thread_data
		{
			bt::weak_lock pop_mtx;		//冲突概率小
			const static int max_queue_size = 128;
			bt::two_lock_queue_ptr<bt::null_lock, bt::null_lock, CPacket*, 1> will_free;
		};

		//输入：1. 请求线程，队列为空的时候
		//输出：1. 请求线程
		//		2. 总库不够(低概率)
		struct malloc_thread_data
		{
			//mutex push_mtx;				//输入冲突概率大
			bt::weak_lock pop_mtx;			//冲突概率小
			const static int max_malloc = 6;
			const static int max_block_size = 32*1024;
			const static int max_queue_size = 256;
			bt::two_lock_queue_ptr<bt::null_lock, bt::null_lock, CPacket*, 1> will_malloc[max_malloc];	//1K/2K/4K/8K/16K/32K/64K
			int malloc_seq;	//记录申请频率

			//空PACKET的管理，只有在一个线程操作，不需要锁
			const static int empty_packet_malloc_batch_num = 128;
			CPacket* empty_packet_header;
			bt::nint empty_packet_num;
		};

		free_thread_data* new_free_thread_data();
		inline free_thread_data* get_free_thread_data();
		void del_free_thread_data(free_thread_data* ftd);
		void free_free_thread_data(free_thread_data* ftd, bool my_or_other, bool to_mallor);
		void free_free_thread_data(CPacket** pkt, bt::nint num);							//传进来的参数必须即是数组，也是队列
		void free_free_thread_data(CPacket* pkt);
		void free_all_free_thread_data();

		malloc_thread_data* new_malloc_thread_data();
		inline malloc_thread_data* get_malloc_thread_data();
		void del_malloc_thread_data(malloc_thread_data* mtd);
		void malloc_malloc_thread_data(malloc_thread_data* mtd, bt::nint index, bt::nint count);
		void free_malloc_thread_data(malloc_thread_data* mtd);
		void free_all_malloc_thread_data();
		void push_malloc_thread_data_unlock(malloc_thread_data* mtd, CPacket** pkt, bt::nint num);//传进来的参数必须即是数组，也是队列
		malloc_thread_data* get_active_malloc_thread_data_unlock(bt::nint dec_malloc_seq);

		void packet_vector_to_list(CPacket** pkt, bt::nint num);
		bt::nint packet_list_to_vector(CPacket* src, CPacket** pkt, bt::nint num);

	protected:
		bt::mutex _buddy_mtx;
		buddy _buddy;

		const static bt::nint max_batch_num = free_thread_data::max_queue_size / 2;
		bt::nint _free_batch_num;
		bt::nint _malloc_batch_num;
	
		bt::weak_rw_lock _td_mtx;						//写的概率低
		std::set<free_thread_data*> _all_ftd;
		std::set<malloc_thread_data*> _all_mtd;

		bt::thread_local_ptr _thread_ftd;
		bt::thread_local_ptr _thread_mtd;

		//统计相关的参数
		bt::nint _statis_buddy_malloc;
		bt::nint _statis_buddy_free;
		bt::nint _statis_free_to_malloc;
		bt::nint _statis_free_all_buffer;
	};
}

#endif