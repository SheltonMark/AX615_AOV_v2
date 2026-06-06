
/*
作者：文昌鱼
时间：2010.10.08
说明：暂时实现vector,map,list

在优化STL的过程中，STL的优化只是为了性能优化用。编写功能时，可以先实现功能，然后再优化。这样不会禁锢自己的思想，将思路和性能分开来。

先用工具组合成更高级的工具，或程序。然后对工具进行优化。

*/

#ifndef __BASICTOOL_BTSTL__
#define __BASICTOOL_BTSTL__

#include "public_include.h"
#include "pool_allocator.h"
#include "stl.h"

namespace bt
{
	//以const char*为KEY用
	template<class _Ty>
	struct cstr_less
		: public std::binary_function<_Ty, _Ty, bool>
	{
		bool operator()(const _Ty& _Left, const _Ty& _Right) const
		{
			return strcmp(_Left, _Right) < 0;
		}
	};

	template<class TYPE>
	class btvector : public std::vector<TYPE, pool_allocator<TYPE> >
	{
	public:
	};

	template<class KEY, class TYPE, class LESS = std::less<KEY> >
	class btmap : public std::map<KEY, TYPE, LESS, pool_allocator<std::pair<const KEY, TYPE> > >
	{
	public:
	};

	template<class KEY, class TYPE, class LESS = std::less<KEY> >
	class btmultimap : public std::multimap<KEY, TYPE, LESS, pool_allocator<std::pair<const KEY, TYPE> > >
	{
	public:
	};

	template<class KEY, class LESS = std::less<KEY> >
	class btset : public std::set<KEY, LESS, pool_allocator<KEY> >
	{
	public:
	};

	template<class TYPE>
	class btlist : public std::list<TYPE, pool_allocator<TYPE> >
	{
	public:
	};

	template<class TYPE>
	class btdeque : public std::deque<TYPE, pool_allocator<TYPE> >
	{
	public:
	};

	template<class TYPE>
	class btqueue : public std::queue<TYPE, std::deque<TYPE, pool_allocator<TYPE> > >
	{
	public:
	};


	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//构造一些比较常用的类型
	typedef btmap<int, void*>	btmap_int2ptr;
	typedef btmap<int, int>		btmap_int2int;
	typedef btset<void*>		btset_ptr;
	typedef btset<int>			btset_int;
}

#endif //__BASICTOOL_BTSTL__

