
#ifndef __BASICTOOL_BTSTLEXT__
#define __BASICTOOL_BTSTLEXT__

#include "pool_allocator.h"
#include "stl.h"
#include "btstring.h"


	//在各平台下不一样
#if defined(OS_WIN32)

#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS

#include <xhash>
#include <hash_map>

namespace stdext
{
	inline size_t hash_value(const bt::btstring& str)
	{	// hash string to size_t value
		return stdext::hash_value(str.c_str());
	}
}

namespace bt
{
	template<class _Kty,
	class _Pr = std::less<_Kty> >
	class bthashcompare : public stdext::hash_compare<_Kty, _Pr>
	{	// traits class for hash containers
	public:
		enum
		{	// parameters for hash table
			bucket_size = 1,	// 0 < bucket_size					//也就是桶的个数与元素的个数一样
			min_buckets = 8		// min_buckets = 2 ^^ N, 0 < N
		};
	};

	class cstr_hash_compare : public bthashcompare<const char*>
	{	// traits class for hash containers
	public:
		bool operator()(const char* _Keyval1, const char* _Keyval2) const
		{	// test if _Keyval1 ordered before _Keyval2
			return strcmp(_Keyval1, _Keyval2) < 0;
		}
		size_t operator()(const char* _Keyval) const
		{	// test if _Keyval1 ordered before _Keyval2
			return bthashcompare<const char*>::operator ()(_Keyval);
		}
	};

	template<class _Ty>
	class cstrhashmap : public stdext::hash_map<const char*, _Ty, cstr_hash_compare, pool_allocator<std::pair<const char*, _Ty> > >
	{
	public:
	};

	template<class _Kty, class _Ty>
	class bthashmap : public stdext::hash_map<_Kty, _Ty, bthashcompare<_Kty>, pool_allocator<std::pair<const _Kty, _Ty> > >
	{
	public:
	};

	template<class _Kty, class _Ty>
	class bthashmultimap : public stdext::hash_multimap<_Kty, _Ty, bthashcompare<_Kty>, pool_allocator<std::pair<const _Kty, _Ty> > >
	{
	public:
	};


}

#elif defined(OS_LINUX)

#include <ext/hash_map>

namespace __gnu_cxx  
{  
	template<> struct hash<bt::btstring>
	{  
		size_t operator()(const bt::btstring &s) const{ __gnu_cxx::hash<const char*> h;return h(s.c_str()); }
	};
}

namespace bt
{
	//以const char*为KEY用
	template<class _Ty>
	struct cstr_equal_to
		: public std::binary_function<_Ty, _Ty, bool>
	{
		bool operator()(const _Ty& _Left, const _Ty& _Right) const
		{
			return strcmp(_Left, _Right) == 0;
		}
	};

	template<class _Tp>
	class cstrhashmap : public __gnu_cxx::hash_map<const char*, _Tp, __gnu_cxx::hash<const char*>, cstr_equal_to<const char*>, pool_allocator<_Tp> >
	{
	public:
	};

	template<class _Kty, class _Tp>
	class bthashmap : public __gnu_cxx::hash_map<_Kty, _Tp, __gnu_cxx::hash<_Kty>, std::equal_to<_Kty>, pool_allocator<_Tp> >
	{
	public:
	};

	template<class _Kty, class _Tp>
	class bthashmultimap : public __gnu_cxx::hash_multimap<_Kty, _Tp, __gnu_cxx::hash<_Kty>, std::equal_to<_Kty>, pool_allocator<_Tp> >
	{
	public:
	};
}

#endif


#endif

