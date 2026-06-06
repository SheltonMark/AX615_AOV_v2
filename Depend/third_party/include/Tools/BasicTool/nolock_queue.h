
/*
作者：文昌鱼
时间：2010.07.07
说明：不需要加锁的队列

时间：2011.02.14
说明：做些调整

时间：2011.05.01
说明：在第一个无锁队列的基础上，改进

注意：
	1.TYPE要支持两个操作 =0，==0。如果为结构体，变量判断的依赖项一定要在结构体最后。
	2.进入队列的数据不能==0
	3.适用于大对象

时间：2012.01.28
说明：解决原子队列的重大BUG

解决掉以下两个问题
注意：0判断的字段一定要在接口的末端，因为要读的时候，先判断这个字段为非0，才读其他字段，但是如果不是最后一个字段，为非0时其他字段可能还没拷贝完
注意：atomic_queue有个重大BUG，因为索引比值先走。如果两个读写线程因为值的更新暂停在那里，其他线程可能会一个循环下来，覆盖掉前两个线程等待的值。

时间：2012.11.08
说明：多线程同步不能依赖于赋值顺序，因为CPU缓存同步到内存的顺序无法控制
现将依赖于赋值顺序的锁删除

时间：2012.11.13
说明：可以调用flush_all_cpu_cache刷新强制同步CPU缓存和内存的值。


时间：2015.1.20
说明：优化性能，首要是解决锁冲突，减少锁冲突而导致的线程切换。

*/


#ifndef __BASICTOOL_NOLOCK_QUEUE__
#define __BASICTOOL_NOLOCK_QUEUE__

#include "atomic.h"
#include "lock.h"
#include "mutex.h"
#include "btstl_container.h"
#include "thread_exit.h"
#include "thread_local_ptr.h"

namespace bt
{

	/*
	stl队列+锁
	*/
	template <class LOCK_TYPE, class TYPE>
	class lock_queue
	{
	public:
		lock_queue();
		~lock_queue();

	public:
		void push(TYPE const& data);
		uint pop(TYPE& data);
		void push(TYPE const& data, uint count);
		uint pop(TYPE& data, uint count);
		void set_max_size(uint size);

		inline uint size() const;
		inline bool empty() const;
		inline bool full() const;

	private:
		LOCK_TYPE _mtx;
		std::deque<TYPE> _data;
		uint _max_size;
	};

	/*
	创意来自大华的同事。

	将读写分开，多个线程读/写，需要读/写锁
	MAX_ITEM_SIZE：必须是2的幂，

	与原来的对比：
	1.一个线程读一个线程写可以不加锁，也不需要原子操作。但是写的时候要刷一下CPU缓存。
	2.多个线程操作时，原子操作的性能和自旋锁的性能一样。
	3.降低复杂度。
	*/

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE = 0>
	class two_lock_queue
	{
	public:
		two_lock_queue();
		~two_lock_queue();

	public:
		void push(TYPE const& data);
		uint pop(TYPE& data);
		void push(TYPE const& data, uint count);
		uint pop(TYPE& data, uint count);

		void set_max_size(uint size);

		inline uint size() const;
		inline uint max_size() const;
		inline bool empty() const;
		inline bool full() const;

		//
	protected:
		inline uint _vec_inside_size() const;

	private:
		uint _max_size;
		TYPE* _data;

		POP_LOCK_TYPE _read_mtx;
		volatile uint _read;
		PUSH_LOCK_TYPE _write_mtx;
		volatile uint _write;

		//超出固定大小的时候，防止死循环。
		lock_queue<mutex, TYPE> _reserve;
	};

	//队列里面一个项只放一个值
	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE = long, int SAFE_SIZE = 0>
	class two_lock_queue_value
	{
	public:
		two_lock_queue_value();
		~two_lock_queue_value();

	public:
		void push(TYPE const& data);
		uint pop(TYPE& data);
		void push(TYPE const& data, uint count);
		uint pop(TYPE& data, uint count);
		void set_max_size(uint size);

		inline uint size() const;
		inline uint max_size() const;
		inline bool empty() const;
		inline bool full() const;

		//
	protected:
		inline uint _vec_inside_size() const;

	private:
		uint _max_size;
		TYPE* _data;

		POP_LOCK_TYPE _read_mtx;
		volatile uint _read;
		PUSH_LOCK_TYPE _write_mtx;
		volatile uint _write;

		//超出固定大小的时候，防止死循环。
		lock_queue<mutex, TYPE> _reserve;
	};


	//队列里面一个项只放一个值，而且不会是0或者-1
	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE = void*, int SAFE_SIZE = 0>
	class two_lock_queue_ptr
	{
	public:
		two_lock_queue_ptr();
		~two_lock_queue_ptr();

	public:
		void push(TYPE const& data);
		uint pop(TYPE& data);
		void push(TYPE const& data, uint count);
		uint pop(TYPE& data, uint count);
		void set_max_size(uint size);

		inline uint size() const;
		inline uint max_size() const;
		inline bool empty() const;
		inline bool full() const;

		//
	protected:
		inline uint _vec_inside_size() const;

	private:
		uint _max_size;
		TYPE* _data;

