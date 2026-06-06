
/*

作者：文昌鱼
时间：2011.04.16
说明：给回调做个统一的结构。没有自动增加或降低索引的功能

用途：
1.回调设置了，不能修改，只有等到析构的时候才释放。这样不用锁保护
2.如果中途需要变化，老的需要释放，那必须在回调调用的线程释放
3.

作者：文昌鱼
时间：2012.01.14
说明：重新调整回调

1.为了回调的时候不频繁加锁解锁、加索引减索引，添加一个20秒延时销毁对象。
但这个隐患，如果回调超过30秒且回调对象被销毁掉且后面又有成员访问，则会崩溃。
2.在回调线程里加延时销毁对象，这个不会有隐患，但有点繁琐
3.提供最安全、效率最低、最容易使用，对于频率低的，可以使用的，每次回调都加锁加索引。

运算符重载：
1.1目、2目操作符重载：返回值 operator 操作符 (参数)
2.函数调用操作符重载：返回值 operator () (参数)
3.类型转化操作符重载(好像也用在if())：operator 类型 ()
4.指针操作符重载：返回值 operator -> ()

回调函数的注意事项：
1.回调函数最好先赋值到临时变量再使用，以免判断和调用的时候值不一样。

*/

#ifndef __BASICTOOL_CALLBACK__
#define __BASICTOOL_CALLBACK__

#include "public_include.h"
#include "refer.h"
#include "delay_del_object.h"

namespace bt
{
	///////////////////////////////////////////////////////////////////////////////////////////////////
	/*
	说明：
	1.只保存回调指针，并做些规范约束。调用回调的时候，会先取出指针存在临时变量上，先判断后调用。
	2.线程不安全。如果要求不严格，也可以不加锁
	3.指针修改和读取都是原子操作
	*/
	//template<class LSNER_TYPE>
	//class call_callback;

	template<class LSNER_TYPE>
	class callback_variable;
	template<class LSNER_TYPE>
	class call_callback
	{
	public:
		inline call_callback(const callback_variable<LSNER_TYPE>& cbv);
		inline call_callback(const call_callback& ccb);
		inline call_callback();

	public:
		inline LSNER_TYPE* operator->() const;
		inline operator LSNER_TYPE* () const;
		inline call_callback& operator=(const callback_variable<LSNER_TYPE>& cbv);
		inline call_callback& operator=(const call_callback& ccb);

	protected:
		LSNER_TYPE*		_lsner;
	};

	template<class LSNER_TYPE>
	class callback_variable
	{
	public:
		typedef call_callback<LSNER_TYPE> call;
		friend class call_callback<LSNER_TYPE>;

	public:
		inline callback_variable(LSNER_TYPE* lsner = NULL);
		inline callback_variable(const callback_variable& cbv);

	public:
		inline void set_lsner(LSNER_TYPE* lsner = NULL);
		inline void* get_lsner() const;				//不建议通过这个接口获取指针然后调用成员函数。推荐用在比较上面。   

		inline callback_variable& operator=(const callback_variable& cbv);
		inline operator void* () const;

	protected:
		LSNER_TYPE*		_lsner;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	/*
	说明：
	1.除了保存回调指针，还保持回调时的附加数据，和引用索引
	2.线程不安全
	3.修改和获取都不是原子操作
	4.提供手动操作索引的接口
	*/

	template<class LSNER_TYPE, class DATA_TYPE>
	class callback_variable_ex;
	template<class LSNER_TYPE, class DATA_TYPE = ptr_int>
	class call_callback_ex
	{
	public:
		inline call_callback_ex(const callback_variable_ex<LSNER_TYPE, DATA_TYPE>& cbv);
		inline call_callback_ex(const call_callback_ex& ccb);
		inline call_callback_ex();

	public:
		inline const DATA_TYPE& get_data() const;

		inline void inc_ref() const;
		inline void dec_ref() const;

		inline LSNER_TYPE* operator->() const;
		inline operator LSNER_TYPE* () const;
		inline call_callback_ex& operator=(const call_callback_ex& ccb);
		inline call_callback_ex& operator=(const callback_variable_ex<LSNER_TYPE, DATA_TYPE>& cbv);

	protected:
		LSNER_TYPE*		_lsner;
		DATA_TYPE		_data;
		refer*			_ref;
	};

	template<class LSNER_TYPE, class DATA_TYPE = ptr_int>
	class callback_variable_ex
	{
	public:
		typedef call_callback_ex<LSNER_TYPE, DATA_TYPE> call;
		friend class call_callback_ex<LSNER_TYPE, DATA_TYPE>;

	public:
		inline callback_variable_ex(LSNER_TYPE* lsner = NULL, DATA_TYPE data = 0, refer* ref = NULL);
		inline callback_variable_ex(const callback_variable_ex& cbv);

	public:
		inline void set_lsner(LSNER_TYPE* lsner = NULL, DATA_TYPE data = 0, refer* ref = NULL);
		inline void* get_lsner() const;
		inline DATA_TYPE& get_data();
		inline const DATA_TYPE& get_data() const;
		inline refer* get_refer() const;

		inline void inc_ref() const;
		inline void dec_ref() const;

		inline callback_variable_ex& operator=(const callback_variable_ex& cbv);
		inline operator void* () const;

	protected:
		LSNER_TYPE*		_lsner;
		DATA_TYPE		_data;
		refer*			_ref;
	};


