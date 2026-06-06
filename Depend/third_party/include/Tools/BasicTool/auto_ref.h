
/*
作者：文昌鱼
时间：2010.09.19
修改：增加智能引用索引类


*/


#ifndef __BASICTOOL_AUTO_REFER__
#define __BASICTOOL_AUTO_REFER__

#include "refer.h"

namespace bt
{
	template<class T>
	class auto_ref
	{
	public:
		inline auto_ref();
		inline auto_ref(T* p, bool inc_ref = true);
		inline auto_ref(const auto_ref& other);
		inline ~auto_ref();

		inline auto_ref& operator=(T* p);
		inline auto_ref& operator=(const auto_ref& other);
		inline bool operator==(T* p) const;
		inline bool operator!=(T* p) const;
		inline bool operator==(const auto_ref& other) const;
		inline bool operator!=(const auto_ref& other) const;
		inline operator T* ();
		inline operator const T* () const;
		inline T* operator->();
		inline const T* operator->() const;

		inline void attach(T* p, bool inc_ref = true);
		inline T* detach(bool dec_ref = true);

	protected:
		T* _p;
	};

	//对象可以和引用索引分开
	template<class T>
	class auto_ref_ex
	{
	public:
		inline auto_ref_ex();
		inline auto_ref_ex(T* p, refer* ref = NULL, bool inc_ref = true);
		inline auto_ref_ex(const auto_ref_ex& other);
		inline ~auto_ref_ex();

		inline auto_ref_ex& operator=(const auto_ref_ex& other);
		inline bool operator==(T* p) const;
		inline bool operator!=(T* p) const;
		inline bool operator==(const auto_ref_ex& other) const;
		inline bool operator!=(const auto_ref_ex& other) const;
		inline operator T* ();
		inline operator const T* () const;
		inline T* operator->();
		inline const T* operator->() const;

		inline void attach(T* p, refer* ref, bool inc_ref = true);
		inline T* detach(refer** ref = NULL, bool dec_ref = true);

	protected:
		refer* _ref;
		T* _p;
	};
}

#include "auto_ref.inl"


#endif //__BASICTOOL_AUTO_REFER__


