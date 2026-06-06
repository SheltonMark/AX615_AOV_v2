
/*
作者：文昌鱼
时间：2010.06.13
说明：引用索引的基类

时间：2010.09.19
修改：增加单线程使用的引用索引refer_int

时间：2011.04.07
修改：增加inside_refer。针对一个对象超过两个索引添加的。

*/


#ifndef __BASICTOOL_REFER__
#define __BASICTOOL_REFER__

#include "atomic.h"

namespace bt
{
	class refer
	{
	public:
		//初始索引为1
		refer();
		virtual ~refer();

	public:
		virtual int inc_ref();
		//当索引等于0，会delete this
		virtual int dec_ref();

	public:
		int get_ref_count();
		void set_ref_count(int count);

	public:
		//
		virtual void delete_this(refer* my);

	protected:
		atomic _at;
	};

	class refer_int : public refer
	{
	public:
		refer_int();

	public:
		virtual int inc_ref();
		virtual int dec_ref();
	};

	template <class PARENT_REFER = refer>
	class inside_refer : public PARENT_REFER
	{
	public:
		inside_refer();

	protected:
		virtual void delete_this(refer* my);

	public:
		void set_parent_refer(refer* parent_ref);

	protected:
		refer* _parent;
	};

	/*
	外部索引：给使用者使用
	内部索引：给一些功能回调使用
	*/
	template <class REFER = refer, class INSIDE_REFER = REFER>
	class tow_refer : public REFER
	{
	public:
		tow_refer();

	//给子类提供内部索引
	protected:
		refer* get_inside_refer();

	protected:
		virtual void delete_this(refer* my);

		/*
		重载这个回调时，要清空所有对内部索引的引用，防止互引用，导致内存释放不了
		*/
		virtual void on_outside_refer_zero();

	public:
		virtual int inc_ref();
		virtual int dec_ref();
		
	protected:
		inside_refer<INSIDE_REFER> _inside_ref;
		atomic _call_on_outside_refer_zero;
	};
}

#include "refer.inl"

#endif