	///////////////////////////////////////////////////////////////////////////////////////////////////
	/*
	说明：
		1.支持多线程，可选择性锁保护
		2.自动调用引用索引
		3.可以无隐患使用
		4.可能每次回调调用都会加锁、解锁、递增引用索引、递减引用索引，性能比较差，应该在调用不平凡的地方使用
	*/

	template<class LSNER_TYPE, class DATA_TYPE, class LOCK_TYPE>
	class autoref_callback_variable;
	template<class LSNER_TYPE, class DATA_TYPE = ptr_int, class LOCK_TYPE = null_lock>
	class call_autoref_callback
	{
	public:
		call_autoref_callback(const autoref_callback_variable<LSNER_TYPE, DATA_TYPE, LOCK_TYPE>& cbv);
		call_autoref_callback(const call_autoref_callback& ccb);
		inline call_autoref_callback();
		inline ~call_autoref_callback();

	public:
		inline const DATA_TYPE& get_data() const;

		inline LSNER_TYPE* operator->() const;
		inline operator LSNER_TYPE* () const;
		call_autoref_callback& operator=(const call_autoref_callback& ccb);
		call_autoref_callback& operator=(const autoref_callback_variable<LSNER_TYPE, DATA_TYPE, LOCK_TYPE>& cbv);

	protected:
		LSNER_TYPE*		_lsner;
		DATA_TYPE		_data;
		refer*			_ref;
	};

	template<class LSNER_TYPE, class DATA_TYPE = ptr_int, class LOCK_TYPE = null_lock>
	class autoref_callback_variable
	{
	public:
		typedef call_autoref_callback<LSNER_TYPE, DATA_TYPE, LOCK_TYPE> call;
		friend class call_autoref_callback<LSNER_TYPE, DATA_TYPE, LOCK_TYPE>;

	public:
		inline autoref_callback_variable(LSNER_TYPE* lsner = NULL, DATA_TYPE data = 0, refer* ref = NULL, bool inc_ref = true);
		inline autoref_callback_variable(const autoref_callback_variable& cbv);
		inline ~autoref_callback_variable();

	public:
		void set_lsner(LSNER_TYPE* lsner = NULL, DATA_TYPE data = 0, refer* ref = NULL, bool inc_ref = true);
		inline const DATA_TYPE& get_data() const;
		inline void* get_lsner() const;

		inline autoref_callback_variable& operator=(const autoref_callback_variable& cb);
		inline operator void* () const;

	protected:
		BT_FALSE_CONST LOCK_TYPE _mtx;
		LSNER_TYPE*		_lsner;
		DATA_TYPE		_data;
		refer*			_ref;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	/*
	说明：
		是autoref_callback_variable的一种扩展，降低每次调用的成本，提高性能。同时也有些约束。
		约束：
		1.回调函数修改时，老的回调对象延时30秒销毁，所以回调函数的时间最多不能超过30秒。
		2.资源延时销毁可能会造成资源堆积。
		3.回调函数的修改不支持多线程。

		为了解决30秒后销毁存在的隐患和资源堆积的问题，增加了手动添加延时销毁对象。如果增加所有回调函数会调用到
		的所有线程的延时销毁对象，回调对象可以很安全的实时的销毁。

		暂时还没有一个十全十美的方法。
	*/

	template<class LSNER_TYPE, class DATA_TYPE>
	class delayref_callback_variable;
	template<class LSNER_TYPE, class DATA_TYPE = ptr_int>
	class call_delayref_callback
	{
	public:
		inline call_delayref_callback(const delayref_callback_variable<LSNER_TYPE, DATA_TYPE>& cbv);
		inline call_delayref_callback(const call_delayref_callback& ccb);
		inline call_delayref_callback();

	public:
		inline const DATA_TYPE& get_data() const;

		inline LSNER_TYPE* operator->() const;
		inline operator LSNER_TYPE* () const;
		inline call_delayref_callback& operator=(const call_delayref_callback& ccb);
		inline call_delayref_callback& operator=(const delayref_callback_variable<LSNER_TYPE, DATA_TYPE>& cbv);

	protected:
		LSNER_TYPE*		_lsner;
		DATA_TYPE		_data;
	};

	template<class LSNER_TYPE, class DATA_TYPE = ptr_int>
	class delayref_callback_variable
	{
	public:
		typedef call_delayref_callback<LSNER_TYPE, DATA_TYPE> call;
		friend class call_delayref_callback<LSNER_TYPE, DATA_TYPE>;

	public:
		inline delayref_callback_variable(LSNER_TYPE* lsner = NULL, DATA_TYPE data = 0, refer* ref = NULL, bool inc_ref = true);
		inline delayref_callback_variable(const delayref_callback_variable& cbv);
		inline ~delayref_callback_variable();

	public:
		void set_lsner(LSNER_TYPE* lsner = NULL, DATA_TYPE data = 0, refer* ref = NULL, bool inc_ref = true);
		inline const DATA_TYPE& get_data() const;
		inline void* get_lsner() const;

		inline delayref_callback_variable& operator=(const delayref_callback_variable& cb);
		inline operator void* () const;

		inline void onoff_delay_30s_dec(bool onoff);			//默认是开
		inline void add_delay_dec_refer(delay_dec_refer* obj);
		inline void add_delay_dec_refer(const btlist<delay_dec_refer*>& obj_list);

	protected:
		LSNER_TYPE*		_lsner;
		DATA_TYPE		_data;
		refer*			_ref;

		bool			_delay_30s_dec;
		btlist<delay_dec_refer*> _delay_dec_refer;
	};
}

#include "callback.inl"


#endif //__BASICTOOL_CALLBACK__