		POP_LOCK_TYPE _read_mtx;
		volatile uint _read;
		PUSH_LOCK_TYPE _write_mtx;
		volatile uint _write;

		//超出固定大小的时候，防止死循环。
		lock_queue<mutex, TYPE> _reserve;
	};

	/*
	针对多个线程入，一个线程出的队列。
	*/
	template <class TYPE>
	class hydra_queue : public tlp_listener
	{
	protected:
		struct queue_item
		{
			TYPE t;
			nint index;
		};
		struct thread_queue
		{
			two_lock_queue<null_lock, null_lock, queue_item> queue;
			bool will_delete;

			const static int MAX_ITEM_BUF = 16;
			queue_item item_buf[MAX_ITEM_BUF];
			nint buf_read;
			nint buf_write;
		};
	public:
		hydra_queue();
		~hydra_queue();

	public:
		void push(TYPE const& data);
		uint pop(TYPE& data);
		void push(TYPE const& data, uint count);
		uint pop(TYPE& data, uint count);
		void set_max_size(uint size);

		inline uint size() const;					//计算出来的值不会很准确
		inline uint max_size() const;
		inline bool empty() const;
		inline bool full() const;

	protected:
		virtual void on_tlp_free_resource(int threadid, void* value, ptr_int user_data);

	private:
		thread_queue* _new_thread_queue();
		inline thread_queue* _get_thread_queue();
		void _dealwith_add_queue();
		void _dealwith_delete_queue(int index);
		uint _pop(TYPE& data);

	private:
		thread_local_ptr _tlp;
		nint _max_size;

		atomic _write_index;
		nint _read_index;
		std::vector<thread_queue*> _all_queue;

		mutex _mtx;
		std::list<thread_queue*> _add;
	};

	/*
	创意来自大华的同事。

	将读写分开，多个线程读/写，需要读/写锁
	MAX_ITEM_SIZE：必须是2的幂，

	与原来的对比：
	1.一个线程读一个线程写可以不加锁，也不需要原子操作。
	2.多个线程操作时，原子操作的性能和自旋锁的性能一样。
	3.降低复杂度。
	*/
	//template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE>
	//class nolock_queue
	//{
	//public:
	//	nolock_queue();
	//	~nolock_queue();

	//public:
	//	void push(TYPE const& data, uint count = 1);
	//	uint pop(TYPE& data, uint count = 1);
	//	void set_max_size(uint size);

	//	inline uint size() const;
	//	inline bool empty() const;
	//	inline bool full() const;

	////
	//protected:
	//	inline uint _vec_inside_size() const;

	//private:
	//	std::vector<TYPE> _data;
	//	POP_LOCK_TYPE _read_mtx;
	//	volatile uint _read;
	//	PUSH_LOCK_TYPE _write_mtx;
	//	volatile uint _write;

	//	//超出固定大小的时候，防止死循环。
	//	lock_queue<spin_mutex, TYPE> _reserve;
	//};

	/*
	加锁的队列，对于多线程操作，冲突的概率还是很大的。
	在第一次实现的队列的基础上，稍加改进。
	*/
	//template <class PUSH_ATOMIC_TYPE, class POP_ATOMIC_TYPE, class TYPE>
	//class atomic_queue
	//{
	//public:
	//	atomic_queue();
	//	~atomic_queue();

	//public:
	//	void push(TYPE const& data, uint count = 1);
	//	uint pop(TYPE& data, uint count = 1);
	//	void set_max_size(uint size);

	//	inline uint size() const;
	//	inline bool empty() const;
	//	inline bool full() const;

	////
	//protected:
	//	inline uint _vec_inside_size() const;
	//	inline nint32 _max_size() const;
	//	inline nint32 _size_mask() const;

	//private:
	//	struct TYPE_EX
	//	{
	//		TYPE v;
	//		nint32 seq;		//_read的高位
	//	};

	//	std::vector<TYPE_EX> _data;
	//	POP_ATOMIC_TYPE _read;
	//	PUSH_ATOMIC_TYPE _write;

	//	//超出固定大小的时候，防止死循环。
	//	lock_queue<spin_mutex, TYPE> _reserve;
	//};


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//纪念第一个无锁队列的实现
	//#if defined(BT_SUPPORT_ATOMIC)
	//	template <int MAX_ITEM_SIZE, class TYPE>
	//	class nolock_queue
	//	{
	//	public:
	//		nolock_queue();
	//		~nolock_queue();
	//
	//	public:
	//		void push(TYPE const& data, uint count = 1);
	//		bool pop(TYPE& data, uint count = 1);
	//
	//		inline uint size() const;
	//		inline bool empty() const;
	//		inline bool full() const;
	//
	//	private:
	//		TYPE _data[MAX_ITEM_SIZE];
	//		atomic _read;
	//		atomic _write;
	//	};
	//#else
	//	template <int MAX_ITEM_SIZE, class TYPE>
	//	class nolock_queue : public lock_queue<mutex, TYPE, btqueue<TYPE> >
	//	{
	//	};
	//#endif

} //namespace bt

#include "nolock_queue.inl"

#endif //__BASICTOOL_NOLOCK_QUEUE__

