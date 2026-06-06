
/*
作者：文昌鱼
时间：2011.08.15
说明：从TIMER中剥离出来
*/

#ifndef __BASICTOOL_WATCH_TIMER__
#define __BASICTOOL_WATCH_TIMER__

#include "public_include.h"

namespace bt
{
	//简单计时器
	class watch_timer
	{
	public:
		watch_timer(uint interval = BT_MAX_UINT, bool high_precision = false);

	public:
		void reset(uint interval);
		uint get_interval();
		bool check_active();

	protected:
		uint _get_tick();

	protected:
		uint _last_active_time;
		uint _interval;
		bool _high_precision;
	};
}

#endif

