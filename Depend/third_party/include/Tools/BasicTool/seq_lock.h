

/*
作者：文昌鱼
时间：2015.02.12
说明：顺序锁

在LINUX内核的顺序锁上，让读和写完全没有冲突。这个锁没有对写作保护，如果有需要可以在set_data外加一个锁

有以下优点：
1. 读和读没有锁冲突
2. 读和写没有锁冲突
3. 读不需要刷新CPU高速缓存

缺点：
1. set_data没有锁保护。如有需要另外加锁。
2. 只保证读写的完整性，不保证CPU间的实时性。也就是说可能读的值是老的。

适合场景：大量读，偶尔写

*/



#ifndef __BASICTOOL_SEQ_LOCK__
#define __BASICTOOL_SEQ_LOCK__

#include "public_include.h"
#include "atomic.h"

namespace bt
{
	template<class TYPE>
	class seq_lock
	{
	public:
		seq_lock();

	public:
		TYPE get_data();
		void set_data(const TYPE& data);

	protected:
		volatile TYPE _data[2];
		atomic _seq_num;
	};
}

#include "seq_lock.inl"

#endif












