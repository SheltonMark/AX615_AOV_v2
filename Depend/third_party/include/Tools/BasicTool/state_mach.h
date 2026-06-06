
/*
作者：文昌鱼
时间：2011.04.18
说明：状态机

例子
if (check_action())
{
	//设置参数
	action();
}

*/


#ifndef __BASICTOOL_STATE_MACH__
#define __BASICTOOL_STATE_MACH__

#include "public_include.h"

namespace bt
{
	class state_mach
	{
	public:
		state_mach();

	public:
		virtual bool check_action(int act, int seq) = 0;
		virtual bool action(int act, int seq) = 0;
		virtual void on_state_change(int old_state, int new_state) = 0;

		int get_state();

		void* get_local_data();

	protected:
		int _state;
	};
}

#endif //__BASICTOOL_STATE_MACH__

