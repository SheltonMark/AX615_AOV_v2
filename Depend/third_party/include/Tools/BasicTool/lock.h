
/*
作者：文昌鱼
时间：2010.06.13
说明：定义锁的基类


时间：2011.04.16
说明：修改auto_lock，不调用虚函数，提高效率

时间：2011.09.17
心得：读加不加锁的区别：不加锁会得到正在变化时候的值，虽然值变了，但附属的其他变化可能还未完成。
加了锁，只会完成变化后的值。
	  但是对于单个int型，写的时候加锁了，读加不加锁也就无所谓了





时间：2011.09.17
下面一个很好的例子：
//关闭这里有个同步的问题，在多线程里面需要说明下
//on_dev_disable调用后不会真正的创建存在
//1.on_dev_disable后说明下级对象已经关闭
//2.on_dev_disable后不会有新的对象创建，因为其他线程已经识别出_real_enable==1了
//3.on_dev_disable里面必须要有锁，才能实现第2步
//这是个很好的例子，以后可以参考
if (_real_enable)
{
	_real_enable = 1;

	for (btmap<nint, cvi_dev_chn*>::iterator iter = _chn.begin();iter != _chn.end();++iter)
	{
		cvi_dev_chn* chn = iter->second;
		chn->on_dev_disable();
	}

	VERIFY_HISI(HI_MPI_VI_Disable(_devid), {});

	_real_enable = 0;
}

*/


#ifndef __BASICTOOL_LOCK__
#define __BASICTOOL_LOCK__

#include "public_include.h"

namespace bt
{
	class base_lock
	{
	public:
		virtual ~base_lock(){}

	public:
		//给dyna_lock用
		virtual void virtual_lock() = 0;
		virtual bool virtual_try_lock() = 0;
		virtual void virtual_unlock() = 0;

		//这三个函数不是虚函数，为了提高调用性能
		//inline void lock(){};
		//inline bool try_lock(){return true;};
		//inline void unlock(){};

		//自旋次数，只对自旋锁有效
		virtual void set_spin_count(nint spin){};

		//判断是不是空锁
		virtual bool is_null_lock(){return false;}
	};

	class null_lock : public base_lock
	{
	public:
		virtual void virtual_lock(){};
		virtual bool virtual_try_lock(){return true;};
		virtual void virtual_unlock(){};

		inline void lock(){};
		inline bool try_lock(){return true;};
		inline void unlock(){};

		virtual bool is_null_lock(){return true;}
	};

	template<class LOCK_TYPE>
	class auto_lock
	{
	public:
		inline auto_lock(LOCK_TYPE& l, bool init_lock = true);
		inline ~auto_lock();

	public:
		inline void lock();
		inline void unlock();

	private:
		LOCK_TYPE& _lock;
		bool _locked;
	};

} //namespace bt

#include "lock.inl"

#endif //__BASICTOOL_LOCK__

