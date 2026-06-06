
#include "global_resource.h"

namespace bt
{

	inline time_t get_time()
	{
		return global_resource::instance()->get_time();
	}

	inline uint32_t get_tick()
	{
		return global_resource::instance()->get_tick();
	}

	//获得微妙级当前时间
	inline uint64_t get_usec_time()
	{
		return global_resource::instance()->get_usec_time();
	}

	inline void get_time_tm(struct tm* ptm)
	{
		global_resource::instance()->get_time_tm(ptm);
	}

	inline ZLTIME& ZLTimeConvert(ZLTIME_T& zlt)
	{
		return *(ZLTIME*)&zlt;
	}
	
	inline ZLTIME get_zl_time()
	{
		return global_resource::instance()->get_zl_time();
	}
}
