/*

作者：文昌鱼
时间：2011.04.16
说明：按时间平滑数值

*/

#ifndef __BASICTOOL_SMOOTH_VALUE__
#define __BASICTOOL_SMOOTH_VALUE__

#include "public_include.h"

namespace bt
{
	class smooth_value
	{
	public:
		smooth_value();
		smooth_value(nint interval);

	public:
		void set_interval(nint interval);
		void add_value(int value);
		int  get_smooth_value();

	protected:
		int				_last_smooth_value;
		nint			_last_calc_tick;
		nint			_interval;
		sint64			_sum_value;
		nint			_last_add;
	};

}

#endif //__BASICTOOL_SMOOTH_VALUE__

