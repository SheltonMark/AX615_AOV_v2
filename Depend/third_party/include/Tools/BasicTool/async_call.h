
/*
作者：文昌鱼
时间：2011.04.18
说明：统一异步调用，增加效率

以步骤为状态

例子：
if (check_execute(2, 0)
{
	局部变量赋值
	execute(2);
}

加锁的问题很麻烦
在check_execute加锁
在execute_end解锁

step定义：
默认值-1
调用开始，第一步为0，后面递增1、2、......
最后一步又切回-1，可以销毁一切调用资源

*/


#ifndef __BASICTOOL_ASYNC_CALL__
#define __BASICTOOL_ASYNC_CALL__

#include "public_include.h"

namespace bt
{
	class async_call
	{
	public:
		async_call();
		virtual ~async_call();

	//执行接口
	public:
		virtual bool check_execute(int step, int seq);		//判断是否可以执行
		virtual bool execute(int step);						//和check_execute必须配套使用
		virtual void execute_end(int next_step, int next_seq);	//必须在execute退出时调用

		bool start();										//开始执行
		void break_();										//中断执行
		bool to_step(int step, int seq);					//check_execute和execute的组合
		int  get_step();									//大于0表示执行，小于0表示正在中断

	public:
		const static int STEP_DEFAULT_AND_END = 0;			//默认和最后一步的步骤值
		const static int STEP_START = 1;
		const static int STEP_BREAK = -1;
		const static int STEP_NO_NEXT = -1000000;

	//内部接口
	protected:
		void execute_end_next();							//
		void execute_end_next_end();						//
		void execute_end_next_start();
		void execute_end_no_next();

		void execute_end_and_break();

	//数据
	protected:
		int _step;
		int _next_step;
		int _next_seq;
	};

}

#endif //__BASICTOOL_ASYNC_CALL__


