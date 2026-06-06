
/*

作者：文昌鱼
时间：2011.04.16
说明：计算码流速度

*/

#ifndef __BASICTOOL_CALC_NET_FLUX__
#define __BASICTOOL_CALC_NET_FLUX__

#include "public_include.h"

namespace bt
{
	class calc_net_flux
	{
	public:
		calc_net_flux();
		calc_net_flux(nint interval);

	public:
		void add_bytes(int len);
		int  get_flux();
		void set_interval(nint interval);

	protected:
		int  _last_flux;
		nint _last_calc_tick;
		int  _bytes;
		nint _last_add;
		nint _interval;
	};
}

#endif //__BASICTOOL_CALC_NET_FLUX__



