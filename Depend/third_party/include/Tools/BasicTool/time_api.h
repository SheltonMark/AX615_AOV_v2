
#ifndef __BASICTOOL_TIME_API__
#define __BASICTOOL_TIME_API__

#include "public_include.h"

namespace bt
{
	void local_time(time_t t, struct tm* ptm);
	void gm_time(time_t t, struct tm* ptm);		//GMT时间

	int  get_time_zone_sec();					//返回时差对应的秒的偏移
	void update_time_zone_sec();

	time_t mk_time(struct tm* ptm, bool local = true);

	//获取当前时间
	inline time_t get_time();					//从缓存里面获取
	time_t get_time_ex();						//直接从系统时间获取
	inline void get_time_tm(struct tm* ptm);

	//毫秒级的计时。注意开机25天后会循环计时。
	//get_tick:从变量获取，性能好。但精度不高，特别是在CPU高的时候，可能会很差，用在对精度不高的地方。
	//get_tick_ex:直接从系统接口获取。
	inline uint32_t get_tick();
	uint32_t get_tick_ex();

	//微妙级的计时，性能会差点
	uint64_t get_usec_tick();

	//获得微妙级当前时间
	inline uint64_t get_usec_time();			//从缓存读
	uint64_t get_usec_time_ex();				//从系统读

	//时间的另一种表示
	void ZLTimeConvert(ZLTIME_T* zlt, struct tm* ptm);	//ZLTIME_T => tm
	void ZLTimeConvert(struct tm* ptm, ZLTIME_T* zlt);	//tm => ZLTIME_T
	inline ZLTIME& ZLTimeConvert(ZLTIME_T& zlt);
	inline ZLTIME get_zl_time();
}

#include "time_api.inl"

#endif

