
/*

作者：文昌鱼
时间：2010.11.05
说明：内存池分配器。提高STL性能

使用后，在线程结束的时候记得调用allocator_pool::instance()->thread_uninit();

*/

#ifndef __BASICTOOL_POOL_ALLOCATOR__
#define __BASICTOOL_POOL_ALLOCATOR__

#include "global_mem_pool.h"

namespace bt
{
	// TEMPLATE CLASS pool_allocator
	template<class _Ty>
	class pool_allocator
	{	// generic pool_allocator for objects of class _Ty
	public:
		typedef _Ty value_type;
		typedef value_type  *pointer;
		typedef value_type & reference;
		typedef const value_type  *const_pointer;
		typedef const value_type & const_reference;

		typedef size_t size_type;
#if defined(OS_LINUX)
		typedef long difference_type;
#else
		typedef ptrdiff_t difference_type;
#endif

		template<class _Other>
		struct rebind
		{	// convert an pool_allocator<_Ty> to an pool_allocator <_Other>
			typedef pool_allocator<_Other> other;
		};

		pointer address(reference _Val) const
		{	// return address of mutable _Val
			return (&_Val);
		}

		const_pointer address(const_reference _Val) const
		{	// return address of nonmutable _Val
			return (&_Val);
		}

		pool_allocator()
		{	// construct default pool_allocator (do nothing)

		}

		pool_allocator(const pool_allocator<_Ty>&)
		{	// construct by copying (do nothing)
		}

		template<class _Other>
		pool_allocator(const pool_allocator<_Other>&)
		{	// construct from a related map_allocator (do nothing)
		}

		template<class _Other>
		pool_allocator<_Ty>& operator=(const pool_allocator<_Other>&)
		{	// assign from a related map_allocator (do nothing)
			return (*this);
		}

		~pool_allocator()
		{
		}

		void deallocate(pointer _Ptr, size_type _Count)
		{
			global_mem_pool::static_delete_mem(_Ptr, (uint)(_Count * sizeof(_Ty)));
		}

		pointer allocate(size_type _Count)
		{
			void* p = global_mem_pool::static_new_mem((uint)(_Count * sizeof(_Ty)));
			return (pointer)p;
		}

		pointer allocate(size_type _Count, const void  *)
		{	// allocate array of _Count elements, ignore hint
			return (allocate(_Count));
		}

		void construct(pointer _Ptr, const _Ty& _Val)
		{	// construct object at _Ptr with value _Val
			BT_CALL_CONSTRUCT(_Ptr, value_type, (_Val));
		}

		void destroy(pointer _Ptr)
		{	// destroy object at _Ptr
			BT_CALL_DESTRUCTOR(_Ptr, value_type);
		}

		size_t max_size() const
		{	// estimate maximum array size
			size_t _Count = (size_t)(-1) / sizeof (_Ty);
			return (0 < _Count ? _Count : 1);
		}
	};

	// pool_allocator TEMPLATE OPERATORS
	template<class _Ty, class _Other> inline
		bool operator==(const pool_allocator<_Ty>&, const pool_allocator<_Other>&)
	{	// test for pool_allocator equality (always true)
		return (true);
	}

	template<class _Ty, class _Other> inline
		bool operator!=(const pool_allocator<_Ty>&, const pool_allocator<_Other>&)
	{	// test for pool_allocator inequality (always false)
		return (false);
	}
}

#endif //__BASICTOOL_POOL_ALLOCATOR__

